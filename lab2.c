#include "lab2.h"
#include <math.h>
/**
 * Functions for the cache simulator
 * @author: Kor Shelley
 * @date: 2023-02-28
 * @version: 1.0
 */

// Conversion functions
/**
 * Calculates if a number is a power of 2 
 * @param x: The number to check
 * @return: 1 if x is a power of 2, 0 otherwise
 */ 
int isPowerOfTwo(int x){
    return (x != 0) && ((x & (x - 1)) == 0);
}

/**
 * Converts an integer to binary
 * @param n: The integer to convert
 * @param bits: The number of bits to use or 0 to use the minimum number of bits
 * @return: An array of the binary representation of n
 */
int* intToBinary(int n, int bits){
    int b = ceil(log2(n));
    if(b > bits && bits != 0){
        printf("Error: The number %d cannot be represented in %d bits.\n", n, bits);\
        exit(1);
    }

    int *binary;
    if(bits == 0){
        binary = malloc(sizeof(int) * b + 1);
        if(binary == NULL){
            printf("Error: Could not allocate memory for binary.\n");
            exit(1);
        }
        // First index is the length of the array
        binary[0] = b + 1;
        for(int i = b; i > 0; i--){
            binary[i] = n % 2;
            n /= 2;
        }
    } else {
        binary = malloc(sizeof(int) * bits);
        if(binary == NULL){
            printf("Error: Could not allocate memory for binary.\n");
            exit(1);
        }
        for(int i = bits - 1; i >= 0; i--){
            binary[i] = n % 2;
            n /= 2;
        }
    }

    
    return binary;
}

/**
 * Converts a hex string to a binary string
 * @param hex: The hex string to convert (must be 8 characters long/32 bits)
 * @return: The binary string (will be 32 bits long)
 */
int* hexToBinary(char *hex){
    int *binary = malloc(sizeof(int) * 32);
    if(binary == NULL){
        printf("Error: Could not allocate memory for binary.\n");
        exit(1);
    }
    for(int i = 0; i < 8; i++){
        int hexVal = 0;
        if(hex[i] >= '0' && hex[i] <= '9'){
            hexVal = hex[i] - '0';
        } else if(hex[i] >= 'A' && hex[i] <= 'F'){
            hexVal = hex[i] - 'A' + 10;
        } else if(hex[i] >= 'a' && hex[i] <= 'f'){
            hexVal = hex[i] - 'a' + 10;
        } else {
            printf("Error: Invalid hex character: %c.\n", hex[i]);
            exit(1);
        }

        int *tmp = intToBinary(hexVal, 4);
        for(int j = 0; j < 4; j++){
            binary[i * 4 + j] = tmp[j];
        }
        free(tmp);
    }

    return binary;
}

/**
 * Converts a binary string to an integer
 * @param binary: The binary string to convert
 * @param bits: The number of bits to use
 * @return: The integer representation of the binary string
 */
int binaryToInt(int *binary, int bits){
    int ret = 0;
    for(int i = 0; i < bits; i++){
        ret += binary[i] * pow(2, bits - i - 1);
    }
    // for(int i = 0; i < bits; i++){
    //     ret += binary[i] * pow(2, bits - i - 1);
    // }
    return ret;
}


// Config file functions
/**
 * Reads the config file and returns an array of the values
 * @param configFile: The config file to read
 * @return: An array of the values in the config file
 */
int* readConfig(FILE *configFile){
    int *config = malloc(sizeof(int) * 6);
    if(config == NULL){
        printf("Error: Could not allocate memory for config.\n");
        exit(1);
    }

    char *line = malloc(sizeof(char) * 100);
    if(line == NULL){
        printf("Error: Could not allocate memory for line.\n");
        exit(1);
    }

    int i = 0;
    while(fgets(line, sizeof(line), configFile)){
        printf("%s", line);
        int tmp = atoi(line);

        switch (i)
        {
        case 0:
            if(!isPowerOfTwo(tmp)){
                printf("Error: Line size is not a power of 2.\n");
                exit(1);
            }
            config[0] = tmp;
            break;
        case 1:
            if(!isPowerOfTwo(tmp) && tmp != 0){
                printf("Error: Associativity is not a power of 2.\n");
                exit(1);
            }
            config[1] = tmp;
            break;
        case 2:
            if(!isPowerOfTwo(tmp)){
                printf("Error: Data size is not a power of 2.\n");
                exit(1);
            }
            config[2] = tmp;
            break;
        case 3:
            if(tmp != 0 && tmp != 1){
                printf("Error: Replacement policy is not 0 or 1.\n");
                exit(1);
            }
            config[3] = tmp;
            break;
        case 4:
            config[4] = tmp;
            break;
        case 5:
            if(tmp != 0 && tmp != 1){
                printf("Error: Write policy is not 0 or 1.\n");
                exit(1);
            }
            config[5] = tmp;
            break;
        default:
            printf("Error: Too many lines in config file.\n");
            exit(1);
            break;
        }
        i++;
    }
    fclose(configFile);
    free(line);
    return config;
}

// Cache functions
/**
 * Creates the cache
 * @param config: The config array
 * @return: The cache
 */
struct Cache createCache(int* config){ //lineSize, int associativity, int dataSize){
    struct Cache ret;
    ret.config = config;


    // Cache size is in kilobytes, so we need to convert it to bytes
    int lineSize = config[0];
    int associativity = config[1];
    int dataSize = config[2];
    printf("Line size: %d\n", lineSize);
    printf("Associativity: %d\n", associativity);
    printf("Data size: %d\n", dataSize);

    dataSize *= 1024;
    printf("Data size in bytes: %d\n", dataSize);

    // Calculate the number of blocks in the cache
    int numBlocks = dataSize / lineSize;
    printf("Number of blocks: %d\n", numBlocks);


    // Calculate the number of sets in the cache
    int numSets;
    printf("Associativity: %d\n", associativity);
    switch (associativity){
    case 0:
        numSets = 1; // Fully associative
        config[1] = numBlocks; // Set associativity to the number of blocks per set
        break;
    case 1:
        numSets = numBlocks; // Direct mapped
        config[1] = 1; // Set associativity to 1 block per set (direct mapped)
        break;
    default:
        numSets = numBlocks / associativity; // N-way associative
        break;
    }
    printf("Number of sets: %d\n", numSets);

    int *blockConfig = malloc(sizeof(int) * 6);
    if(blockConfig == NULL){
        printf("Error: Could not allocate memory for blockConfig.\n");
        exit(1);
    }

    for(int i = 0; i < 5; i++)
        blockConfig[i] = 0;

    // Calculate block size
    int blockSize = 32 + 1; // memory address size + valid bit
    if(config[3] == 1){
        // Replacement policy is FIFO
        int fifo = log2(associativity);
        blockSize += fifo; // Add FIFO bits
        blockConfig[1] = fifo;
    }
    if(config[5] == 1){
        // Write policy is write-through
        blockSize += 1; // Add dirty bit
        blockConfig[0] = 1;
    }

    int setIndex = log2(numSets);
    int blockOffset = log2(lineSize);
    int tag = 32;
    // int tag = blockSize - setIndex - blockOffset;
    // blockConfig[2] = tag;
    
    // Add config to cache struct
    blockConfig[2] = tag; // tag address size
    blockConfig[3] = numSets; // log2 for set index bits
    blockConfig[4] = blockOffset;
    blockConfig[5] = blockSize;
    ret.blockConfig = blockConfig;

    // Create the cache
    int ***cache = malloc(sizeof(int**) * numSets); // Create the sets
    if(cache == NULL){
        printf("Error: Cache could not be created.\n");
        exit(1);
    }

    for(int i = 0; i < numSets; i++){
        cache[i] = malloc(sizeof(int*) * associativity); // Create the blocks
        if (cache[i] == NULL){
            printf("Error: Cache set #%d could not be created.\n", i);
            exit(1);
        }

        int *index = intToBinary(i, setIndex);
        int setIndexOffset = 1 + blockConfig[0];

        for(int j = 0; j < associativity; j++){
            // Create the block
            cache[i][j] = malloc(sizeof(int) * blockSize);
            if(cache[i][j] == NULL){
                printf("Error: Cache block #%d-%d could not be created.\n", i, j);
                exit(1);
            }

            for(int k = 0; k < blockSize; k++){
                cache[i][j][k] = 0;
            }

            // Set the set index
            for(int k = 0; k < setIndex; k++){
                cache[i][j][k + setIndexOffset] = index[k];
            }
        }
        free(index);
    }


    // Add cache to cache struct and return
    ret.cache = cache;
    return ret;
}

/**
 * Takes in a cache struct and prints the cache
 */
void printCache(struct Cache cache){
    int numSets = cache.blockConfig[3];
    int blocksPerSet = cache.config[1];
    int blockSize = cache.blockConfig[5];

    for(int i = 0; i < numSets; i++){
        printSet(cache.cache[i], blocksPerSet, blockSize, i);
    }
    // Footer
    for(int k = 0; k < blockSize; k++){
        printf("=");
    };
}

void printSet(int **set, int blocksPerSet, int blockSize, int setIndex){
    printf("\t==== Set %d: ====\n", setIndex);
    for(int j = 0; j < blocksPerSet; j++){
        if(j != 0){
            for(int k = 0; k < blockSize; k++){
                printf("-");
            } printf("\n");
        }

        for(int k = 0; k < blockSize; k++){
            printf("%d", set[j][k]);
        } printf("\n");
    }
}

// Cache access functions
void fifo(struct Cache cache, int setIndex, int *newTag){
    int **set = cache.cache[setIndex];
    int tagIndexOffset = 1 + cache.blockConfig[0];
    int fifoBits = cache.blockConfig[1];
    
    // setIndexOffset += fifoBits;
    tagIndexOffset += log2(cache.blockConfig[3]);
    int fifoOffset = tagIndexOffset + cache.blockConfig[2];

    int blocksPerSet = cache.config[1];

    // Increase the count before adding the new tag
    for(int i = 0; i < blocksPerSet; i++){
        if (set[i][0] == 1){
            // Block is valid
            int *fifo = malloc(sizeof(int) * fifoBits);
            if (fifo == NULL){
                printf("Error: Could not allocate memory for fifo.\n");
                exit(1);
            }
            // Get the FIFO bits
            for(int j = 0; j < fifoBits; j++){
                fifo[j] = set[i][j + fifoOffset];
            }

            // Convert to int and increment
            int tmp = binaryToInt(fifo, fifoBits);
            tmp++;

            // Check if there are too many blocks
            if(tmp >= blocksPerSet){
                tmp = 0;
                set[i][0] = 0; // Set valid bit to 0
            }

            // Convert back to binary and write to cache
            int *newFifo = intToBinary(tmp, fifoBits);
            for(int j = 0; j < fifoBits; j++){
                set[i][j + fifoOffset] = newFifo[j];
            }

            free(newFifo);
            free(fifo);
        }
    }

    for(int i = 0; i < blocksPerSet; i++){
        if(set[i][0] == 0){
            // Block is empty

            set[i][0] = 1; // Set valid bit to 1
            for(int j = 0; j < cache.blockConfig[2]; j++){
                set[i][j + tagIndexOffset] = newTag[j]; // Write the new tag
            }
            break;
        }
    }
}

void random(struct Cache cache, int setIndex, int *newTag){
    int **set = cache.cache[setIndex];
    int tagIndexOffset = 1 + cache.blockConfig[0];
    tagIndexOffset += log2(cache.blockConfig[3]);

    int blocksPerSet = cache.config[1];

    int flag = 0;
    for(int i = 0; i < blocksPerSet; i++){
        if(set[i][0] == 0){
            // Block is empty

            set[i][0] = 1; // Set valid bit to 1
            for(int j = 0; j < cache.blockConfig[2]; j++){
                set[i][j + tagIndexOffset] = newTag[j]; // Write the new tag
            }
            flag = 1;
            break;
        }
    }
    if(!flag){
        // No empty blocks, so replace a random block
        int randomBlock = rand() % blocksPerSet;
        for(int j = 0; j < cache.blockConfig[2]; j++){
            set[randomBlock][j + tagIndexOffset] = newTag[j];
        }
    }
}

/**
 * Runs a load operation on the cache
 * @return: The number of cycles the operation took
 */
int load(struct Cache cache, int setIndex, int blockIndex, struct Trace trace){
    int cycles = 1; // 1 cycle for the load operation
    int missPenalty = cache.config[4];

    int **set = cache.cache[setIndex];
    int *block;

    // Get the block
    int* block;
    if(blockIndex == -1){
        // Block was a miss
        cycles += missPenalty; // Add the miss penalty
        int replacementPolicy = cache.config[3];
        if(replacementPolicy){
            // FIFO
            fifo(cache, setIndex, trace.address);
        } else{
            // Random
            random(cache, setIndex, trace.address);
        }
    } else{
        // Block was a hit
        block = set[blockIndex];
    }
    return cycles;
}

/**
 * TO DO: Implement this
 * Runs a store operation on the cache
 * @return: The number of cycles the operation took
 */
int store(struct Cache cache, int setIndex, int blockIndex, struct Trace trace){
    int cycles = 1; // 1 cycle for the store operation
    int missPenalty = cache.config[4];

    int **set = cache.cache[setIndex];
    int *block;

    // Get the block
    int* block;
    if(blockIndex == -1){
        // Block was a miss
        cycles += missPenalty; // Add the miss penalty
        storeMiss(set, cache, trace.address);
        /*
         * Check for write allocate
         */
    } else{
        // Block was a hit
        /*
         * TO DO: Dirty bit and all that jazz
         */
    }
    return cycles;
}

// Trace file functions
/**
 * Reads the trace file and returns an array of the values
 * @param traceFile: The trace file to read
 * @return: An array of the values in the trace file
 */
struct SimData readTraceFile(FILE* traceFile, struct Cache cache){
    char *line = malloc(sizeof(char) * 32);
    if(line == NULL){
        printf("Error: Could not allocate memory for line.\n");
        exit(1);
    }
    struct SimData data;

    while (fgets(line, 32, traceFile)){
        printf("%s", line);

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
        trace.cyclesSinceLastAccess = line[13] - '0';

        runTrace(cache, trace, data);
        free(addressBin);
    }

    free(line);
    
    return data;
}

void runTrace(struct Cache cache, struct Trace trace, struct SimData simData){
    // TODO

    // Get the memory address
    int *address = trace.address;
    int addressInt = binaryToInt(address, 32);

    // Get the set index
    int setIndex = addressInt % cache.blockConfig[3];
    printf("Set index: %d \n", setIndex);

    int **set = cache.cache[setIndex];
    int blocksPerSet = cache.config[1];

    // Check if the address is in the set
    int blockIndex = existsInSet(set, blocksPerSet, address, cache.blockConfig);

    int cycles = trace.cyclesSinceLastAccess;
    switch (trace.accessType)
    {
    case 's':
        /* code */
        // cycles += store(cache, setIndex, blockIndex, trace);
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

    simData.cycles += cycles;
    simData.totalOps++;
}

int existsInSet(int **set, int blocksPerSet, int *address, int *blockConfig){
    int setIndexOffset = 1 + blockConfig[0]; // Valid bit + dirty bit
    setIndexOffset += log2(blockConfig[3]); // Set index bits

    for(int i = 0; i < blocksPerSet; i++){
        int *block = set[i];
        int j;
        for(j = 0; j < blockConfig[2]; j++){
            if(block[j + setIndexOffset] != address[j]){
                // Block is not a match
                break;
            }
        }
        if(j == blockConfig[2]){
            // Block is a match
            return i;
        }
    }
    return -1; // Block is not in set
}