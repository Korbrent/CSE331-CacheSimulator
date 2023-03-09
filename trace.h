#include "lab2.h"
#include "cache.h"

#ifndef TRACE_H
#define TRACE_H

// Trace file functions
struct SimData readTraceFile(FILE* traceFile, struct Cache cache);
struct SimData runTrace(struct Cache cache, struct Trace trace, struct SimData simData);
#endif