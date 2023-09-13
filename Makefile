PKG_CONFIG ?= pkg-config
CFLAGS := -g -O2 -Wall
LDLIBS := $(shell $(PKG_CONFIG) --cflags --libs liblz4)
bindir ?= ${PREFIX}bin/

lz4jsoncat: lz4jsoncat.c

lz4jsoncat.so: lz4jsoncat.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^ $(LDLIBS)

install: lz4jsoncat
	cp lz4jsoncat.1 /usr/local/share/man/man1/
	mkdir -p ${bindir}
	cp lz4jsoncat ${bindir}

clean:
	@# The @ suppresses output. Without the @ before the for loop, the entire bash block is printed to stdout. And it's apparently required for comments too.
	@# lz4jsoncat.dSYM is generated on macOS machines
	@for file in lz4jsoncat lz4jsoncat.so lz4jsoncat.*dSYM bin; do \
		rm -rf "$$file"; \
	done
