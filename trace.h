/*************************************************************************
/
/ filename: trace.h
/
/ description: This file is the header file for trace.c
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
#include "lab2.h"
#include "cache.h"

#ifndef TRACE_H
#define TRACE_H

// Trace file functions
struct SimData readTraceFile(FILE* traceFile, struct Cache cache);
struct SimData runTrace(struct Cache cache, struct Trace trace, struct SimData simData);
#endif