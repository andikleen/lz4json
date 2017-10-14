CFLAGS := -g -O2 -Wall $(shell pkg-config --cflags --libs liblz4)

lz4jsoncat: lz4jsoncat.c

clean:
	rm -f lz4jsoncat
