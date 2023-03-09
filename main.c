#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "lab2.h"
#include "cache.h"
#include "trace.h"

// Cache simulator main function
int main(int argc, char *argv[]){

    // Block size for the cache in bytes
    unsigned int LINE_SIZE = -1; // Must be a power of 2

    // Associativity of the cache; 0 = fully-associative, 1 = direct-mapped, n = n-way associative
    unsigned int ASSOCIATIVITY = -1; // Must be a power of 2

    // Total size of the cache in kilobytes, not including overhead such as tag bits
    unsigned int DATA_SIZE = -1; // Must be a power of 2

    // Replacement policy; 0 = random replacement, 1 = FIFO
    unsigned int REPLACEMENT_POLICY = -1;

    // Number of cycles penalized on a cache miss
    unsigned int MISS_PENALTY = -1; // Can be any positive integer

    // Write policy; 0 = write-through, 1 = write-back
    // 0 = no-write-allocate, 1 = write-allocate
    unsigned int WRITE_POLICY = -1; 

    // Check for correct number of arguments
    if(argc != 3){
        printf("Error: Incorrect number of arguments.\n"
                "Usage: ./cachesim <config file> <trace file>\n");
        return 1;
    }

    // Open the config file
    FILE *configFile = fopen(argv[1], "r");
    if(configFile == NULL){
        printf("Error: Config file not found.\n");
        return 1;
    }

    // Read the config file
    int* config = readConfig(configFile);

    LINE_SIZE = config[0];
    ASSOCIATIVITY = config[1];
    DATA_SIZE = config[2];
    REPLACEMENT_POLICY = config[3];
    MISS_PENALTY = config[4];
    WRITE_POLICY = config[5];

    printf("Line size:          %d B\n", LINE_SIZE);
    printf("Associativity:      %d-way mapping\n", ASSOCIATIVITY);
    printf("Data size:          %d KB\n", DATA_SIZE);
    printf("Replacement policy: %d\n", REPLACEMENT_POLICY);
    printf("Miss penalty:       %d\n", MISS_PENALTY);
    printf("Write policy:       %d\n", WRITE_POLICY);

    // Create the cache
    struct Cache cache;
    cache = createCache(config);
    free(config);
    printf("Total Blocks:       %d blocks\n", cache.config.numBlocks);
    printf("Total Sets:         %d sets\n", cache.blockConfig.numSets);
    printf("Blocks/Set:         %d blocks\n", cache.config.associativity);
    printf("Bits/Block:         %d bits\n", cache.blockConfig.blockSize);
    
    FILE *traceFile = fopen(argv[2], "r");
    if(traceFile == NULL){
        printf("Error: Trace file not found.\n");
        return 1;
    }

    // Read the trace file
    struct SimData output = readTraceFile(traceFile, cache);
    
    freeCache(cache);

    // Print the output
    printf("Total operations:   %d\n", output.totalOps);
    printf("Load hits:          %d\n", output.loadHits);
    printf("Store hits:         %d\n", output.storeHits);
    printf("Load misses:        %d\n", output.loadMisses);
    printf("Store misses:       %d\n", output.storeMisses);
    printf("Total cycles:       %d\n", output.cycles);

    return 0;
}

