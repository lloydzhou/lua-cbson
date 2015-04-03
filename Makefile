##### Available defines for CBSON_CFLAGS #####
##
## USE_INTERNAL_ISINF:      Workaround for Solaris platforms missing isinf().
## DISABLE_INVALID_NUMBERS: Permanently disable invalid JSON numbers:
##                          NaN, Infinity, hex.
##
## Optional built-in number conversion uses the following defines:
## USE_INTERNAL_FPCONV:     Use builtin strtod/dtoa for numeric conversions.
## IEEE_BIG_ENDIAN:         Required on big endian architectures.
## MULTIPLE_THREADS:        Must be set when Lua CBSON may be used in a
##                          multi-threaded application. Requries _pthreads_.
# make clean && make LUA_INCLUDE_DIR=/usr/local/openresty/luajit/include/luajit-2.1 LUA_CMODULE_DIR=/usr/local/openresty/lualib LUA_MODULE_DIR=/usr/local/openresty/lualib  CBSON_CFLAGS="-I/usr/local/include/libbson-1.0 -g -fpic" CC=cc
##### Build defaults #####
LUA_VERSION =       5.1
TARGET =            cbson.so
PREFIX =            /usr/local
#CFLAGS =            -g -Wall -pedantic -fno-inline
CFLAGS =            -O3 -Wall -pedantic -DNDEBUG
CBSON_CFLAGS =      -fpic
CBSON_LDFLAGS =     -shared -fpic -lpthread -ldl
LUA_INCLUDE_DIR =   $(PREFIX)/include
LUA_CMODULE_DIR =   $(PREFIX)/lib/lua/$(LUA_VERSION)
LUA_MODULE_DIR =    $(PREFIX)/share/lua/$(LUA_VERSION)
LUA_BIN_DIR =       $(PREFIX)/bin

##### Platform overrides #####
##
## Tweak one of the platform sections below to suit your situation.
##
## See http://lua-users.org/wiki/BuildingModules for further platform
## specific details.

## Linux

## FreeBSD
#LUA_INCLUDE_DIR =   $(PREFIX)/include/lua51

## MacOSX (Macports)
#PREFIX =            /opt/local
#CBSON_LDFLAGS =     -bundle -undefined dynamic_lookup

## Solaris
#PREFIX =            /home/user/opt
#CC =                gcc
#CBSON_CFLAGS =      -fpic -DUSE_INTERNAL_ISINF

## Windows (MinGW)
#TARGET =            cbson.dll
#PREFIX =            /home/user/opt
#CBSON_CFLAGS =      -DDISABLE_INVALID_NUMBERS
#CBSON_LDFLAGS =     -shared -L$(PREFIX)/lib -llua51
#LUA_BIN_SUFFIX =    .lua

##### Number conversion configuration #####

## Use Libc support for number conversion (default)
#FPCONV_OBJS =       fpconv.o

## Use built in number conversion
#FPCONV_OBJS =       g_fmt.o dtoa.o
#CBSON_CFLAGS +=     -DUSE_INTERNAL_FPCONV

## Compile built in number conversion for big endian architectures
#CBSON_CFLAGS +=     -DIEEE_BIG_ENDIAN

## Compile built in number conversion to support multi-threaded
## applications (recommended)
#CBSON_CFLAGS +=     -pthread -DMULTIPLE_THREADS
#CBSON_LDFLAGS +=    -pthread

##### End customisable sections #####

EXECPERM =          755

BUILD_CFLAGS =      -I$(LUA_INCLUDE_DIR) $(CBSON_CFLAGS)
OBJS =              lua_cbson.o

.PHONY: all clean install

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(BUILD_CFLAGS) -o $@ $<

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(CBSON_LDFLAGS) -o $@ $(OBJS) libbson/.libs/libbson-1.0.so

install: $(TARGET)
	mkdir -p $(DESTDIR)/$(LUA_CMODULE_DIR)
	rm -f $(DESTDIR)/$(LUA_CMODULE_DIR)/$(TARGET)
	cp $(TARGET) $(DESTDIR)/$(LUA_CMODULE_DIR)
	chmod $(EXECPERM) $(DESTDIR)/$(LUA_CMODULE_DIR)/$(TARGET)

clean:
	rm -f *.o $(TARGET)
