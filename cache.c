#include "cache.h"
#include <math.h>


// Cache functions

/**
 * Creates the cache
 * @param config: The config array [lineSize, associativity, dataSize, replacementPolicy, missPenalty, writePolicy]
 * @return: The cache struct with the cache array and configs initialized
 */
struct Cache createCache(int* config){
    struct Cache ret;
    int lineSize = config[0];
    int associativity;
    int dataSize = config[2];
    int missPenalty = config[4];
    int writePolicy = config[5];


    // Cache size is in kilobytes, so we need to convert it to bytes
    dataSize *= 1024;

    // Calculate the number of blocks in the cache
    int numBlocks = dataSize / lineSize;

    // Calculate the number of sets in the cache
    int numSets;
    switch (config[1]){
    case 0:
        numSets = 1; // Fully associative
        config[1] = numBlocks; // Set associativity to the number of blocks per set
        break;
    case 1:
        numSets = numBlocks; // Direct mapped
        // Set associativity is 1 block per set (direct mapped)
        config[3] = 0; // Set replacement policy to random replacement (direct mapped)
        break;
    default:
        numSets = numBlocks / config[1]; // N-way associative
        // Set associativity to the n blocks per set (n-way associative)
        break;
    }
    associativity = config[1];

    // A block is ordered [Valid Bit] [Dirty Bit] [Set Index] [Tag Bits] [Fifo Bits] [Block Offset]
    // [Valid Bit]
    ret.blockConfig.validBit = 1;
    // [Dirty Bit]
    if(config[5] == 1){
        // Write policy is write-through
        ret.blockConfig.dirtyBit = 1;
    } else {
        ret.blockConfig.dirtyBit = 0;
    }
    // [Set Index]
    ret.blockConfig.setIndexBits = log2(numSets);
    if(numSets == 1){
        // Fully associative
        ret.blockConfig.setIndexBits = 0;
    }
    // [Tag Bits]
    ret.blockConfig.tagBits = 32;
    // [Fifo Bits]
    if(config[3] == 1){
        // Replacement policy is FIFO
        ret.blockConfig.fifoBits = log2(config[1]);
    } else {
        ret.blockConfig.fifoBits = 0;
    }
    // [Block Offset]
    ret.blockConfig.blockOffsetBits = log2(lineSize);

    ret.blockConfig.blockSize = ret.blockConfig.validBit + ret.blockConfig.dirtyBit + ret.blockConfig.setIndexBits + ret.blockConfig.tagBits + ret.blockConfig.fifoBits + ret.blockConfig.blockOffsetBits;
    ret.blockConfig.numSets = numSets; // log2 for set index bits


    // Create the cache. Structure is [Set][Block][Bit]

    int ***cache = malloc(sizeof(int**) * numSets); // Create the sets
    if(cache == NULL){
        printf("Error: Cache could not be created.\n");
        exit(1);
    }

    int blockSize = ret.blockConfig.blockSize;
    int setIndex = ret.blockConfig.setIndexBits;
    int setIndexOffset = ret.blockConfig.validBit + ret.blockConfig.dirtyBit;

    for(int i = 0; i < numSets; i++){
        int *index = intToBinary(i, setIndex); // Get the set index in binary

        cache[i] = malloc(sizeof(int*) * associativity); // Create the blocks
        if (cache[i] == NULL){
            printf("Error: Cache set #%d could not be created.\n", i);
            exit(1);
        }

        // Create the blocks in the set
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

    // Set the config values in the cache struct
    ret.config.lineSize = lineSize;
    ret.config.associativity = associativity;
    ret.config.dataSize = dataSize;
    ret.config.replacementPolicy = config[3];
    ret.config.missPenalty = missPenalty;
    ret.config.writePolicy = writePolicy;
    ret.config.numBlocks = numBlocks;

    // Add cache to cache struct and return
    ret.cache = cache;
    return ret;
}

/**
 * Takes in a cache struct and frees the memory
 * @param cache: The cache struct to free
 */
void freeCache(struct Cache cache){
    int numSets = cache.blockConfig.numSets;
    int associativity = cache.config.associativity;

    for(int i = 0; i < numSets; i++){
        for(int j = 0; j < associativity; j++){
            // Free the block
            free(cache.cache[i][j]);
        }
        // Free the set
        free(cache.cache[i]);
    }
    // Free the cache
    free(cache.cache);
}

/**
 * Takes in a cache struct and prints the cache
 * @param cache: The cache struct to print
 */
void printCache(struct Cache cache){
    int numSets = cache.blockConfig.numSets;
    int blocksPerSet = cache.config.associativity;
    int blockSize = cache.blockConfig.blockSize;

    for(int i = 0; i < numSets; i++){
        printSet(cache.cache[i], blocksPerSet, blockSize, i);
    }
    // Footer
    for(int k = 0; k < blockSize; k++){
        printf("=");
    }; printf("\n");
}

/**
 * Takes in a set and prints it
 * @param set: The set to print
 * @param blocksPerSet: The number of blocks per set
 * @param blockSize: The size of each block
 * @param setIndex: The index of the set
 */
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

/**
 * Takes in a blockConfig struct and prints the block layout
 * @param config: The blockConfig struct to print
 */
void printBlockLayout(struct blockConfig config){
    int k = 0;
    if(config.validBit == 1){
        printf("V");
        k++;
    }
    if(config.dirtyBit == 1){
        printf("D");
        k++;
    }
    for(int i = k; k < config.setIndexBits + i; k++)
        printf("S");
    for(int i = k; k < config.tagBits + i; k++)
        printf("T");
    for(int i = k; k < config.fifoBits + i; k++)
        printf("F");
    for(int i = k; k < config.blockOffsetBits + i; k++)
        printf("B");
    printf("\n");
    if(k != config.blockSize){
        printf("Error: Block size does not match block layout.\n");
        printf("Block size: %d\n", config.blockSize);
        printf("Block layout: %d\n", k);
        exit(1);
    }

}


// Cache access functions

/**
 * FIFO replacement policy on a miss in the cache
 * @param cache: The cache to access
 * @param setIndex: The index of the set to add to cache
 * @param newTag: The new tag to add to the set
 */
void fifoReplacement(struct Cache cache, int setIndex, int *newTag){
    int **set = cache.cache[setIndex];
    int tagIndexOffset = 1 + cache.blockConfig.dirtyBit
                 + log2(cache.blockConfig.numSets);
    int fifoBits = cache.blockConfig.fifoBits;
    int fifoOffset = tagIndexOffset + cache.blockConfig.tagBits;

    int blocksPerSet = cache.config.associativity;

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
                // Replace the block and reset the count
                tmp = 0;
                set[i][0] = 0; // Set valid bit to 0
                if(cache.blockConfig.dirtyBit == 1 && set[i][1] == 1){
                    // Block is dirty, write back to memory

                    set[i][1] = 0; // Set dirty bit to 0
                    // printf("FifoReplacement: Writing back to memory.\n");
                    /*
                     * Write back to memory here
                     * (Not implemented)
                     */
                } 
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
            for(int j = 0; j < cache.blockConfig.tagBits; j++){
                set[i][j + tagIndexOffset] = newTag[j]; // Write the new tag
            }
            break;
        }
    }
}

/**
 * Random replacement policy on a miss in the cache
 * @param cache: The cache to access
 * @param setIndex: The index of the set to add to cache
 * @param newTag: The new tag to add to the set
 */
void randomReplacement(struct Cache cache, int setIndex, int *newTag){
    int **set = cache.cache[setIndex];
    int tagIndexOffset = 1 + cache.blockConfig.dirtyBit
                 + log2(cache.blockConfig.numSets);

    int blocksPerSet = cache.config.associativity;

    int flag = 0;
    for(int i = 0; i < blocksPerSet; i++){
        if(set[i][0] == 0){
            // Block is empty

            set[i][0] = 1; // Set valid bit to 1
            for(int j = 0; j < cache.blockConfig.tagBits; j++){
                set[i][j + tagIndexOffset] = newTag[j]; // Write the new tag
            }
            flag = 1;
            break;
        }
    }
    if(!flag){
        // No empty blocks, so replace a random block
        int randomBlock = rand() % blocksPerSet;
        if(cache.blockConfig.dirtyBit == 1 && set[randomBlock][1] == 1){
            // Block is dirty, write back to memory

            set[randomBlock][1] = 0; // Set dirty bit to 0
            // printf("RandomReplacement: Writing back to memory.\n");
            /*
             * Write back to memory here
             * (Not implemented)
             */
        }
        set[randomBlock][0] = 0; // Set valid bit to 0

        set[randomBlock][0] = 1; // Set valid bit to 1
        for(int j = 0; j < cache.blockConfig.tagBits; j++){
            set[randomBlock][j + tagIndexOffset] = newTag[j];
        }
    }
}

/**
 * Runs a load operation on the cache
 * @param cache: The cache to access
 * @param setIndex: The index of the set to access
 * @param blockIndex: The index of the block to access
 * @param trace: The trace struct containing the address
 * @return: The number of cycles the operation took
 */
int load(struct Cache cache, int setIndex, int blockIndex, struct Trace trace){
    int cycles = 1; // 1 cycle for the load operation
    int missPenalty = cache.config.missPenalty;

    // Get the block
    if(blockIndex == -1){
        // Block was a miss
        cycles += missPenalty; // Add the miss penalty

        // Get the replacement policy
        // 0 = Random, 1 = FIFO
        int replacementPolicy = cache.config.replacementPolicy;
        
        // Load the block into the cache from memory
        if(replacementPolicy){
            // FIFO
            fifoReplacement(cache, setIndex, trace.address);
        } else{
            // Random
            randomReplacement(cache, setIndex, trace.address);
        }
    } else{
        // Block was a hit, load block from cache
        // printf("Loading from cache...\n");
        /**
         * Load from cache here
         * (Not implemented)
         */
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
    int missPenalty = cache.config.missPenalty;

    int **set = cache.cache[setIndex];

    // Get the block
    if(blockIndex == -1){
        // Block was a miss
        if(cache.blockConfig.dirtyBit){
            // Write allocate
            // printf("Write allocate, loading block into cache...\n");
            /*
             * Load the block into the cache from memory
             */

            // Get the replacement policy
            // 0 = Random, 1 = FIFO
            int replacementPolicy = cache.config.replacementPolicy;
            if(replacementPolicy){
                // FIFO
                fifoReplacement(cache, setIndex, trace.address);
            } else{
                // Random
                randomReplacement(cache, setIndex, trace.address);
            }

            // Set the dirty bit to 1
            blockIndex = existsInSet(set, cache.config.associativity, trace.address, cache.blockConfig);
            set[blockIndex][1] = 1;

            cycles += missPenalty; // Add the miss penalty
        } else {
            // No write allocate
            // printf("No write allocate, writing to memory...\n");
            /*
             * Write to memory here
             * (Not implemented)
             */
            // No miss penalty
        }
    } else{
        // Block was a hit
        if(cache.blockConfig.dirtyBit){
            // Write-back policy, write to cache (dirty bit is set to 1)
            // printf("Write-back policy, writing to the cache...\n");
            set[blockIndex][1] = 1; // Set dirty bit to 1
            /*
             * Write to cache here
             * (Not implemented)
             */
        } else {
            // Write-through policy, write to memory
            // printf("Write-through policy, writing to memory...\n");
            /*
             * Write to memory here
             * (Not implemented)
             */
        }
    }
    return cycles;
}

/**
 * Checks if a block exists in a set
 * @param set: The set to check
 * @param blocksPerSet: The number of blocks in the set
 * @param address: The address to check for
 * @param blockConfig: The block configuration
 * @return: The index of the block if it exists, -1 if it does not
 */
int existsInSet(int **set, int blocksPerSet, int *address, struct blockConfig blockConfig){
    int tagIndexOffset = 1 + blockConfig.dirtyBit + blockConfig.setIndexBits;

    // Check each block in the set for a match
    for(int i = 0; i < blocksPerSet; i++){
        int *block = set[i];

        if(block[0] == 0){
            // Block is invalid
            continue;
        }

        int j = 0;
        while(j < blockConfig.tagBits){
            if(block[j + tagIndexOffset] != address[j]){
                // Block is not a match
                break;
            }
            j++;
        }

        // Check if we reached the end of the tag bits
        if(j == blockConfig.tagBits){
            // Block is a match
            return i;
        }
    }
    return -1; // Block is not in set
}