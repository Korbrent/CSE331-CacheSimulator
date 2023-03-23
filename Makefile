# *************************************************************************
# 
#  filename: Makefile
# 
#  description: Makefile for the cachesim program.
#               Run `make all` to compile the program.
#               Run `make clean` to remove all compiled files.
# 
#  author: Shelley, Korbin
# 
#  class: CSE 331
#  instructor: Zheng
#  assignment: Lab #2
# 
#  assigned: 2/28/2023
#  due: 3/10/2023
# 
# ************************************************************************
CC=gcc
CFLAGS=-Wall -g
LFLAGS=-lm

all: cachesim

lab2.o: lab2.c
	$(CC) $(CFLAGS) -c $^ $(LFLAGS)

cache.o: cache.c
	$(CC) $(CFLAGS) -c $^ $(LFLAGS)

trace.o: trace.c
	$(CC) $(CFLAGS) -c $^ $(LFLAGS)

cachesim: main.c lab2.o cache.o trace.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
	
clean:
	rm *.o cachesim