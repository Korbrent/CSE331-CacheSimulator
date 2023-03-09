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

test: test.c lab2.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
	
clean:
	rm *.o cachesim