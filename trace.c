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
        printf("\nRead line: ");
        printf("%s\n", line);

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
        printf("Address: %s\n", address);

        // Convert address to binary
        int *addressBin = hexToBinary(address);
        free(address);

        printf("Address in binary: ");
        for(int i = 0; i < 32; i++)
            printf("%d", addressBin[i]);
        printf("\n");

        // Create and run the trace
        struct Trace trace;
        trace.accessType = op;
        trace.address = addressBin;

        trace.cycles = 0;
        for(int i = 13; (line[i] >= '0') && (line[i] <= '9'); i++)
            trace.cycles = (trace.cycles * 10) + (line[i] - '0');
        printf("Cycles since last access: %d\n", trace.cycles);

        // Run the trace
        data = runTrace(cache, trace, data);
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
    printf("\nRunning %s trace on ", trace.accessType == 'l' ? "load" : "store");
    for(int i = 0; i < 32; i++)
        printf("%d", trace.address[i]);
    printf("\n");

    // Get the memory address
    int *address = trace.address;
    unsigned int addressInt = binaryToInt(address, 32);

    // Get the set index
    int setIndex = addressInt % cache.blockConfig.numSets;

    int **set = cache.cache[setIndex];
    int blocksPerSet = cache.config.associativity;

    // Check if the address is in the set
    int blockIndex = existsInSet(set, blocksPerSet, address, cache.blockConfig);

    int cycles = trace.cycles;

    switch (trace.accessType)
    {
    case 's':
        cycles += store(cache, setIndex, blockIndex, trace);

        if(blockIndex == -1)
            simData.storeMisses++;
        else
            simData.storeHits++;

        break;
    case 'l':
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
    printf("\n"); printSet(set, blocksPerSet, cache.blockConfig.blockSize, setIndex);

    simData.cycles += cycles;
    simData.totalOps++;
    return simData;
}
