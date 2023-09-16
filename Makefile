PKG_CONFIG ?= pkg-config
CFLAGS := -g -O2 -Wall
LDLIBS := $(shell $(PKG_CONFIG) --cflags --libs liblz4)

PREFIX ?= /usr/local/
bindir ?= ${PREFIX}bin/
mandir ?= ${PREFIX}man/
libdir ?= ${PREFIX}lib/

lz4jsoncat: lz4jsoncat.c

lz4json.so:
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ $(LDLIBS)

install: lz4jsoncat lz4json.so
	mkdir -p ${bindir} ${mandir}
	cp lz4jsoncat.1 ${mandir}
	cp lz4jsoncat ${bindir}
	mv lz4json.so ${libdir}

clean:
	@# The @ suppresses output. Without the @ before the for loop, the entire bash block is printed to stdout. And it's apparently required for comments too.
	@# lz4jsoncat.dSYM is generated on macOS machines
	@for file in lz4jsoncat lz4json.so lz4jsoncat.*dSYM bin; do \
		rm -rf "$$file"; \
	done
