/*************************************************************************
/
/ filename: lab2.h
/
/ description: This file contains general definitions for the cache sim,
/              including structs that all other files rely upon.
/              Essentially its my master header file.
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
/* A block is ordered [Valid Bit] [Dirty Bit] [Tag Bits] [Set Index] [Fifo Bits] [Block Offset]*/
struct blockConfig
{
    int validBit;       // 1 bit
    int dirtyBit;       // 1 bit if write-back, 0 if write-through
    int tagBits;        // 32 - log2(numSets)
    int setIndexBits;   // log2(numSets)
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