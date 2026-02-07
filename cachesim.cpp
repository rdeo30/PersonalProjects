#include "cachesim.hpp"
#include <vector>
#include <cmath>
/**
 * Subroutine for initializing the cache simulator. You many add and initialize any global or heap
 * variables as needed.
 * TODO: You're responsible for completing this routine
 */

//sim_stats_t stats;
    
int numOffsetBits;
int numIndexBits;
int numTagBits;

uint64_t cacheTimer; //instead of dealing with data structure for LRU, each block just holds last time it was accessed? this will then determine which one was accessed last.

int numOfWays;
int numOfSets;

prefetch_algo_t preFetchAlgo;
replacement_policy_t replacementPolicy;


//16 blocks for default sim (1kB total size/64B block size)
//8 sets, 2 ways per set (000,001,010,011,100,101,110,111)
struct Block{
    int dirty;
    int valid;
    uint64_t tag; //tag + index is unique for each block
    int lruPosition;

};

//Each "Set" is an array of "Blocks" (the ways).
// A cache is an array of "Sets". 
Block **cache; 


//The L1 cache uses a write-back, write-allocate (WBWA) policy and uses MIP as its insertion policy

using namespace std;

void sim_setup(sim_config_t *config) {
    cacheTimer = 0;
    //memset(&stats, 0, sizeof(sim_stats_t));

    numOffsetBits = config->l1_config.b;

    numIndexBits = config->l1_config.c - config->l1_config.b - config->l1_config.s;
    
    numTagBits = 64 - numOffsetBits - numIndexBits;

    preFetchAlgo = config->l1_config.prefetch_algorithm;

    replacementPolicy = config->l1_config.replace_policy;

    numOfSets = (uint64_t) (1) << numIndexBits; //2^#ofIndexBits
    numOfWays = (1) << config->l1_config.s; //2^s

    cache = (Block **)malloc(numOfSets * sizeof(Block *));


    for (int i = 0; i < numOfSets; i++) {
        //malloc the ways for each set (2 ways for 8 sets)
        cache[i] = (Block*)malloc(numOfWays * sizeof(Block));
        
        //[numOfSets(0->7)][numOfWays(0->2)]
        for (int j = 0; j < numOfWays; j++) {
            cache[i][j].valid = 0;
            cache[i][j].tag = 0;
            cache[i][j].lruPosition = 0;
        }
    }

}

/**
 * Subroutine that simulates the cache one trace event at a time.
 * TODO: You're responsible for completing this routine
 */
void sim_access(char rw, uint64_t addr, sim_stats_t* stats) {
    cacheTimer++;

    uint64_t addressIndex = (addr >> numOffsetBits) & ((1ULL << numIndexBits) - 1);
    uint64_t addressTag = (addr >> (numOffsetBits + numIndexBits)); //& ((1 << numTagBits) - 1);

    stats->accesses_l1++;


    //for searching through all ways in a set
    for(int i = 0; i < numOfWays; i++) {
        if(cache[addressIndex][i].tag == addressTag && cache[addressIndex][i].valid) {
            //handle cache hit
            cache[addressIndex][i].lruPosition = cacheTimer;
            stats->hits_l1++;
            if (rw == WRITE) {
                cache[addressIndex][i].dirty = 1;
                stats->writes++;
            } else {
                stats->reads++;
            }
            return;
        } 
    }

    //if we searched and couldn't find a valid and matching tag, consider the fact we missed:
    stats->misses_l1++;
    //Check invalid bits first? maybe cold start cache and cache is filled with garbage that can be written over
    for(int i = 0; i < numOfWays; i++) {
        if(cache[addressIndex][i].valid != 1) {
            cache[addressIndex][i].tag = addressTag;
            cache[addressIndex][i].valid = 1;
            cache[addressIndex][i].lruPosition = cacheTimer;
            if (rw == WRITE) {
                stats->writes++;
                cache[addressIndex][i].dirty = 1;
            } else {
                stats->reads++;
                cache[addressIndex][i].dirty = 0;
            }
            return;
        }
    }

    //if that wasn't the case, then we have a full set, and must evict a block based on LRU, and insert using MRU insertion policy
    uint64_t minTime = UINT64_MAX;
    int wayToRemove = -1;
    for(int i = 0; i < numOfWays; i++) {
        if(cache[addressIndex][i].lruPosition < minTime) {
            minTime = cache[addressIndex][i].lruPosition;
            wayToRemove = i;
        }
    }

    if(cache[addressIndex][wayToRemove].dirty == 1) {
        stats->write_backs_l1++;
        //handle write to l2 here later possibly
    }

    if (rw == WRITE) {
        stats->writes++;
    } else {
        stats->reads++;
    }


    cache[addressIndex][wayToRemove].tag = addressTag;
    cache[addressIndex][wayToRemove].valid = 1;
    cache[addressIndex][wayToRemove].lruPosition = cacheTimer;
    cache[addressIndex][wayToRemove].dirty = rw == WRITE ? 1 : 0;
    

}

/**
 * Subroutine for cleaning up any outstanding memory operations and calculating overall statistics
 * such as miss rate or average access time.
 * TODO: You're responsible for completing this routine
 */
void sim_finish(sim_stats_t *stats) {

    stats->hit_ratio_l1 = (double) (stats->hits_l1) / (double) (stats->accesses_l1);
    stats->miss_ratio_l1 = (double) (stats->misses_l1) / (double) (stats->accesses_l1);

    //AAT = HT + (MR * MP)
    

}