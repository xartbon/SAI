CC=gcc
IDIR=-I /usr/include/sai
LDIR=-L /usr/lib
LIBS=-lsai

all: test

.PHONY: test
test: lag_ut
	./lag_ut

lag_ut: lag_ut.o
	gcc $^ $(LDIR) $(LIBS) -o $@

lag_ut.o: lag_ut.c
	gcc -c $^ $(IDIR) -o $@
