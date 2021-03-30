# NAME: Michelle Goh
#   NetId: mg2657
CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -g3 -I/c/cs323/Hwk4/

all: lzw encode decode

lzw: lzw.o /c/cs323/Hwk4/code.o hash.o stack.o
	${CC} ${CFLAGS} $^ -o $@
	ln -f lzw encode
	ln -f lzw decode

lzw.o: /c/cs323/Hwk4/code.h

hash.o: hash.h

stack.o: stack.h

encode: lzw

decode: lzw

clean:
		rm -f encode decode *.o
