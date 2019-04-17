cmake_minimum_required (VERSION 2.6)
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
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pthread")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pthread")
 
# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/examples/Config.h.in"
  "${PROJECT_BINARY_DIR}/examples/Config.h"
  )
include_directories ("${PROJECT_BINARY_DIR}/examples")
 
# add the binary tree to the search path for include files
# so that we will find Config.h
include_directories("${PROJECT_BINARY_DIR}")

# Pthread is garbage
if (PERFSTUBS_USE_STATIC AND NOT APPLE)
    set (PTHREAD_LIB -Wl,--whole-archive pthread -Wl,--no-whole-archive)
else (PERFSTUBS_USE_STATIC AND NOT APPLE)
    set (PTHREAD_LIB pthread)
endif (PERFSTUBS_USE_STATIC AND NOT APPLE)
set (EXTRA_LIBS ${EXTRA_LIBS} ${PTHREAD_LIB} m)

# add the perfstubs library
if (PERFSTUBS_USE_TIMERS)
  include_directories ("${PROJECT_SOURCE_DIR}/perfstub_api")
  add_subdirectory (perfstub_api)
  set (EXTRA_LIBS ${EXTRA_LIBS} perfstubs dl)
endif (PERFSTUBS_USE_TIMERS)

# add the default implementation?
if (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)
  include_directories ("${PROJECT_SOURCE_DIR}/implementation")
  add_subdirectory (implementation)
  if (BUILD_SHARED_LIBS)
    set (IMPL_LIB implementation)
  else (BUILD_SHARED_LIBS)
    if (APPLE)
        set (IMPL_LIB -Wl,-all_load implementation)
    else (APPLE)
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
target_link_libraries (perfstubs_test_cpp ${EXTRA_LIBS})
target_link_libraries (perfstubs_test_c ${EXTRA_LIBS})

include(CTest)

# does the application run
add_test (cpp_test perfstubs_test_cpp 25)
set_tests_properties (cpp_test PROPERTIES PASS_REGULAR_EXPRESSION "Tool: perftoo_start main")
add_test (c_test perfstubs_test_c 25)
set_tests_properties (c_test PROPERTIES PASS_REGULAR_EXPRESSION "Tool: perftoo_start main")