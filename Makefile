PKG_CONFIG ?= pkg-config
CFLAGS := -g -O2 -Wall
LDLIBS := $(shell $(PKG_CONFIG) --cflags --libs liblz4)

lz4jsoncat: lz4jsoncat.c

install: lz4jsoncat
	cp lz4jsoncat.1 /usr/local/share/man/man1/
	cp lz4jsoncat /usr/local/bin/

clean:
	@# The @ suppresses output. Without the @ before the for loop, the entire bash block is printed to stdout. And it's apparently required for comments too.
	@# lz4jsoncat.dSYM is generated on macOS machines
	@for file in lz4jsoncat lz4jsoncat.dSYM; do \
		if [ -f "$$file" ]; then \
			rm -f "$$file" || exit 1; \
		elif [ -d "$$file" ]; then \
			rm -rf "$$file" || exit 1; \
		fi \
	done
