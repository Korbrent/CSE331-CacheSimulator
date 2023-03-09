#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#ifndef LAB2_H
#define LAB2_H


// Cache structs

// Config struct holds the configuration of the cache
struct config
{
    int lineSize;           // Line size in bytes
    int associativity;      // Blocks per set
    int dataSize;           // Data size in Kilobytes
    int replacementPolicy;  // 1 for FIFO, 0 for random
    int missPenalty;        // Cycles for a miss
    int writePolicy;        // 0 = (write-through,no-write-allocate)
                            // 1 = (write-back,write-allocate)
    int numBlocks;          // number of blocks in the cache
};

// BlockConfig struct holds the configuration of a block in the cache
/* A block is ordered [Valid Bit] [Dirty Bit] [Set Index] [Tag Bits] [Fifo Bits] [Block Offset]*/
struct blockConfig
{
    int validBit;       // 1 bit
    int dirtyBit;       // 1 bit if write-back, 0 if write-through
    int setIndexBits;   // log2(numSets)
    int tagBits;        // 32
    int fifoBits;       // log2(blocksPerSet)
    int blockOffsetBits;// log2(lineSize)
    int blockSize;      // total bits in a block
    int numSets;        // log2(numSets) = setIndexBits
};

// Cache struct holds the cache itself and the configuration of the cache
struct Cache
{
    int ***cache;     // cache[sets][blocks][bits]
    struct config config;
    struct blockConfig blockConfig;
};

struct Trace
{
    char accessType;  // 'l' for load, 's' for store
    int *address;     // 32-bit address
    int cycles;       // Cycles since last access
};

struct SimData
{
    int totalOps;     // Total number of operations
    int loadHits;     // Number of load hits
    int storeHits;    // Number of store hits
    int loadMisses;   // Number of load misses
    int storeMisses;  // Number of store misses
    int cycles;       // Total cycles
};

// Conversion functions
int isPowerOfTwo(int n);
int* intToBinary(int n, int bits);
int* hexToBinary(char* hex);
int binaryToInt(int* binary, int bits);

// Config file functions
int* readConfig(FILE *configFile);

#endif