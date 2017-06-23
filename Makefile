CC=gcc
CFLAGS=-g -Wall -O0

all: csl

csl: evaluator.o ops.o scanner.o stack.o

test: list.o \
	tests/t_list.o \
	tests/t_atoms.o \
	tests/t_eval_add_subtract.o \
	tests/t_eval_stackops.o \
	tests/t_define.o \
	atoms.o \
	evaluator.o \
	tests/t_if.o \
	ops.o

	$(CC) -I. -o tests/t_list tests/t_list.o list.o
	./tests/t_list

	$(CC) -I. -o tests/t_atoms tests/t_atoms.o atoms.o
	./tests/t_atoms

	$(CC) -I. -o tests/t_eval_add_subtract \
		tests/t_eval_add_subtract.o list.o evaluator.o atoms.o ops.o
	./tests/t_eval_add_subtract

	$(CC) -I. -o tests/t_eval_stackops \
		tests/t_eval_stackops.o list.o evaluator.o atoms.o ops.o
	./tests/t_eval_stackops

	$(CC)  -I. -o tests/t_define \
		tests/t_define.o list.o evaluator.o atoms.o ops.o
	./tests/t_define

	$(CC) -I. -o tests/t_if \
		tests/t_if.o list.o evaluator.o atoms.o ops.o
	./tests/t_if

clean:
	rm -f *.o
	rm -f */*.o
	rm -f csl
