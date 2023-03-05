#include "lab2.h"
#include <math.h>
/**
 * Functions for the cache simulator
 * @author: Kor Shelley
 * @date: 2023-02-28
 * @version: 1.0
 */

/**
 * Calculates if a number is a power of 2 
 * @param x: The number to check
 * @return: 1 if x is a power of 2, 0 otherwise
 */ 
int isPowerOfTwo(int x){
    return (x != 0) && ((x & (x - 1)) == 0);
}

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
    int tag = blockSize - setIndex - blockOffset;

    // Add config to cache struct
    blockConfig[2] = tag;
    blockConfig[3] = numSets; // log2 for set index
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
    for(int i = 0; i < numSets; i++){
        printf("\t==== Set %d: ====\n", i);
        for(int j = 0; j < blocksPerSet; j++){
            if(j != 0){
                printf("||");
                for(int k = 0; k < cache.blockConfig[5]; k++){
                    printf("-");
                } printf("||\n");
            }

            printf("||");
            for(int k = 0; k < cache.blockConfig[5]; k++){
                printf("%d", cache.cache[i][j][k]);
            } printf("||\n");
        }
        printf("||");
        for(int k = 0; k < cache.blockConfig[5]; k++){
            printf("=");
        } printf("||\n");
    
    }
}

/**
 * Reads the trace file and returns an array of the values
 * @param traceFile: The trace file to read
 * @return: An array of the values in the trace file
 */
int* readTraceFile(FILE* traceFile){
    char *line = malloc(sizeof(char) * 32);
    if(line == NULL){
        printf("Error: Could not allocate memory for line.\n");
        exit(1);
    }

    while (fgets(line, 32, traceFile)){
        printf("%s", line);
        char op = line[0];

        char *address = malloc(sizeof(char) * 8);
        if(address == NULL){
            printf("Error: Could not allocate memory for address.\n");
            exit(1);
        }

        for(int i = 0; i < 8; i++){
            address[i] = line[i + 4];
        }
        printf("Address: %s\n", address);
    }
    
    int *out = malloc(sizeof(int) * 2);
    return out;
}