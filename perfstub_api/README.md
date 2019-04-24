# Profiling Interface for ADIOS2

Copyright (c) 2019 University of Oregon
Distributed under the BSD Software License
(See accompanying file LICENSE.txt)

_Note:_ This library was originally written as a stub library wrapper in ADIOS.  This is a generic design and implementation for other libraries and tools.

## Todo Items
- [x] Make the interface generic.
    - [x] Replace ADIOST-specific symbols with generic versions that will be implemented by interested measurement libraries (i.e. Score-P). 
    - ~~[ ] New environment variable specifying location of library containing function implementations.~~
    - [x] Remove dynamic-linking specific approach (checking ```LD_PRELOAD```)  If the symbol isn't loaded, don't search for it.  We have to support static linking, too - and if we ```dlopen()``` a library with pthread in it, the application will crash because of initialization of thread-local static globals.

- [x] Add a CMake option to disable the API entirely.

- [x] Add CMake support for linking in measurement libraries when static linking.

- [ ] Investigate API call to trigger writing of performance data to the ADIOS2 archive (performance data stored with the science data).

## Overview

These files contain a thin stub interface for instrumenting library or application code.  The interface can be compiled away entirely.  The function calls are "stubs" in the form of function pointers, initialized to ```nullptr```. The functions are optionally assigned at runtime using dlsym() calls, as is typical with plugin implementations.  If static linking is used, a *weak symbol* technique is used instead.  If the function pointers have the value ```nullptr```, then this library is a few more instructions than a no-op.  If the function pointers are assigned, the measurement library functions are called to perform the timing measurement.  The symbols are made available to the environment either through ```LD_PRELOAD``` settings or by linking in the measurement library.

Convenience macros are provided for constructing descriptive timer names using pre-processor definitions such as ```__FILE__```, ```__LINE__```, and ```__func__```.  For C++ codes, there are also scoped timers to minimize instrumentation text and ensure timers are stopped in functions with multiple return locations or exceptions that throw outside of scope.

## Known Issues

Because the implementation uses ```libdl.so``` there will be linker warnings when linking static executables.  It is known that static executables that load shared-object measurement libraries with pthread support will crash, because of a known issue with thread local storage at program startup.  However, both dynamic and static implementations and examples are provided.

## How to instrument with the C API

For C code, you have the option of specifying the timer name or letting the API generate it based on source code location data.

### Timers

Option 1, explicit timer name:

```C
void function_to_time(void) {
    PERFSTUBS_START("interesting loop");
    ...
    PERFSTUBS_STOP("interesting loop");
}
```

Option 2, generated timer name:

```C
void function_to_time(void) {
    /* Will generate something like:
     * "function_to_time [{filename.c} {123,0}]"
     */
    PERFSTUBS_START_FUNC();
    ...
    PERFSTUBS_STOP_FUNC();
}
```

### Counters

The interface can be used to capture interesting counter values, too:

```C
PERFSTUBS_SAMPLE_COUNTER("Bytes Written", 1024);
```

### Metadata

The interface can be used to capture interesting metadata:

```C
PERFSTUBS_METADATA("ADIOS Method", "POSIX");
```

## How to instrument with the C++ API

The C++ API adds additional scoped timers for convenience:

```C++
void function_to_time(void) {
    /* Will generate something like:
     * "function_to_time [{filename.cpp} {123,0}]"
     */
    PERFSTUBS_SCOPED_TIMER_FUNC();
    ...
}
```

```C++
do {
    PERFSTUBS_SCOPED_TIMER("While Loop");
    ...
} while (!done);
```

## How to use at runtime

To use the API with an application or library, the executable can be linked dynamically or statically.  Dynamic applications can be observed with an external tool such as TAU by using the ```tau_exec``` program wrapper:

```bash
mpirun -np 4 tau_exec -T mpi,papi,pthread,cupti -ebs -cupti -io ./executable
```

The example above will use a TAU configuration with PAPI, MPI and Pthread support, and will enable event based sampling (-ebs), CUDA (-cupti) and POSIX I/O measurement (-io).
