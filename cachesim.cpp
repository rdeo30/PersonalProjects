#include "cachesim.hpp"
#include <vector>
#include <cmath>
#include <list>
#include <stdlib.h>
#include <stdio.h>
/**
 * Subroutine for initializing the cache simulator. You many add and initialize any global or heap
 * variables as needed.
 * TODO: You're responsible for completing this routine
 */

//sim_stats_t stats;

using namespace std;

int c;
int b_l1;
int s_l1;

int b_l2;
int s_l2;
    
int numOffsetBits_l1;
int numIndexBits_l1;
int numTagBits_l1;

int numOffsetBits_l2;
int numIndexBits_l2;
int numTagBits_l2;

uint64_t cacheTimer; //instead of dealing with data structure for LRU, each block just holds last time it was accessed? this will then determine which one was accessed last.

int numOfWays_l1;
int numOfSets_l1;

int numOfWays_l2;
int numOfSets_l2;

prefetch_algo_t preFetchAlgo_l2;
replacement_policy_t replacementPolicy_l1;
replacement_policy_t replacementPolicy_l2;


//16 blocks for default sim (1kB total size/64B block size)
//8 sets, 2 ways per set (000,001,010,011,100,101,110,111)
struct Block{
    int dirty;
    int valid;
    uint64_t tag; //tag + index is unique for each block
    int isPreFetch; //metadata tag for prefetched blocks

};

bool l2CacheEnabled;

//Each "Set" is an array of "Blocks" (the ways).
// A cache is an array of "Sets". 
Block **cache_l1; 

Block **cache_l2; 

vector<list<int> > l1_lru;
vector<list<int> > l2_lru;

//The L1 cache uses a write-back, write-allocate (WBWA) policy and uses MIP as its insertion policy
//The L2 cache uses a write-through, write no allocate (WTWNA) write policy and LIP as its insertion policy.


void sim_setup(sim_config_t *config) {
    cacheTimer = 0;
    
    //L1 Cache set-up:

    s_l1 = config->l1_config.s;
    b_l1 = config->l1_config.b;

    numOffsetBits_l1 = config->l1_config.b;

    numIndexBits_l1 = config->l1_config.c - config->l1_config.b - config->l1_config.s;
    
    numTagBits_l1 = 64 - numOffsetBits_l1 - numIndexBits_l1;

    replacementPolicy_l1 = config->l1_config.replace_policy;

    numOfSets_l1 = (uint64_t) (1) << numIndexBits_l1; //2^#ofIndexBits
    numOfWays_l1 = (1) << config->l1_config.s; //2^s

    cache_l1 = (Block **)malloc(numOfSets_l1 * sizeof(Block *));

    l1_lru.resize(numOfSets_l1);


    for (int i = 0; i < numOfSets_l1; i++) {
        //malloc the ways for each set (2 ways for 8 sets)
        cache_l1[i] = (Block*)malloc(numOfWays_l1 * sizeof(Block));
        
        //[numOfSets(0->7)][numOfWays(0->2)]
        for (int j = 0; j < numOfWays_l1; j++) {
            cache_l1[i][j].valid = 0;
            cache_l1[i][j].dirty = 0;
            cache_l1[i][j].tag = 0;
            cache_l1[i][j].isPreFetch = 0;

            l1_lru[i].push_back(j);
        }
    }

    l2CacheEnabled = false;

    //L2 Cache set-up:

    if(!config->l2_config.disabled) {

        l2CacheEnabled = true;

        replacementPolicy_l2 = config->l2_config.replace_policy;
        preFetchAlgo_l2 = config->l2_config.prefetch_algorithm;

        s_l2 = config->l2_config.s;
        b_l2 = config->l2_config.b;

        numOffsetBits_l2 = config->l2_config.b;

        numIndexBits_l2 = config->l2_config.c - config->l2_config.b - config->l2_config.s;
        
        numTagBits_l2 = 64 - numOffsetBits_l2 - numIndexBits_l2;

        numOfSets_l2 = (uint64_t) (1) << numIndexBits_l2; //2^#ofIndexBits
        numOfWays_l2 = (1) << config->l2_config.s; //2^s

        cache_l2 = (Block **)malloc(numOfSets_l2 * sizeof(Block *));

        l2_lru.resize(numOfSets_l2);


        for (int i = 0; i < numOfSets_l2; i++) {
            //malloc the ways for each set (2 ways for 8 sets)
            cache_l2[i] = (Block*)malloc(numOfWays_l2 * sizeof(Block));
        
            //[numOfSets(0->7)][numOfWays(0->2)]
            for (int j = 0; j < numOfWays_l2; j++) {
                cache_l2[i][j].valid = 0;
                cache_l2[i][j].dirty = 0;
                cache_l2[i][j].tag = 0;
                cache_l2[i][j].isPreFetch = 0;

                l2_lru[i].push_back(j);
            }
        }

    }

}

/**
 * Subroutine that simulates the cache one trace event at a time.
 * TODO: You're responsible for completing this routine
 */
void sim_access(char rw, uint64_t addr, sim_stats_t* stats) {
    cacheTimer++;

    uint64_t addressIndex = (addr >> numOffsetBits_l1) & ((1ULL << numIndexBits_l1) - 1);
    uint64_t addressTag = (addr >> (numOffsetBits_l1 + numIndexBits_l1)); //& ((1 << numTagBits_l1) - 1);

    stats->accesses_l1++;


    //for searching through all ways in a set
    for(int i = 0; i < numOfWays_l1; i++) {
        if(cache_l1[addressIndex][i].tag == addressTag && cache_l1[addressIndex][i].valid) {
            //handle cache hit
            
            l1_lru[addressIndex].remove(i);
            l1_lru[addressIndex].push_front(i); //move way to the front (becomes MRU)
            stats->hits_l1++;
            if (rw == WRITE) {
                cache_l1[addressIndex][i].dirty = 1;
                stats->writes++;
            } else {
                stats->reads++;
            }
            return;
        } 
    }

    //if we searched and couldn't find a valid and matching tag, consider the fact we missed:
    stats->misses_l1++;

    if (rw == WRITE) {
        stats->writes++;
    } else {
        stats->reads++;
    }

    if(!l2CacheEnabled) {
        stats->reads_l2++;
        stats->read_misses_l2++;
    } else {

        uint64_t addressIndex_l2 = (addr >> numOffsetBits_l2) & ((1ULL << numIndexBits_l2) - 1);
        uint64_t addressTag_l2 = (addr >> (numOffsetBits_l2 + numIndexBits_l2)); //& ((1 << numTagBits_l1) - 1);

        stats->reads_l2++;

        int l2HitWay = -1;

        for(int i = 0; i < numOfWays_l2; i++) {
            if(cache_l2[addressIndex_l2][i].tag == addressTag_l2 && cache_l2[addressIndex_l2][i].valid) {
                l2HitWay = i;

                stats->read_hits_l2++;

                if (cache_l2[addressIndex_l2][i].isPreFetch) {
                    stats->prefetch_hits_l2++;
                    cache_l2[addressIndex_l2][i].isPreFetch = 0;
                }

                //update LRU:

                l2_lru[addressIndex_l2].remove(i);
                l2_lru[addressIndex_l2].push_front(i);
                break;
            }
        }

        if(l2HitWay == -1) {
            stats->read_misses_l2++;

            int l2wayToEvict = -1;

            for(int i = 0; i < numOfWays_l2; i++) {
                if(!cache_l2[addressIndex_l2][i].valid) {
                    l2wayToEvict = i;
                    break;
                }
            }

            if (l2wayToEvict == -1) {
                l2wayToEvict = l2_lru[addressIndex_l2].back();
                
                if (cache_l2[addressIndex_l2][l2wayToEvict].isPreFetch) {
                    stats->prefetch_misses_l2++;
                }
            }

            cache_l2[addressIndex_l2][l2wayToEvict].tag = addressTag_l2;
            cache_l2[addressIndex_l2][l2wayToEvict].valid = 1;
            cache_l2[addressIndex_l2][l2wayToEvict].isPreFetch = 0;
            cache_l2[addressIndex_l2][l2wayToEvict].dirty = 0;

            l2_lru[addressIndex_l2].remove(l2wayToEvict);

            if(replacementPolicy_l2 == REPLACEMENT_POLICY_LIP) {
                l2_lru[addressIndex_l2].push_back(l2wayToEvict);
                for(int i = 0; i < numOfWays_l2; i++) {
                    //push back invalid ways even though this wau was the least recently used
                    if(!cache_l2[addressIndex_l2][i].valid) {
                        l2_lru[addressIndex_l2].remove(i);
                        l2_lru[addressIndex_l2].push_back(i);
                    }
                }
            } else {
                l2_lru[addressIndex_l2].push_front(l2wayToEvict);
                
            }

            if(preFetchAlgo_l2 == PREFETCH_PLUS_ONE) {
                uint64_t nextAddr = addr + (1ULL << numOffsetBits_l2);

                bool l1Present = false;
                uint64_t preFetchL1Index = (nextAddr >> numOffsetBits_l1) & ((1ULL << numIndexBits_l1) - 1);
                uint64_t preFetchL1Tag = (nextAddr >> (numOffsetBits_l1 + numIndexBits_l1));

                for(int i = 0; i < numOfWays_l1; i++) {
                    if(cache_l1[preFetchL1Index][i].tag == preFetchL1Tag && cache_l1[preFetchL1Index][i].valid) {
                        l1Present = true;
                        break;
                    }
                }
                
                if(!l1Present) {
                    bool l2Present = false;
                    uint64_t preFetchL2Index = (nextAddr >> numOffsetBits_l2) & ((1ULL << numIndexBits_l2) - 1);
                    uint64_t preFetchL2Tag = (nextAddr >> (numOffsetBits_l2 + numIndexBits_l2));

                    for(int i = 0; i < numOfWays_l2; i++) {
                        if(cache_l2[preFetchL2Index][i].tag == preFetchL2Tag && cache_l2[preFetchL2Index][i].valid) {
                            l2Present = true;
                            break;
                        }
                    }

                    if(!l2Present) {
                        //if there were no duplicate pre fetches found in L1 or L2, then we must issue a prefetch in l2
                        stats->prefetches_issued_l2++;

                        int preFetchWayToRemove = -1;
                        //remove invalid (coldstart ways) first
                        for(int i = 0; i < numOfWays_l2; i++) {
                            if(!cache_l2[preFetchL2Index][i].valid) {
                                preFetchWayToRemove = i;
                                break;
                            }
                        }
                        //if none invalid, kick out least recently used
                        if (preFetchWayToRemove == -1) {
                            preFetchWayToRemove = l2_lru[preFetchL2Index].back();
                            if (cache_l2[preFetchL2Index][preFetchWayToRemove].isPreFetch) {
                                stats->prefetch_misses_l2++;
                            }
                        }


                        cache_l2[preFetchL2Index][preFetchWayToRemove].tag = preFetchL2Tag;
                        cache_l2[preFetchL2Index][preFetchWayToRemove].valid = 1;
                        cache_l2[preFetchL2Index][preFetchWayToRemove].isPreFetch = 1;
                        cache_l2[preFetchL2Index][preFetchWayToRemove].dirty = 0;

                        l2_lru[preFetchL2Index].remove(preFetchWayToRemove);

                        if(replacementPolicy_l2 == REPLACEMENT_POLICY_LIP) {
                            l2_lru[preFetchL2Index].push_back(preFetchWayToRemove);
                            for(int i = 0; i < numOfWays_l2; i++) {
                                if(!cache_l2[preFetchL2Index][i].valid) {
                                    l2_lru[preFetchL2Index].remove(i);
                                    l2_lru[preFetchL2Index].push_back(i);
                                }
                            }
                        } else {
                            l2_lru[preFetchL2Index].push_front(preFetchWayToRemove);
                            
                        }
                        
                    }
                }
            }
        }
    }


    int wayToRemove = l1_lru[addressIndex].back();

    
    if(cache_l1[addressIndex][wayToRemove].dirty && cache_l1[addressIndex][wayToRemove].valid) {
        stats->write_backs_l1++;

        if(!l2CacheEnabled) {
            stats->writes_l2++;
        } else {
            uint64_t wbAddr = (cache_l1[addressIndex][wayToRemove].tag << (numIndexBits_l1 + numOffsetBits_l1)) | (addressIndex << numOffsetBits_l1);

            uint64_t wbL2Index = (wbAddr >> numOffsetBits_l2) & ((1ULL << numIndexBits_l2) - 1);
            uint64_t wbL2Tag = (wbAddr >> (numOffsetBits_l2 + numIndexBits_l2));

            stats->writes_l2++;

            for(int i = 0; i < numOfWays_l2; i++) {
                if(cache_l2[wbL2Index][i].tag == wbL2Tag && cache_l2[wbL2Index][i].valid) {
                    
                    //prefetch hit
                    if (cache_l2[wbL2Index][i].isPreFetch) {
                        stats->prefetch_hits_l2++;
                        cache_l2[wbL2Index][i].isPreFetch = 0;
                    }
                    //Move to MRU
                    l2_lru[wbL2Index].remove(i);
                    l2_lru[wbL2Index].push_front(i);
                    break;
                }
            }

        }
    }

    //update way
    cache_l1[addressIndex][wayToRemove].valid = 1;
    cache_l1[addressIndex][wayToRemove].dirty = (rw == WRITE) ? 1 : 0;
    cache_l1[addressIndex][wayToRemove].tag = addressTag;
    cache_l1[addressIndex][wayToRemove].isPreFetch = 0;
    
    //That way becomes most recently used.
    l1_lru[addressIndex].remove(wayToRemove);
    l1_lru[addressIndex].push_front(wayToRemove);

}

/**
 * Subroutine for cleaning up any outstanding memory operations and calculating overall statistics
 * such as miss rate or average access time.
 * TODO: You're responsible for completing this routine
 */
void sim_finish(sim_stats_t *stats) {

    double hitTime_l1 = L1_HIT_TIME_CONST + (L1_HIT_TIME_PER_S * s_l1);
    double hitTime_l2 = 0; 
    if(l2CacheEnabled) {
        hitTime_l2 = L2_HIT_TIME_CONST + (L2_HIT_TIME_PER_S * s_l2);
    }

    stats->hit_ratio_l1 = (double) (stats->hits_l1) / (double) (stats->accesses_l1);
    stats->miss_ratio_l1 = (double) (stats->misses_l1) / (double) (stats->accesses_l1);

    stats->read_miss_ratio_l2 = 1.0;
    stats->read_hit_ratio_l2 = 0.0;
    if(l2CacheEnabled) {
        stats->read_miss_ratio_l2 = (double) (stats->read_misses_l2) / (double) (stats->reads_l2);
        stats->read_hit_ratio_l2 = (double) stats->read_hits_l2 / (double) stats->reads_l2;
    }

    double bytesPerBlock = (1) << b_l1;

    //DRAM_TIME = DRAM_AT + (DRAM_AT_PER_WORD × WORDS_PER_BLOCK)
    double dramAccessTime = DRAM_AT + (DRAM_AT_PER_WORD * (bytesPerBlock / WORD_SIZE));

    //AAT = HT + (MR * MP)

    stats->avg_access_time_l2 = hitTime_l2 + (stats->read_miss_ratio_l2 * dramAccessTime);
    stats->avg_access_time_l1 = hitTime_l1 + ((stats->miss_ratio_l1) * stats->avg_access_time_l2);


}