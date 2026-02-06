#include "cachesim.hpp"
#include <vector>
#include <cmath>
/**
 * Subroutine for initializing the cache simulator. You many add and initialize any global or heap
 * variables as needed.
 * TODO: You're responsible for completing this routine
 */

int numOffsetBits;
int numIndexBits;
int numTagBits;

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
            cache[i][j].valid = false;
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

    uint64_t addressIndex = (addr >> numOffsetBits) & ((1ULL << numIndexBits) - 1);
    uint64_t addressTag = (addr >> (numOffsetBits + numOffsetBits)) & ((1 << numTagBits) - 1);

    


    //for searching through all ways in a set
    for(int i = 0; i < numOfWays; i++) {
        if(cache[addressIndex][i].tag == addressTag && cache[addressIndex][i].valid) {
            //handle cache hit
        }
    }

}

/**
 * Subroutine for cleaning up any outstanding memory operations and calculating overall statistics
 * such as miss rate or average access time.
 * TODO: You're responsible for completing this routine
 */
void sim_finish(sim_stats_t *stats) {

}