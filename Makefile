CC=gcc
CFLAGS=-g

all: csl

csl: evaluator.o ops.o scanner.o stack.o

clean:
	rm -f *.o
	rm -f csl
