F90?=gfortran
CC?=gcc
CXX?=g++
PWD=$(shell pwd)
PREFIX?=$(PWD)/../ps_build
SRCS = $(PWD)/../examples/main.F90 $(PWD)/../implementation/tool.c
OBJS = main.o tool.o

PS_LIB=$(PREFIX)/lib/libperfstubs.so
LIBS = $(PS_LIB) -ldl -lpthread

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
    LDFLAGS=-undefined dynamic_lookup
endif
PERF_OPT= -DPERFSTUBS_USE_TIMERS

OPT=-I$(PREFIX)/include

main: $(OBJS) libtool.so
	$(F90) -o main $(OPT) main.o $(LIBS) $(LDFLAGS)

main.o: $(PWD)/../examples/main.F90
	$(F90) -o $@ -c $(OPT) $^ $(CFLAGS)

tool.o: $(PWD)/../implementation/tool1_implementation.cpp
	$(CXX) -o $@ -c $(OPT) $^ -std=c++11 $(CFLAGS) -fPIC

libtool.so: tool.o
	$(CXX) -o $(PWD)/libtool.so tool.o -shared

clean:
	rm -f $(OBJS) main libtool.so
