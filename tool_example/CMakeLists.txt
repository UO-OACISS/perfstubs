# Copyright (c) 2019-2022 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

    # add the default implementation?
    if (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)
        add_library(tool_example tool1_implementation.cpp)
        target_include_directories(tool_example PRIVATE
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
          $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
          $<INSTALL_INTERFACE:include>
        )
        if (APPLE)
            target_link_options(tool_example PUBLIC -undefined dynamic_lookup)
        endif (APPLE)
        if (BUILD_SHARED_LIBS)
            set (IMPL_LIB tool_example)
        else (BUILD_SHARED_LIBS)
            if (APPLE)
                set (IMPL_LIB -Wl,-all_load tool_example)
            else (APPLE)
                # This only seems to work for static linking on Linux.  Boooo...
                # That's ok, we preload the library when testing, see below.
                set (IMPL_LIB -Wl,--whole-archive tool_example -Wl,--no-whole-archive)
            endif (APPLE)
        endif (BUILD_SHARED_LIBS)
    endif (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)


