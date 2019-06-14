# Copyright (c) 2019 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

# Doesn't compile on Windows.
if (WIN32)
    message(FATAL_ERROR "\nPerfStubs won't compile on Windows. Exiting.")
endif (WIN32)

cmake_minimum_required (VERSION 3.9)
project (perfstubs)

# should we use our own math functions?
option (PERFSTUBS_USE_TIMERS
    "Use provided perfstubs implementation" ON)

# should we use our own math functions?
option (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION
    "Use provided perfstubs implementation" ON)

# should we use static or dynamic linking?
option (PERFSTUBS_USE_STATIC
    "Use static linking" OFF)

if (PERFSTUBS_USE_STATIC)
    set (BUILD_SHARED_LIBS OFF)
else (PERFSTUBS_USE_STATIC)
    set (BUILD_SHARED_LIBS ON)
endif (PERFSTUBS_USE_STATIC)

# The version number.
set (PerfStubs_VERSION_MAJOR 0)
set (PerfStubs_VERSION_MINOR 1)
set (PerfStubs_VERSION ${PerfStubs_VERSION_MAJOR}.${PerfStubs_VERSION_MINOR})

set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pthread")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pthread")
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/perfstubs_api/Config.h.in"
    "${PROJECT_BINARY_DIR}/perfstubs_api/Config.h"
    )
 
# add the binary tree to the search path for include files
# so that we will find Config.h
include_directories("${PROJECT_BINARY_DIR}")
# Also add the source tree for all targets
include_directories ("${PROJECT_SOURCE_DIR}")

# Deal with CMP0042 warnings from CMake
if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
endif (APPLE)

# Pthread is garbage when linking static - not all symbols are included,
# in particular std::thread.detach() and std::thread.join().
# It's a pthread problem, not a gcc or libc problem.
if (PERFSTUBS_USE_STATIC AND NOT APPLE)
    set (PTHREAD_LIB -Wl,--whole-archive pthread -Wl,--no-whole-archive)
else (PERFSTUBS_USE_STATIC AND NOT APPLE)
    set (PTHREAD_LIB pthread)
endif (PERFSTUBS_USE_STATIC AND NOT APPLE)
set (EXTRA_LIBS ${EXTRA_LIBS} ${PTHREAD_LIB} m)

add_library(perfstubs perfstubs_api/Timer.cpp)

# add the perfstubs library
if (PERFSTUBS_USE_TIMERS)
    if (PERFSTUBS_USE_STATIC)
        set (TOOL_LIBS ${TOOL_LIBS} perfstubs)
    else (PERFSTUBS_USE_STATIC)
        set (TOOL_LIBS ${TOOL_LIBS} perfstubs dl)
    endif (PERFSTUBS_USE_STATIC)
endif (PERFSTUBS_USE_TIMERS)

# add the default implementation?
if (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)
    add_library(implementation implementation/ToolImplementation.cpp)
    if (BUILD_SHARED_LIBS)
        set (IMPL_LIB implementation)
    else (BUILD_SHARED_LIBS)
        if (APPLE)
            set (IMPL_LIB -Wl,-all_load implementation)
        else (APPLE)
            # This only seems to work for static linking on Linux.  Boooo...
            # That's ok, we preload the library when testing, see below.
            set (IMPL_LIB -Wl,--whole-archive implementation -Wl,--no-whole-archive)
        endif (APPLE)
    endif (BUILD_SHARED_LIBS)
    set (EXTRA_LIBS ${EXTRA_LIBS} ${IMPL_LIB})
endif (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)

if (PERFSTUBS_USE_STATIC AND NOT APPLE)
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
endif (PERFSTUBS_USE_STATIC AND NOT APPLE)

# add the executable
add_executable(perfstubs_test_cpp examples/main.cpp)
add_executable(perfstubs_test_c examples/main.c)
target_link_libraries (perfstubs_test_cpp ${TOOL_LIBS} ${EXTRA_LIBS})
target_link_libraries (perfstubs_test_c ${TOOL_LIBS} ${EXTRA_LIBS})

include(CTest)

# does the application run
add_test (cpp_test perfstubs_test_cpp 25)
set_tests_properties (cpp_test PROPERTIES PASS_REGULAR_EXPRESSION "Tool: perftool_timer_start main")
add_test (c_test perfstubs_test_c 25)
set_tests_properties (c_test PROPERTIES PASS_REGULAR_EXPRESSION "Tool: perftool_timer_start main")

# For dynamic testing, preload the "tool"
if (NOT PERFSTUBS_USE_STATIC AND NOT APPLE)
set_tests_properties(cpp_test PROPERTIES
    ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
set_tests_properties(c_test PROPERTIES
    ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
endif (NOT PERFSTUBS_USE_STATIC AND NOT APPLE)

# build a pkg-config file
set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
foreach(LIB ${TOOL_LIBS})
    set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs.pc.in" "${PROJECT_BINARY_DIR}/perfstubs.pc" @ONLY)

# Add all targets to the build-tree export set
export(TARGETS perfstubs
  FILE "${PROJECT_BINARY_DIR}/PerfStubsTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PerfStubs)


foreach(LIB ${TOOL_LIBS})
    set(PRIVATE_CMAKE_LIBS "${PRIVATE_CMAKE_LIBS} ${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs-config.cmake.in" "${PROJECT_BINARY_DIR}/perfstubs-config.cmake" @ONLY)

install (TARGETS perfstubs 
    EXPORT PerfStubsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    COMPONENT library
)
install (FILES perfstubs_api/Timer.h DESTINATION include/perfstubs_api)
install (FILES perfstubs_api/Tool.h DESTINATION include/perfstubs_api)
install (FILES ${CMAKE_BINARY_DIR}/perfstubs.pc DESTINATION lib/pkgconfig)
install (FILES ${CMAKE_BINARY_DIR}/perfstubs-config.cmake DESTINATION lib/cmake)

# Install the export set for use with the install-tree
install(EXPORT PerfStubsTargets 
  DESTINATION lib/cmake
  COMPONENT library)

