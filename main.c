/*************************************************************************
/
/ filename: main.c
/
/ description: This file is the main function for the cache simulator.
/              It calls the main functions of the cache and trace files.
/              It also prints the results of the simulation.
/
/ author: Shelley, Korbin
/
/ class: CSE 331
/ instructor: Zheng
/ assignment: Lab #2
/
/ assigned: 2/28/2023
/ due: 3/10/2023
/
/************************************************************************/
#include "lab2.h"
#include "cache.h"
#include "trace.h"
#include <string.h>

void writeDataToFile(char *fileName, struct SimData output);

// Cache simulator main function
int main(int argc, char *argv[]){

    // Check for correct number of arguments
    if(argc != 3){
        printf("Error: Incorrect number of arguments.\n"
                "Usage: ./cachesim <config file> <trace file>\n");
        return 1;
    }

    // Open the config file
    FILE *configFile = fopen(argv[1], "r");
    if(!configFile){
        printf("Error: Config file not found.\n");
        return 1;
    }

    // Read the config file
    int* config = readConfig(configFile);

    /*
     config[0]
     // Block size for the cache in bytes
     // Must be a power of 2
    
     config[1]
     // Associativity of the cache; 0 = fully-associative, 1 = direct-mapped, n = n-way associative
     // Must be a power of 2

     config[2]
     // Total size of the cache in kilobytes, not including overhead such as tag bits
     // Must be a power of 2
     
     config[3]
     // Replacement policy; 0 = random replacement, 1 = FIFO

     config[4]
     // Number of cycles penalized on a cache miss, can be any positive integer

     config[5]
     // Write policy; 0 = write-through, 1 = write-back
     // 0 = no-write-allocate, 1 = write-allocate
     */

    printf("Line size:          %d B\n", config[0]);
    printf("Associativity:      %d-way mapping\n", config[1]);
    printf("Data size:          %d KB\n", config[2]);
    printf("Replacement policy: %d\n", config[3]);
    printf("Miss penalty:       %d\n", config[4]);
    printf("Write policy:       %d\n", config[5]);
    // sleep(5);

    // Create the cache
    struct Cache cache;
    cache = createCache(config);
    free(config);

    printf("Total Blocks:       %d blocks\n", cache.config.numBlocks);
    printf("Total Sets:         %d sets\n", cache.blockConfig.numSets);
    printf("Blocks/Set:         %d blocks\n", cache.config.associativity);
    printf("Bits/Block:         %d bits\n", cache.blockConfig.blockSize);
    // printBlockLayout(cache.blockConfig);
    // printCache(cache);

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

    writeDataToFile(argv[2], output);
    return 0;
}

/**
 * Writes the output data to a file named "{fileName}.out".
 * @param fileName The name of the trace file.
 * @param output The output data from the simulation.
 */
void writeDataToFile(char *fileName, struct SimData output){
    char outfile[strlen(fileName) + 5];
    strcpy(outfile, fileName);
    strcat(outfile, ".out");
    
    FILE *file = fopen(outfile, "w");
    if(!file){
        printf("Error: File not writable.\n");
        return;
    }

    float hit_rate = (float)(output.loadHits + output.storeHits) / (float)output.totalOps;
    int loadOps = output.loadHits + output.loadMisses;
    float ld_hit_rate = (float)output.loadHits / (float)loadOps;
    int storeOps = output.storeHits + output.storeMisses;
    float st_hit_rate = (float)output.storeHits / (float)storeOps;

    printf("Hit rate:           %.2f%%\n", hit_rate * 100);
    printf("Load hit rate:      %.2f%%\n", ld_hit_rate * 100);
    printf("Store hit rate:     %.2f%%\n", st_hit_rate * 100);

    if(loadOps + storeOps != output.totalOps)
        printf("Error: Mismatch in op count.\n");

    float avg_access_time = (float)output.cycles / (float)output.totalOps;
    printf("Average access time: %.2f cycles\n", avg_access_time);

    fprintf(file, "%f\n%f\n%f\n%d\n%f\n",//"%.2f\n%.2f\n%.2f\n%d\n%.2f\n",
            hit_rate,
            ld_hit_rate,
            st_hit_rate,
            output.cycles,
            avg_access_time);

    fclose(file);
    return;
}