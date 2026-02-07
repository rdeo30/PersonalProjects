#include <iostream>
#include "cachesim.hpp"
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

sim_stats_t stats;

int numOffsetBits;
int numIndexBits;
int numTagBits;
int main() {

    sim_config_t config = DEFAULT_SIM_CONFIG;

    
    

    sim_setup(&config);

    stats.hits_l1++;
    stats.hits_l1++;
    printf("%d",stats.hits_l1);


    char test = 'W';

}    


void sim_setup(sim_config_t *config) {

    memset(&stats, 0, sizeof(sim_stats_t));

    numOffsetBits = config->l1_config.b;
    numIndexBits = config->l1_config.c - config->l1_config.b - config->l1_config.s;
    numTagBits = 64 - numOffsetBits - numIndexBits;

    int numOfSets = (1) << numIndexBits; //2^#ofIndexBits
    int numOfWays = (1) << config->l1_config.s; //2^s

    uint64_t address = 0x0000560feb6d7f70;

    uint64_t addressIndex = (address >> numOffsetBits) & ((1ULL << numIndexBits) - 1);
    uint64_t addressTag = (address >> (numOffsetBits + numIndexBits)); //& ((1 << numTagBits) - 1);

    cout << numOffsetBits << endl;
    cout << numIndexBits << endl;
    cout << numTagBits << endl;
    cout << addressIndex << endl;
    cout << addressTag << endl;
    


}

