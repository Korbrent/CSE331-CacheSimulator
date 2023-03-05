#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef LAB2_H
#define LAB2_H


struct Cache
{
    int ***cache;
    int *config; // lineSize, associativity, dataSize, replacementPolicy, missPenalty, writePolicy
    int *blockConfig; // validBit, FIFO bits, tag length, number of sets, block offset bit length, block size
};

int isPowerOfTwo(int n);
int* readConfig(FILE *configFile);
int* intToBinary(int n, int bits);
struct Cache createCache(int* config);
void printCache(struct Cache cache);

int* readTraceFile(FILE* traceFile);
int* hexToBinary(char* hex); // Assumes hex is 8 characters long and 32 bits

#endif