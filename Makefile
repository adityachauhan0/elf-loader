CC = gcc
CFLAGS = -m32 -Wall -fPIC
LDFLAGS = -m32 -shared

all: bin/lib_simpleloader.so bin/launch test/fib

bin/lib_simpleloader.so: src/loader.c src/loader.h
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ src/loader.c

bin/launch: src/launch.c bin/lib_simpleloader.so
	$(CC) $(CFLAGS) -o $@ src/launch.c -L./bin -lsimpleloader -Wl,-rpath,./bin

test/fib: test/fib.c
	$(CC) -m32 -no-pie -nostdlib -o $@ test/fib.c

clean:
	rm -f bin/lib_simpleloader.so bin/launch test/fib

.PHONY: all clean
