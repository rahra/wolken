CC=gcc
CFLAGS=-O2 -g -Wall -Wextra -std=gnu99 $(shell pkg-config --cflags cairo)
LDLIBS=-lm $(shell pkg-config --libs cairo)

all: wolken

wolken: wcairo.o wolken.o memimg.o smlog.o

clean:
	rm -f *.o wolken

.PHONY: clean

