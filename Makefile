CFLAGS := -g -O2 -Wall
LDLIBS := -llz4

lz4jsoncat: lz4jsoncat.c

clean:
	rm -f lz4jsoncat
