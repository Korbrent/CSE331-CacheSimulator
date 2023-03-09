#include "trace.h"
#include "lab2.h"


// Trace file functions

/**
 * Reads the trace file and runs a simulation of the traces on the cache
 * @param traceFile: The trace file to read
 * @param cache: The cache struct to use
 * @return: A struct containing the simulation data
 */
struct SimData readTraceFile(FILE* traceFile, struct Cache cache){
    char *line = malloc(sizeof(char) * 32);
    if(line == NULL){
        printf("Error: Could not allocate memory for line.\n");
        exit(1);
    }
    struct SimData data;
    data.cycles = 0;
    data.totalOps = 0;
    data.loadHits = 0;
    data.loadMisses = 0;
    data.storeHits = 0;
    data.storeMisses = 0;
    

    printf("\nReading trace file...\n");
    while (fgets(line, 32, traceFile)){
        // Print the line
      //* printf("\nRead line: ");
      //* printf("%s\n", line);

        // Get the operation
        char op = line[0];

        // Get the address
        char *address = malloc(sizeof(char) * 8);
        if(address == NULL){
            printf("Error: Could not allocate memory for address.\n");
            exit(1);
        }

        for(int i = 0; i < 8; i++)
            address[i] = line[i + 4];
        // printf("Address: %s\n", address);

        // Convert address to binary
        int *addressBin = hexToBinary(address);
        free(address);

      //* printf("Address in binary: ");
        // for(int i = 0; i < 32; i++)
            // printf("%d", addressBin[i]);
        // printf("\n");

        // Create and run the trace
        struct Trace trace;
        trace.accessType = op;
        trace.address = addressBin;
        trace.cycles = line[13] - '0';
      //* printf("Cycles since last access: %d\n", trace.cycles);

        // Run the trace
        data = runTrace(cache, trace, data);
        // data.cycles += trace.cycles;
        free(addressBin);
    }

    free(line);
    fclose(traceFile);
    return data;
}

/**
 * Runs a trace on the cache
 * @param cache: The cache struct to use
 * @param trace: The trace to run
 * @param simData: The simulation data struct to update
 */
struct SimData runTrace(struct Cache cache, struct Trace trace, struct SimData simData){
    // TODO
  //* printf("\nRunning %s trace on ", trace.accessType == 'l' ? "load" : "store");
    // for(int i = 0; i < 32; i++)
        // printf("%d", trace.address[i]);
    // printf("\n");

    // Get the memory address
    int *address = trace.address;
    unsigned int addressInt = binaryToInt(address, 32);

    // Get the set index
    // printf("Address: %u\n", addressInt);
    // printf("Number of sets: %d\n", cache.blockConfig.numSets);
    int setIndex = addressInt % cache.blockConfig.numSets;
    // printf("Set index: %d \n", setIndex);

    int **set = cache.cache[setIndex];
    int blocksPerSet = cache.config.associativity;
    // printf("Blocks per set: %d\n", blocksPerSet);
    // Check if the address is in the set
    int blockIndex = existsInSet(set, blocksPerSet, address, cache.blockConfig);
    // printf("Block index: %d\n", blockIndex);
    int cycles = trace.cycles;
    switch (trace.accessType)
    {
    case 's':
      //* printf("\nStore op\n");
        // printBlockLayout(cache.blockConfig);
        cycles += store(cache, setIndex, blockIndex, trace);

        if(blockIndex == -1)
            simData.storeMisses++;
        else
            simData.storeHits++;

        break;
    case 'l':
      //* printf("\nLoad op\n");
      //* printBlockLayout(cache.blockConfig);
        cycles += load(cache, setIndex, blockIndex, trace);
        if(blockIndex == -1)
            simData.loadMisses++;
        else
            simData.loadHits++;
        
        break;
    default:
        printf("Error: Invalid access type.\n");
        exit(1);
    }

    simData.cycles += cycles;
    simData.totalOps++;
    return simData;
}
