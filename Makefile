# This is a simple makefile for building just the perfstubs library.
# For a more robust build configuration, or to run the tests, use
# the CMake build configuration instead.

# To use:  run "make" or "make PREFIX=<installation_prefix>"
# Optional: also set CC, CFLAGS, and/or LDFLAGS
# The resulting libraries will have a link/run time dependency on
# libm.a/.so (-lm) and the libperfstubs.so will have a link/run time
# dependency on libdl.so (-ldl).

ifeq ($(PREFIX),)
  PWD = $(shell pwd)
  PREFIX = $(PWD)/ps_build
endif
CC       ?= gcc
CFLAGS   ?= -O3 -Wall -Werror -Wextra
LDFLAGS  ?= -O3 -Wall -Werror -Wextra

LIBSTATIC     = $(PREFIX)/lib/libperfstubs.a
LIBSHARED     = $(PREFIX)/lib/libperfstubs.so
INCLUDES = -I$(PREFIX)/include
SRC      = perfstubs_api/timer.c
STATICOBJ = $(PREFIX)/obj/timer_static.o
SHAREDOBJ = $(PREFIX)/obj/timer_dynamic.o
CONFIG   = $(PREFIX)/include/perfstubs_api/config.h

default: all

all: $(CONFIG) $(LIBSTATIC) $(LIBSHARED)

$(PREFIX):
	$(shell mkdir -p $(PREFIX))
	$(shell mkdir -p $(PREFIX)/lib)
	$(shell mkdir -p $(PREFIX)/obj)
	$(shell cd $(PREFIX))

$(CONFIG): $(PREFIX)
	$(shell mkdir -p $(PREFIX)/include/perfstubs_api)
	$(shell cp perfstubs_api/config.h.default $(CONFIG))
	$(shell cp perfstubs_api/*.h $(PREFIX)/include/perfstubs_api/.)

$(STATICOBJ): $(CONFIG)
	$(CC) -o $(STATICOBJ) -c $(SRC) $(CFLAGS) $(INCLUDES) -DPERFSTUBS_USE_STATIC

$(SHAREDOBJ): $(CONFIG)
	$(CC) -o $(SHAREDOBJ) -c $(SRC) $(CFLAGS) $(INCLUDES) -fPIC

$(LIBSTATIC): $(STATICOBJ)
	ar -rc $(LIBSTATIC) $(STATICOBJ)

$(LIBSHARED): $(SHAREDOBJ)
	$(CC) -o $(LIBSHARED) $(SHAREDOBJ) -shared $(LDFLAGS)

example: $(LIBSHARED)
	$(MAKE) -C makefile_fortran_example
	# Run without the tool
	./makefile_fortran_example/main
	# Run with the tool
	LD_PRELOAD=makefile_fortran_example/libtool.so ./makefile_fortran_example/main


.PHONY: clean

clean:
	rm -rf $(PREFIX)
	$(MAKE) -C makefile_fortran_example clean
