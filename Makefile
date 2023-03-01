CC=gcc
CFLAGS=-Wall -g

all: cachesim

lab2.o: lab2.c
	$(CC) $(CFLAGS) -c $^

cachesim: cachesim.c lab2.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o cachesim