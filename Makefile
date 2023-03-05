CC=gcc
CFLAGS=-Wall -g
LFLAGS=-lm

all: cachesim

lab2.o: lab2.c
	$(CC) $(CFLAGS) -c $^ $(LFLAGS)

cachesim: cachesim.c lab2.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

test: test.c lab2.o
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)
	
clean:
	rm *.o cachesim