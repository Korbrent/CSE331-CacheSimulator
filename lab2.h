#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#ifndef LAB2_H
#define LAB2_H


struct Cache
{
    /* A block is ordered [Valid Bit] [Dirty Bit] [Set Index] [Tag Bits] [Fifo Bits] [Block Offset]*/
    int ***cache;     // cache[sets][blocks][bits]
    int *config;      // lineSize, associativity, dataSize, replacementPolicy, missPenalty, writePolicy
    int *blockConfig; // dirtyBit, FIFO bits, tag length, number of sets, block offset bit length, block size
};

struct Trace
{
    char accessType;  // 'l' for load, 's' for store
    int *address;     // 32-bit address
    int cyclesSinceLastAccess; // Cycles since last access
};

struct SimData
{
    int totalOps;     // Total number of operations
    int loadHits;     // Number of load hits
    int storeHits;    // Number of store hits
    int loadMisses;   // Number of load misses
    int storeMisses;  // Number of store misses

    int cycles;
};

// Conversion functions
int isPowerOfTwo(int n);
int* intToBinary(int n, int bits);
int* hexToBinary(char* hex);
int binaryToInt(int* binary, int bits);

// Config file functions
int* readConfig(FILE *configFile);

// Cache initialization functions
struct Cache createCache(int* config);
void printCache(struct Cache cache);
void printSet(int **set, int blocksPerSet, int blockSize, int setIndex);

// Cache access functions
void fifo(struct Cache cache, int setIndex, int *newTag);
void random(struct Cache cache, int setIndex, int *newTag);
int load(struct Cache cache, int setIndex, int blockIndex, struct Trace trace);
int store(struct Cache cache, int setIndex, int blockIndex, struct Trace trace);
// void replaceBlock();
// void updateFifoBits();
// void updateDirtyBit();


// Trace file functions
struct SimData readTraceFile(FILE* traceFile, struct Cache cache);
void runTrace(struct Cache cache, struct Trace trace, struct SimData simData);
int existsInSet(int **set, int blocksPerSet, int *address, int *blockConfig);

#endif