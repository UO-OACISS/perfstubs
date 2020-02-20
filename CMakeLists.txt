# Copyright (c) 2019-2020 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

cmake_minimum_required (VERSION 3.8)

# Doesn't compile on Windows.
if (WIN32)
    message(FATAL_ERROR "\nPerfStubs won't compile on Windows. Exiting.")
endif (WIN32)

if (APPLE)
    ENABLE_LANGUAGE(C CXX)
    message(STATUS "Disabling Fortran support (OSX detected, assume it's not there).")
else ()
    ENABLE_LANGUAGE(C CXX Fortran)
    set(PS_HAVE_FORTRAN ON CACHE BOOL "Have Fortran Compiler")
    message(STATUS "Enabling Fortran support.")
endif ()

# for the examples only
set (CMAKE_CXX_STANDARD 11)

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

# should we build just the library?
option (PERFSTUBS_LIBRARY_ONLY
    "Build only libperfstsubs" ON)

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

# set debug options

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -DNDEBUG -Wno-deprecated")
  set(CMAKE_C_FLAGS_RELEASE "-Ofast -DNDEBUG -Wno-deprecated")
  # using Clang
  set(CXX_PEDANTIC_COMPILER_WARNINGS "-Werror -Wsometimes-uninitialized -fdiagnostics-show-option -Wall -Wcast-align -Wcast-qual -Werror=format -Werror=missing-braces -Werror=parentheses -Werror=reorder -Werror=return-type -Werror=sequence-point -Werror=sign-compare -Werror=uninitialized -Werror=vla -Wextra -Wformat=2 -Winit-self -Wno-attributes -Wno-cast-align -Wno-delete-non-virtual-dtor -Wno-format-nonliteral -Wno-ignored-qualifiers -Wno-sign-promo -Wno-strict-aliasing -Wno-unused-parameter -Wno-unused-variable -Wno-unused-local-typedefs -Wno-unused-parameter ")
  set(C_PEDANTIC_COMPILER_WARNINGS "-Werror -Wsometimes-uninitialized -fdiagnostics-show-option -Wall -Wcast-align -Wcast-qual -Werror=format -Werror=missing-braces -Werror=parentheses -Werror=reorder -Werror=return-type -Werror=sequence-point -Werror=sign-compare -Werror=uninitialized -Werror=vla -Wextra -Wformat=2 -Winit-self -Wno-attributes -Wno-cast-align -Wno-format-nonliteral -Wno-ignored-qualifiers -Wno-strict-aliasing -Wno-unused-parameter -Wno-unused-variable -Wno-unused-local-typedefs -Wno-unused-parameter ")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${CXX_PEDANTIC_COMPILER_WARNINGS} ${PERFSTUBS_SANITIZE_OPTIONS} -DPERFSTUBS_ERROR_HANDLING")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${C_PEDANTIC_COMPILER_WARNINGS} ${PERFSTUBS_SANITIZE_OPTIONS} -DPERFSTUBS_ERROR_HANDLING")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  # using GCC
  if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.8)
     message(FATAL_ERROR "GCC version must be at least 4.8!")
  endif()

  set(CXX_PEDANTIC_COMPILER_WARNINGS "-fdiagnostics-show-option -Wall -Wmaybe-uninitialized -Wcast-align -Wcast-qual -Wdouble-promotion -Werror=format -Werror=missing-braces -Werror=parentheses -Werror=reorder -Werror=return-type -Werror=sequence-point -Werror=sign-compare -Werror=trampolines -Werror=uninitialized -Werror=vla -Wextra -Wformat=2 -Winit-self -Wno-attributes -Wno-cast-align -Wno-delete-non-virtual-dtor -Wno-format-nonliteral -Wno-ignored-qualifiers -Wno-sign-promo -Wno-strict-aliasing -Wno-sync-nand -Wno-unused-but-set-parameter -Wno-unused-but-set-variable -Wno-unused-local-typedefs -Wno-unused-parameter ")
  set(C_PEDANTIC_COMPILER_WARNINGS "-fdiagnostics-show-option -Wall -Wmaybe-uninitialized -Wcast-align -Wcast-qual -Wdouble-promotion -Werror=format -Werror=missing-braces -Werror=parentheses -Werror=reorder -Werror=return-type -Werror=sequence-point -Werror=sign-compare -Werror=trampolines -Werror=uninitialized -Werror=vla -Wextra -Wformat=2 -Winit-self -Wno-attributes -Wno-cast-align -Wno-format-nonliteral -Wno-ignored-qualifiers -Wno-strict-aliasing -Wno-sync-nand -Wno-unused-but-set-parameter -Wno-unused-but-set-variable -Wno-unused-local-typedefs -Wno-unused-parameter ")
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ${CXX_PEDANTIC_COMPILER_WARNINGS} ${PERFSTUBS_SANITIZE_OPTIONS} -DPERFSTUBS_ERROR_HANDLING")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${C_PEDANTIC_COMPILER_WARNINGS} ${PERFSTUBS_SANITIZE_OPTIONS} -DPERFSTUBS_ERROR_HANDLING")
  # -march=native is broken on sandybridge architectures.
  execute_process(COMMAND lscpu COMMAND grep Model: OUTPUT_VARIABLE CPUMODEL_STRING)
  string(REPLACE " " ";" CPUMODEL_LIST ${CPUMODEL_STRING})
  list(LENGTH CPUMODEL_LIST CPUMODEL_LENGTH)
  math(EXPR CPUMODEL_LENGTH "${CPUMODEL_LENGTH} - 1")
  list(GET CPUMODEL_LIST ${CPUMODEL_LENGTH} CPUMODEL)
  message(STATUS "CPU Model: ${CPUMODEL}")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -flto")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  # using Intel C++
  set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 ")
  set(CMAKE_C_FLAGS_DEBUG "-g -O0 ")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -ipo -DNDEBUG -Wno-deprecated")
  set(CMAKE_C_FLAGS_RELEASE "-Ofast -ipo -DNDEBUG -Wno-deprecated")
  set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS} -ipo")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  # using Visual Studio C++
else()
  message(FATAL_ERROR "unknown compiler ${CMAKE_CXX_COMPILER_ID}")
endif()

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
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pthread")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pthread")

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

target_include_directories(perfstubs PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

target_include_directories(perfstubs_n PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include>
)

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

if(NOT PERFSTUBS_LIBRARY_ONLY)
    # add the default implementation?
    set (NO_IMPL_EXTRA_LIBS ${EXTRA_LIBS})
    if (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)
        add_library(implementation implementation/tool1_implementation.cpp)
        add_library(implementation2 implementation/tool2_implementation.cpp)
        target_include_directories(implementation PRIVATE
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
          $<INSTALL_INTERFACE:include>
        )
        target_include_directories(implementation2 PRIVATE
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
          $<INSTALL_INTERFACE:include>
        )
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
endif(NOT PERFSTUBS_LIBRARY_ONLY)

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

if(NOT PERFSTUBS_LIBRARY_ONLY)
# add the executables
add_executable(perfstubs_test_api_cpp examples/all_api.cpp)
target_link_libraries (perfstubs_test_api_cpp ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_api_cpp_no_tool examples/all_api.cpp)
target_link_libraries (perfstubs_test_api_cpp_no_tool ${TOOL_LIBS} ${NO_IMPL_EXTRA_LIBS})

add_executable(perfstubs_test_api_n_cpp examples/all_api.cpp)
target_link_libraries (perfstubs_test_api_n_cpp ${TOOL_LIBS_N} ${EXTRA_LIBS2})
target_compile_definitions(perfstubs_test_api_n_cpp PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)

add_executable(perfstubs_test_api_c examples/all_api.c)
set_target_properties(perfstubs_test_api_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_api_c ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_api_c_no_tool examples/all_api.c)
set_target_properties(perfstubs_test_api_c_no_tool PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_api_c_no_tool ${TOOL_LIBS} ${NO_IMPL_EXTRA_LIBS})

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

add_executable(perfstubs_test_threads_cpp examples/threaded_example.cpp)
target_link_libraries (perfstubs_test_threads_cpp ${TOOL_LIBS} ${EXTRA_LIBS})

add_executable(perfstubs_test_threads_cpp_no_tool examples/threaded_example.cpp)
target_link_libraries (perfstubs_test_threads_cpp_no_tool ${TOOL_LIBS} ${NO_IMPL_EXTRA_LIBS})

if (APPLE)
    target_link_options(perfstubs_test_overhead PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_overhead_cpp PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_api_cpp_no_tool PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_api_c_no_tool PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_threads_cpp_no_tool PUBLIC -undefined dynamic_lookup)
endif (APPLE)

if (PS_HAVE_FORTRAN)
    add_executable(perfstubs_test_fort examples/main.F90)
    set_target_properties(perfstubs_test_fort PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries (perfstubs_test_fort ${TOOL_LIBS} ${EXTRA_LIBS})
    add_executable(perfstubs_test_fort_no_tool examples/main.F90)
    set_target_properties(perfstubs_test_fort_no_tool PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries (perfstubs_test_fort_no_tool ${TOOL_LIBS} ${NO_IMPL_EXTRA_LIBS})
    add_executable(perfstubs_test_n_fort examples/main.F90)
    set_target_properties(perfstubs_test_n_fort PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries (perfstubs_test_n_fort ${TOOL_LIBS_N} ${EXTRA_LIBS2})
    target_compile_definitions(perfstubs_test_n_c PRIVATE -DPERFSTUBS_MULTIPLE_TOOL_SUPPORT)
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

add_test (test_threads_cpp perfstubs_test_threads_cpp)
set_tests_properties (test_threads_cpp PROPERTIES PASS_REGULAR_EXPRESSION
    "Found ps_tool_initialize")
add_test (test_threads_cpp_no_tool perfstubs_test_threads_cpp_no_tool)
add_test (test_api_cpp_no_tool perfstubs_test_api_cpp_no_tool)
add_test (test_api_c_no_tool perfstubs_test_api_c_no_tool)

if (PS_HAVE_FORTRAN)
    add_test (fort_test perfstubs_test_fort 25)
    set_tests_properties (fort_test PROPERTIES PASS_REGULAR_EXPRESSION
        "Tool: ps_tool_timer_create main")
    add_test (fort_n_test perfstubs_test_n_fort 25)
    set_tests_properties (fort_n_test PROPERTIES PASS_REGULAR_EXPRESSION
        "Tool: ps_tool_timer_create main")
    set_tests_properties (fort_n_test PROPERTIES PASS_REGULAR_EXPRESSION
        "Tool2: ps_tool2_timer_create main")
    add_test (test_fort_cpp_no_tool perfstubs_test_fort_no_tool)
endif ()

endif(NOT PERFSTUBS_LIBRARY_ONLY)

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

