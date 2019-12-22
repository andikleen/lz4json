PKG_CONFIG ?= pkg-config
LOCALBASE ?= /opt/local
LOCALBASE != if [ -d "$(LOCALBASE)" ]; then echo "$(LOCALBASE)"; \
	elif [ -d /opt/local	]; then echo /opt/local	;\
	elif [ -d /usr/pkg	]; then echo /usr/pkg	;\
	fi
CFLAGS += -g -O2 -Wall
CFLAGS += -I/usr/local/include -I$(LOCALBASE)/include

LIBLZ4 = which $(PKG_CONFIG) 2>&1 1>/dev/null \
	&& $(PKG_CONFIG) --cflags --libs liblz4 2>/dev/null \
	|| echo -llz4
LDLIBS ?= $(shell $(LIBLZ4))	# gmake
LDLIBS != $(LIBLZ4)		# bsdmake and newer gmake
LDFLAGS += -L/usr/local/lib -L$(LOCALBASE)/lib

lz4jsoncat: lz4jsoncat.c

clean:
	rm -f lz4jsoncat
