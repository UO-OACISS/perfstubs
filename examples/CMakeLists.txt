# Copyright (c) 2019-2022 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

# add the executables
add_executable(perfstubs_test_api_cpp all_api.cpp)
target_link_libraries (perfstubs_test_api_cpp perfstubs ${IMPL_LIB} ${PTHREAD_LIB})

add_executable(perfstubs_test_api_cpp_no_tool all_api.cpp)
target_link_libraries (perfstubs_test_api_cpp_no_tool perfstubs ${PTHREAD_LIB})

add_executable(perfstubs_test_api_c all_api.c)
set_target_properties(perfstubs_test_api_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_api_c perfstubs ${IMPL_LIB} ${PTHREAD_LIB})

add_executable(perfstubs_test_api_c_no_tool all_api.c)
set_target_properties(perfstubs_test_api_c_no_tool PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_api_c_no_tool perfstubs ${PTHREAD_LIB})

add_executable(perfstubs_test_cpp main.cpp)
target_link_libraries (perfstubs_test_cpp perfstubs ${IMPL_LIB} ${PTHREAD_LIB})

add_executable(perfstubs_test_c main.c)
set_target_properties(perfstubs_test_c PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_c perfstubs ${IMPL_LIB} ${PTHREAD_LIB})

add_executable(perfstubs_test_overhead overhead.c)
set_target_properties(perfstubs_test_overhead PROPERTIES LINKER_LANGUAGE C)
target_link_libraries (perfstubs_test_overhead perfstubs ${PTHREAD_LIB})

add_executable(perfstubs_test_overhead_cpp overhead.cpp)
target_link_libraries (perfstubs_test_overhead_cpp perfstubs ${PTHREAD_LIB})

add_executable(perfstubs_test_threads_cpp threaded_example.cpp)
target_link_libraries (perfstubs_test_threads_cpp perfstubs ${IMPL_LIB} ${PTHREAD_LIB})

add_executable(perfstubs_test_threads_cpp_no_tool threaded_example.cpp)
target_link_libraries (perfstubs_test_threads_cpp_no_tool perfstubs ${PTHREAD_LIB})

if (APPLE)
    target_link_options(perfstubs_test_overhead PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_overhead_cpp PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_api_cpp_no_tool PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_api_c_no_tool PUBLIC -undefined dynamic_lookup)
    target_link_options(perfstubs_test_threads_cpp_no_tool PUBLIC -undefined dynamic_lookup)
endif (APPLE)

if (PS_HAVE_FORTRAN)
    add_executable(perfstubs_test_fort main.F90)
    set_target_properties(perfstubs_test_fort PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries (perfstubs_test_fort perfstubs ${IMPL_LIB} ${PTHREAD_LIB})
    add_executable(perfstubs_test_fort_no_tool main.F90)
    set_target_properties(perfstubs_test_fort_no_tool PROPERTIES LINKER_LANGUAGE Fortran)
    target_link_libraries (perfstubs_test_fort_no_tool perfstubs ${PTHREAD_LIB})
endif ()

# does the application run
add_test (cpp_api_test perfstubs_test_api_cpp)
set_tests_properties (cpp_api_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_set_metadata meta = data")

add_test (c_api_test perfstubs_test_api_c)
set_tests_properties (c_api_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_set_metadata meta = data")

add_test (cpp_test perfstubs_test_cpp 25)
set_tests_properties (cpp_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_timer_start int main")

add_test (c_test perfstubs_test_c 25)
set_tests_properties (c_test PROPERTIES PASS_REGULAR_EXPRESSION
    "Tool: ps_tool_timer_start .* main")

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
    add_test (test_fort_cpp_no_tool perfstubs_test_fort_no_tool)
endif ()


