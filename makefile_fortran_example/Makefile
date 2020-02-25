F90?=gfortran
CC?=gcc
CXX?=g++
PREFIX?=../ps_build
SRCS = ../examples/main.F90 ../implementation/tool.c
OBJS = main.o tool.o

PERF_PACK=$(PREFIX)/lib/libperfstubs.so
LIBS = $(PERF_PACK) -lstdc++

UNAME=$(shell uname)
ifeq ($(UNAME),Darwin)
    LDFLAGS=-undefined dynamic_lookup
endif
PERF_OPT= -DPERFSTUBS_USE_TIMERS

OPT=-I$(PREFIX)/include

main: $(OBJS)
	$(F90) -o main $(OPT) $(OBJS) $(LIBS) $(LDFLAGS)

main.o: ../examples/main.F90
	$(F90) -o $@ -c $(OPT) $^

tool.o: ../implementation/tool1_implementation.cpp
	$(CXX) -o $@ -c $(OPT) $^

clean:
	rm -f $(OBJS) main
