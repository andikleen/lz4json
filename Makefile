PKG_CONFIG ?= pkg-config
CFLAGS := -g -O2 -Wall
LDLIBS := $(shell $(PKG_CONFIG) --cflags --libs liblz4)

lz4jsoncat: lz4jsoncat.c

clean:
	rm -f lz4jsoncat
