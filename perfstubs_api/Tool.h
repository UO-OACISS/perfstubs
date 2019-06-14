// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

/* Declare the functions that a tool should implement. */

#ifdef __cplusplus
extern "C" {
#endif

void perftool_init(void);
void perftool_register_thread(void);
void perftool_exit(void);
void perftool_timer_start(const char *timer_name);
void perftool_timer_stop(const char *timer_name);
void perftool_static_phase_start(const char *phase_name);
void perftool_static_phase_stop(const char *phase_name);
void perftool_dynamic_phase_start(const char *iteration_prefix, int iteration_number);
void perftool_dynamic_phase_stop(const char *iteration_prefix, int iteration_number);
void perftool_sample_counter(const char *counter_name, double value);
void perftool_metadata(const char *name, const char *value);

#ifdef __cplusplus
}
#endif