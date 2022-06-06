# Copyright (c) 2019-2022 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

cmake_minimum_required (VERSION 3.8)

# Doesn't compile on Windows.
if (WIN32)
    message(FATAL_ERROR "\nPerfStubs won't compile on Windows. Exiting.")
endif (WIN32)

if (APPLE)
    ENABLE_LANGUAGE(C CXX)
    set(PS_HAVE_FORTRAN OFF CACHE BOOL "Have Fortran Compiler")
    message(STATUS "Disabling Fortran support (OSX detected, assume it's not there).")
else ()
    ENABLE_LANGUAGE(C CXX Fortran)
    set(PS_HAVE_FORTRAN ON CACHE BOOL "Have Fortran Compiler")
    message(STATUS "Enabling Fortran support.")
endif ()

# for the examples only
set (CMAKE_CXX_STANDARD 11)

project (perfstubs)

# should we use our own tool_example of timers (for examples)?
option (PERFSTUBS_USE_TIMERS
    "Use provided perfstubs tool_example" ON)

# should we use our own tool_example of timers (for examples)?
option (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION
    "Use provided perfstubs tool_example" ON)

# should we use static or dynamic linking?
option (PERFSTUBS_USE_STATIC
    "Use static linking" OFF)

# should we build just the library?
option (PERFSTUBS_BUILD_EXAMPLES
    "Build libperfstsubs examples" OFF)

# SET SANITIZE OPTIONS, IF DESIRED

# defaults
set(PERFSTUBS_SANITIZE_OPTIONS "")

# memory, other
if (DEFINED PERFSTUBS_SANITIZE AND PERFSTUBS_SANITIZE)
  set(PERFSTUBS_SANITIZE_OPTIONS "-fsanitize=address -fsanitize=undefined ")
endif (DEFINED PERFSTUBS_SANITIZE AND PERFSTUBS_SANITIZE)

# race conditions
if (DEFINED PERFSTUBS_SANITIZE_THREAD AND PERFSTUBS_SANITIZE_THREAD)
  set(PERFSTUBS_SANITIZE_OPTIONS "-fsanitize=thread ")
endif (DEFINED PERFSTUBS_SANITIZE_THREAD AND PERFSTUBS_SANITIZE_THREAD)

if (PERFSTUBS_USE_STATIC)
    set (BUILD_SHARED_LIBS OFF)
    message(STATUS "Building static libraries and (if possible) executables.")
else (PERFSTUBS_USE_STATIC)
    set (BUILD_SHARED_LIBS ON)
    message(STATUS "Building shared object libraries and dynamic executables.")
endif (PERFSTUBS_USE_STATIC)

# The version number.
set (PerfStubs_VERSION_MAJOR 0)
set (PerfStubs_VERSION_MINOR 1)
set (PerfStubs_VERSION ${PerfStubs_VERSION_MAJOR}.${PerfStubs_VERSION_MINOR})

set (CMAKE_CXX_STANDARD 11)
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pthread")
#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pthread")
set(CMAKE_THREAD_PREFER_PTHREAD ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package (Threads REQUIRED)

if (PS_HAVE_FORTRAN)
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

# Deal with CMP0042 warnings from CMake
if (APPLE)
    set(CMAKE_MACOSX_RPATH ON)
    set(PS_HAVE_FORTRAN ON CACHE BOOL "Have Fortran Compiler")
endif (APPLE)

# We have to force the static linker to load all symbols, otherwise
# the function pointers won't have definitions, and things "won't work"
set (PS_STATIC_WHOLE_PREFIX "" CACHE STRING "Static linker library prefix flag")
set (PS_STATIC_WHOLE_POSTFIX "" CACHE STRING "Static linker library postfix flag")
if (PERFSTUBS_USE_STATIC)
    if (APPLE) # "Think different!"
        set (PS_STATIC_WHOLE_PREFIX -Wl,-all_load)
        set (PS_STATIC_WHOLE_POSTFIX "")
    else (APPLE)
        set (PS_STATIC_WHOLE_PREFIX -Wl,--whole-archive)
        set (PS_STATIC_WHOLE_POSTFIX -Wl,--no-whole-archive)
    endif (APPLE)
endif (PERFSTUBS_USE_STATIC)

# Pthread is garbage when linking static - not all symbols are included,
# in particular std::thread.detach() and std::thread.join().
# It's a pthread problem, not a gcc or libc problem.
if (PERFSTUBS_USE_STATIC AND NOT APPLE)
    set (PTHREAD_LIB ${PS_STATIC_WHOLE_PREFIX} pthread ${PS_STATIC_WHOLE_POSTFIX})
else (PERFSTUBS_USE_STATIC AND NOT APPLE)
    set (PTHREAD_LIB pthread)
endif (PERFSTUBS_USE_STATIC AND NOT APPLE)

add_library(perfstubs perfstubs_api/timer.c)

if (BUILD_SHARED_LIBS)
    target_link_libraries(perfstubs INTERFACE dl m)
else()
    target_link_libraries(perfstubs INTERFACE m)
endif()

target_include_directories(perfstubs PRIVATE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
)

target_include_directories(perfstubs INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include>
)

if (PERFSTUBS_USE_STATIC AND NOT APPLE)
    SET(CMAKE_EXE_LINKER_FLAGS "-static")
endif (PERFSTUBS_USE_STATIC AND NOT APPLE)

include(GNUInstallDirs)

# Setup RPATH
if (NOT PERFSTUBS_USE_STATIC)
    # use, i.e. don't skip the full RPATH for the build tree
    SET(CMAKE_SKIP_BUILD_RPATH  FALSE)
    # when building, don't use the install RPATH already
    # (but later on when installing)
    SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
    SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
    # the RPATH to be used when installing, but only if it's not a system directory
    LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}" isSystemDir)
    IF("${isSystemDir}" STREQUAL "-1")
        SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
    ENDIF("${isSystemDir}" STREQUAL "-1")
endif (NOT PERFSTUBS_USE_STATIC)

if(PERFSTUBS_BUILD_EXAMPLES)
    include(CTest)
    add_subdirectory(tool_example)
    set (IMPL_LIB ${PS_STATIC_WHOLE_PREFIX} tool_example ${PS_STATIC_WHOLE_POSTFIX})
    add_subdirectory(examples)
endif(PERFSTUBS_BUILD_EXAMPLES)

# build a pkg-config file
set(DEST_DIR "${CMAKE_INSTALL_PREFIX}")
foreach(LIB perfstubs dl m)
    set(PRIVATE_LIBS "${PRIVATE_LIBS} -l${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs.pc.in" "${PROJECT_BINARY_DIR}/perfstubs.pc" @ONLY)

# Add all targets to the build-tree export set
export(TARGETS perfstubs
  FILE "${PROJECT_BINARY_DIR}/PerfStubsTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE PerfStubs)

foreach(LIB perfstubs dl m)
    set(PRIVATE_CMAKE_LIBS "${PRIVATE_CMAKE_LIBS} ${LIB}")
endforeach()
CONFIGURE_FILE("etc/perfstubs-config.cmake.in"
    "${PROJECT_BINARY_DIR}/perfstubs-config.cmake" @ONLY)

    install (TARGETS perfstubs
        EXPORT PerfStubsTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT library
    )

install (FILES perfstubs_api/timer.h DESTINATION include/perfstubs_api)
install (FILES perfstubs_api/timer_f.h DESTINATION include/perfstubs_api)
install (FILES perfstubs_api/tool.h DESTINATION include/perfstubs_api)
install (FILES ${PROJECT_BINARY_DIR}/perfstubs_api/config.h DESTINATION include/perfstubs_api)
install (FILES ${PROJECT_BINARY_DIR}/perfstubs.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
install (FILES ${PROJECT_BINARY_DIR}/perfstubs-config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake)

# Install the export set for use with the install-tree
install(EXPORT PerfStubsTargets
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake
  COMPONENT library)

