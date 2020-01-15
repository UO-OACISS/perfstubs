# Copyright (c) 2019 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

cmake_minimum_required (VERSION 3.8)

# Doesn't compile on Windows.
if (WIN32)
    message(FATAL_ERROR "\nPerfStubs won't compile on Windows. Exiting.")
endif (WIN32)

if (NOT APPLE)
    ENABLE_LANGUAGE(C CXX Fortran)
else ()
    ENABLE_LANGUAGE(C CXX)
endif ()

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

if (NOT APPLE)
    set(CMAKE_Fortran_FLAGS "${CMAKE_Fortran_FLAGS} -cpp")
    if(CMAKE_Fortran_COMPILER MATCHES "gfortran*")
        set(CMAKE_Fortran_FLAGS
            "${CMAKE_Fortran_FLAGS} -std=f2003 -fimplicit-none")
        set(CMAKE_Fortran_FLAGS_DEBUG "-Wall -O0 -g3 -fbounds-check")
        set(CMAKE_Fortran_FLAGS_RELEASE "-O2")
    endif()
endif()

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-DDEBUG)
endif()
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
    "${PROJECT_SOURCE_DIR}/perfstubs_api/config.h.in"
    "${PROJECT_BINARY_DIR}/perfstubs_api/config.h"
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
set (EXTRA_LIBS2 ${EXTRA_LIBS2} ${PTHREAD_LIB} m)

add_library(perfstubs perfstubs_api/timer.c)
add_library(perfstubs_n perfstubs_api_n/timer.c)

# add the perfstubs library
if (PERFSTUBS_USE_TIMERS)
    if (BUILD_SHARED_LIBS)
    set (TOOL_LIBS ${TOOL_LIBS} perfstubs dl)
    set (TOOL_LIBS_N ${TOOL_LIBS_N} perfstubs_n)
    else()
        if (APPLE)
            set (TOOL_LIBS ${TOOL_LIBS} -Wl,-all_load perfstubs)
            set (TOOL_LIBS_N ${TOOL_LIBS_N} -Wl,-all_load perfstubs_n)
        else()
            set (TOOL_LIBS ${TOOL_LIBS}
                -Wl,--whole-archive perfstubs -Wl,--no-whole-archive)
            set (TOOL_LIBS_N ${TOOL_LIBS_N}
                -Wl,--whole-archive perfstubs_n -Wl,--no-whole-archive)
        endif()
    endif()
endif (PERFSTUBS_USE_TIMERS)

# add the default implementation?
set (NO_IMPL_EXTRA_LIBS ${EXTRA_LIBS})
if (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)
    add_library(implementation implementation/tool1_implementation.cpp)
    add_library(implementation2 implementation/tool2_implementation.cpp)
    if (APPLE)
        target_link_options(implementation PUBLIC -undefined dynamic_lookup)
        target_link_options(implementation2 PUBLIC -undefined dynamic_lookup)
    endif (APPLE)
    if (BUILD_SHARED_LIBS)
        set (IMPL_LIB implementation)
        set (IMPL_LIB2 implementation implementation2)
    else (BUILD_SHARED_LIBS)
        if (APPLE)
            set (IMPL_LIB -Wl,-all_load implementation)
            set (IMPL_LIB2 -Wl,-all_load implementation implementation2)
        else (APPLE)
            # This only seems to work for static linking on Linux.  Boooo...
            # That's ok, we preload the library when testing, see below.
            set (IMPL_LIB -Wl,--whole-archive implementation -Wl,--no-whole-archive)
            set (IMPL_LIB2 -Wl,--whole-archive implementation implementation2 -Wl,--no-whole-archive)
        endif (APPLE)
    endif (BUILD_SHARED_LIBS)
    set (EXTRA_LIBS ${EXTRA_LIBS} ${IMPL_LIB})
    set (EXTRA_LIBS2 ${EXTRA_LIBS2} ${IMPL_LIB2})
endif (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)

if (PERFSTUBS_USE_STATIC AND NOT APPLE)
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
endif (PERFSTUBS_USE_STATIC AND NOT APPLE)

# Setup RPATH 
if (NOT PERFSTUBS_USE_STATIC)
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    # the RPATH to be used when installing, but only if it's not a system directory
    LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
    IF("${isSystemDir}" STREQUAL "-1")
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    ENDIF("${isSystemDir}" STREQUAL "-1")
endif (NOT PERFSTUBS_USE_STATIC)

# add the executables
add_executable(perfstubs_test_api_cpp examples/all_api.cpp)
target_link_libraries (perfstubs_test_api_cpp ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_api_n_cpp examples/all_api.cpp)
target_link_libraries (perfstubs_test_api_n_cpp ${TOOL_LIBS_N} ${EXTRA_LIBS2})
target_compile_definitions(perfstubs_test_api_n_cpp PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)

add_executable(perfstubs_test_api_c examples/all_api.c)
set_target_properties(perfstubs_test_api_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_api_c ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_api_n_c examples/all_api.c)
set_target_properties(perfstubs_test_api_n_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_api_n_c ${TOOL_LIBS_N} ${EXTRA_LIBS2})
target_compile_definitions(perfstubs_test_api_n_c PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)

add_executable(perfstubs_test_cpp examples/main.cpp)
target_link_libraries (perfstubs_test_cpp ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_n_cpp examples/main.cpp)
target_link_libraries (perfstubs_test_n_cpp ${TOOL_LIBS_N} ${EXTRA_LIBS2})
target_compile_definitions(perfstubs_test_n_cpp PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)

add_executable(perfstubs_test_c examples/main.c)
set_target_properties(perfstubs_test_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_c ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_n_c examples/main.c)
set_target_properties(perfstubs_test_n_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_n_c ${TOOL_LIBS_N} ${EXTRA_LIBS2})
target_compile_definitions(perfstubs_test_n_c PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)

add_executable(perfstubs_test_overhead examples/overhead.c)
set_target_properties(perfstubs_test_overhead PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_overhead ${TOOL_LIBS} ${NO_IMPL_EXTRA_LIBS})

add_executable(perfstubs_test_overhead_cpp examples/overhead.cpp)
target_link_libraries (perfstubs_test_overhead_cpp ${TOOL_LIBS} ${NO_IMPL_EXTRA_LIBS})

if (APPLE)
    target_link_options(perfstubs_test_overhead PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_overhead_cpp PUBLIC -undefined dynamic_lookup)
endif (APPLE)

if (NOT APPLE)
    if (NOT PERFSTUBS_USE_STATIC)
        add_executable(perfstubs_test_fort examples/main.F90)
        set_target_properties(perfstubs_test_fort PROPERTIES LINKER_LANGUAGE Fortran)
        target_link_libraries (perfstubs_test_fort ${TOOL_LIBS} ${EXTRA_LIBS})
        add_executable(perfstubs_test_n_fort examples/main.F90)
        set_target_properties(perfstubs_test_n_fort PROPERTIES LINKER_LANGUAGE Fortran)
        target_link_libraries (perfstubs_test_n_fort ${TOOL_LIBS_N} ${EXTRA_LIBS2})
        target_compile_definitions(perfstubs_test_n_c PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)
    endif ()
endif ()

include(CTest)

# does the application run
add_test (cpp_api_test perfstubs_test_api_cpp)
set_tests_properties (cpp_api_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_set_metadata meta = data")

add_test (cpp_api_n_test perfstubs_test_api_n_cpp)
set_tests_properties (cpp_api_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_set_metadata meta = data")
set_tests_properties (cpp_api_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool2: ps_tool2_set_metadata meta = data")

add_test (c_api_test perfstubs_test_api_c)
set_tests_properties (c_api_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_set_metadata meta = data")

add_test (c_api_n_test perfstubs_test_api_n_c)
set_tests_properties (c_api_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_set_metadata meta = data")
set_tests_properties (c_api_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool2: ps_tool2_set_metadata meta = data")

add_test (cpp_test perfstubs_test_cpp 25)
set_tests_properties (cpp_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_timer_start int main")

add_test (cpp_n_test perfstubs_test_n_cpp 25)
set_tests_properties (cpp_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_timer_start int main")
set_tests_properties (cpp_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool2: ps_tool2_timer_start int main")

add_test (c_test perfstubs_test_c 25)
set_tests_properties (c_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_timer_start .* main")

add_test (c_n_test perfstubs_test_n_c 25)
set_tests_properties (c_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_timer_start .* main")
set_tests_properties (c_n_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool2: ps_tool2_timer_start .* main")

if (NOT APPLE)
    if (NOT PERFSTUBS_USE_STATIC)
        add_test (fort_test perfstubs_test_fort 25)
        set_tests_properties (fort_test PROPERTIES PASS_REGULAR_EXPRESSION
            "Tool: ps_tool_timer_create main")
        #add_test (fort_n_test perfstubs_test_n_fort 25)
        #set_tests_properties (fort_n_test PROPERTIES PASS_REGULAR_EXPRESSION
        #    "Tool: ps_tool_timer_create main")
        #set_tests_properties (fort_n_test PROPERTIES PASS_REGULAR_EXPRESSION
        #    "Tool2: ps_tool2_timer_create main")
    endif ()
endif ()

# For dynamic testing, preload the "tool"
if (NOT PERFSTUBS_USE_STATIC AND NOT APPLE)
    set_tests_properties(cpp_api_test PROPERTIES
        ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
    set_tests_properties(c_api_test PROPERTIES
        ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
    set_tests_properties(cpp_test PROPERTIES
        ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
    set_tests_properties(c_test PROPERTIES
        ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
    set_tests_properties(fort_test PROPERTIES
        ENVIRONMENT LD_PRELOAD=${CMAKE_BINARY_DIR}/libimplementation.so)
endif (NOT PERFSTUBS_USE_STATIC AND NOT APPLE)

# build a pkg-config file
set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
foreach(LIB ${TOOL_LIBS})
    set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs.pc.in" "${PROJECT_BINARY_DIR}/perfstubs.pc" @ONLY)
foreach(LIB ${TOOL_LIBS_N})
    set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs_n.pc.in" "${PROJECT_BINARY_DIR}/perfstubs_n.pc" @ONLY)

# Add all targets to the build-tree export set
export(TARGETS perfstubs
  FILE "${PROJECT_BINARY_DIR}/PerfStubsTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PerfStubs)

foreach(LIB ${TOOL_LIBS})
    set(PRIVATE_CMAKE_LIBS "${PRIVATE_CMAKE_LIBS} ${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs-config.cmake.in"
    "${PROJECT_BINARY_DIR}/perfstubs-config.cmake" @ONLY)
foreach(LIB ${TOOL_LIBS_N})
    set(PRIVATE_CMAKE_LIBS "${PRIVATE_CMAKE_LIBS} ${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs_n-config.cmake.in"
    "${PROJECT_BINARY_DIR}/perfstubs_n-config.cmake" @ONLY)

install (TARGETS perfstubs perfstubs_n
    EXPORT PerfStubsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    COMPONENT library
)
install (FILES perfstubs_api/timer.h DESTINATION include/perfstubs_api)
install (FILES perfstubs_api/timer_f.h DESTINATION include/perfstubs_api)
install (FILES perfstubs_api/tool.h DESTINATION include/perfstubs_api)
install (FILES ${CMAKE_BINARY_DIR}/perfstubs_api/config.h
    DESTINATION include/perfstubs_api)
install (FILES ${CMAKE_BINARY_DIR}/perfstubs.pc DESTINATION lib/pkgconfig)
install (FILES ${CMAKE_BINARY_DIR}/perfstubs-config.cmake
    DESTINATION lib/cmake)

install (FILES ${CMAKE_BINARY_DIR}/perfstubs_n.pc DESTINATION lib/pkgconfig)
install (FILES ${CMAKE_BINARY_DIR}/perfstubs_n-config.cmake
    DESTINATION lib/cmake)

# Install the export set for use with the install-tree
install(EXPORT PerfStubsTargets 
  DESTINATION lib/cmake
  COMPONENT library)

