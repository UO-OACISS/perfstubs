// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "Timer.h"

#define PS_EXPORT __attribute__((visibility("default")))

#if defined(__clang__) && defined(__APPLE__)
#define PS_WEAK_PRE
#define PS_WEAK_POST __attribute__((weak_import)) {return;}
#define PS_WEAK_POST_NULL __attribute__((weak_import)) {return nullptr;}
#else
#define PS_WEAK_PRE __attribute__((weak))
#define PS_WEAK_POST
#define PS_WEAK_POST_NULL
#endif

extern "C"
{
    PS_EXPORT PS_WEAK_PRE void perftool_init(void) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_register_thread(void) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_exit(void) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_dump_data(void) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void* perftool_timer_create(const char *) PS_WEAK_POST_NULL;
    PS_EXPORT PS_WEAK_PRE void perftool_timer_start(const void *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_timer_stop(const void *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_dynamic_phase_start(const char *, int) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_dynamic_phase_stop(const char *, int) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void* perftool_create_counter(const char *) PS_WEAK_POST_NULL;
    PS_EXPORT PS_WEAK_PRE void perftool_sample_counter(const void *, double) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_metadata(const char *, const char *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_get_timer_data(perftool_timer_data_t *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_get_counter_data(perftool_counter_data_t *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_get_metadata(perftool_metadata_t *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_free_timer_data(perftool_timer_data_t *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_free_counter_data(perftool_counter_data_t *) PS_WEAK_POST;
    PS_EXPORT PS_WEAK_PRE void perftool_free_metadata(perftool_metadata_t *) PS_WEAK_POST;
}


