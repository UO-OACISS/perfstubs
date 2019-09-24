// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "Timer.h"

#define PS_EXPORT __attribute__((visibility("default")))

#if defined(__clang__) && defined(__APPLE__)
#define PS_WEAK_PRE
#define PS_WEAK_POST __attribute__((weak_import)) {return;}
#define PS_WEAK_POST_NULL __attribute__((weak_import)) {return NULL;}
#else
#define PS_WEAK_PRE __attribute__((weak))
#define PS_WEAK_POST
#define PS_WEAK_POST_NULL
#endif

PS_EXPORT PS_WEAK_PRE void perftool_init(void) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_register_thread(void) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_exit(void) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_dump_data(void) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void* perftool_timer_create(const char * name) PS_WEAK_POST_NULL;
PS_EXPORT PS_WEAK_PRE void perftool_timer_start(const void * timer) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_timer_stop(const void * timer) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_set_parameter(const char * name, int64_t value) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_dynamic_phase_start(const char * prefix, int iter) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_dynamic_phase_stop(const char * prefix, int iter) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void* perftool_create_counter(const char * name) PS_WEAK_POST_NULL;
PS_EXPORT PS_WEAK_PRE void perftool_sample_counter(const void * counter, double value) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_metadata(const char * name, const char * value) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_get_timer_data(perftool_timer_data_t * ptd) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_get_counter_data(perftool_counter_data_t * pcd) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_get_metadata(perftool_metadata_t * pm) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_free_timer_data(perftool_timer_data_t * ptd) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_free_counter_data(perftool_counter_data_t * pcd) PS_WEAK_POST;
PS_EXPORT PS_WEAK_PRE void perftool_free_metadata(perftool_metadata_t * pm) PS_WEAK_POST;


