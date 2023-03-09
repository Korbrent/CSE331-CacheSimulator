#include "lab2.h"

#ifndef CACHE_H
#define CACHE_H

// Cache initialization functions
struct Cache createCache(int* config);
void freeCache(struct Cache cache);

// Cache print functions
void printCache(struct Cache cache);
void printSet(int **set, int blocksPerSet, int blockSize, int setIndex);
void printBlockLayout(struct blockConfig config);

// Cache access functions
void fifoReplacement(struct Cache cache, int setIndex, int *newTag);
void randomReplacement(struct Cache cache, int setIndex, int *newTag);
int load(struct Cache cache, int setIndex, int blockIndex, struct Trace trace);
int store(struct Cache cache, int setIndex, int blockIndex, struct Trace trace);

int existsInSet(int **set, int blocksPerSet, int *address, struct blockConfig blockConfig);
#endif