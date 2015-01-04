CC=gcc
CFLAGS=-std=c99 -Wall -pedantic

.PHONY: clean

brainfuck:
	$(CC) $(CFLAGS) brainfuck.c -o $@

clean:
	rm brainfuck
