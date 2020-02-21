# This is a simple makefile for building just the perfstubs library.
# For a more robust build configuration, or to run the tests, use
# the CMake build configuration instead.

# To use:  run "make" or "make PREFIX=<installation_prefix>"

ifeq ($(PREFIX),)
  PWD = $(shell pwd)
  PREFIX = $(PWD)/gmake_build
endif

LIBSTATIC     = $(PREFIX)/lib/libperfstubs.a
LIBSHARED     = $(PREFIX)/lib/libperfstubs.so
CC       = gcc

CFLAGS   = -O3 -Wall -Wextra -Wstrict-aliasing -I$(PREFIX)/include

SRC      = perfstubs_api/timer.c
STATICOBJ = $(PREFIX)/obj/timer_static.o
SHAREDOBJ = $(PREFIX)/obj/timer_dynamic.o
CONFIG   = $(PREFIX)/config.h

default: all

all: $(CONFIG) $(LIBSTATIC) $(LIBSHARED)

$(PREFIX):
	$(shell mkdir -p $(PREFIX))
	$(shell mkdir -p $(PREFIX)/lib)
	$(shell mkdir -p $(PREFIX)/obj)
	$(shell cd $(PREFIX))

$(CONFIG): $(PREFIX)
	$(shell mkdir -p $(PREFIX)/include/perfstubs_api)
	$(shell cp perfstubs_api/config.h.default $(PREFIX)/include/perfstubs_api/config.h)
	$(shell cp perfstubs_api/*.h $(PREFIX)/include/perfstubs_api/.)

$(STATICOBJ): $(CONFIG)
	$(CC) -o $(STATICOBJ) -c $(SRC) $(CFLAGS) -DPERFSTUBS_USE_STATIC

$(SHAREDOBJ): $(CONFIG)
	$(CC) -o $(SHAREDOBJ) -c $(SRC) $(CFLAGS) -fPIC

$(LIBSTATIC): $(STATICOBJ)
	ar -rc $(LIBSTATIC) $(STATICOBJ)

$(LIBSHARED): $(SHAREDOBJ)
	$(CC) -o $(LIBSHARED) $(SHAREDOBJ) -shared

.PHONY: clean

clean:
	rm -rf $(STATICOBJ) $(SHAREDOBJ) $(LIBSTATIC) $(LIBSHARED)
