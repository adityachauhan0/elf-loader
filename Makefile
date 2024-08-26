CC = gcc
CFLAGS = -m32 -Wall
LDFLAGS = -m32 -shared

all: bin/lib_simpleloader.so bin/launch test/fib

bin/lib_simpleloader.so: src/loader.c src/loader.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/loader.c

bin/launch: src/launch.c bin/lib_simpleloader.so
	$(CC) $(CFLAGS) -o $@ src/launch.c -Lbin -lsimpleloader

test/fib: test/fib.c
	$(CC) -m32 -no-pie -nostdlib -o $@ $

clean:
	rm -f bin/lib_simpleloader.so bin/launch test/fib

.PHONY: all clean
