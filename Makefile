PKG_CONFIG ?= pkg-config
CFLAGS := -g -O2 -Wall
LDLIBS := $(shell $(PKG_CONFIG) --cflags --libs liblz4)

all: lz4jsoncat lz4jsonpack

lz4jsoncat: lz4jsoncat.c
lz4jsonpack: lz4jsonpack.c

clean:
	rm -f lz4jsoncat
