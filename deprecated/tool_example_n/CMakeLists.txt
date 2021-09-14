# Copyright (c) 2019-2020 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

    # add the default implementation?
    if (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)
        add_library(implementation2 tool2_implementation.cpp)
        target_include_directories(implementation2 PRIVATE
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
          $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
          $<INSTALL_INTERFACE:include>
        )
        if (APPLE)
            target_link_options(implementation2 PUBLIC -undefined dynamic_lookup)
        endif (APPLE)
        if (BUILD_SHARED_LIBS)
            set (IMPL_LIB2 implementation2)
        else (BUILD_SHARED_LIBS)
            if (APPLE)
                set (IMPL_LIB2 -Wl,-all_load implementation2)
            else (APPLE)
                # This only seems to work for static linking on Linux.  Boooo...
                # That's ok, we preload the library when testing, see below.
                set (IMPL_LIB2 -Wl,--whole-archive implementation2 -Wl,--no-whole-archive)
            endif (APPLE)
        endif (BUILD_SHARED_LIBS)
    endif (PERFSTUBS_USE_DEFAULT_IMPLEMENTATION)


