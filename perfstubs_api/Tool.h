// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

/* Declare the functions that a tool should implement. */

#ifdef __cplusplus
extern "C" {
#endif

// library function declarations
void perftool_init(void);
void perftool_register_thread(void);
void perftool_exit(void);
void perftool_dump(void);

// measurement function declarations
void perftool_timer_start(const char *timer_name);
void perftool_timer_stop(const char *timer_name);
void perftool_static_phase_start(const char *phase_name);
void perftool_static_phase_stop(const char *phase_name);
void perftool_dynamic_phase_start(const char *iteration_prefix, int iteration_number);
void perftool_dynamic_phase_stop(const char *iteration_prefix, int iteration_number);
void perftool_sample_counter(const char *counter_name, double value);
void perftool_metadata(const char *name, const char *value);

// data query function declarations
int perftool_get_timer_names(char **timer_names[]);
int perftool_get_timer_metric_names(char **metric_names[]);
int perftool_get_thread_count(void);
int perftool_get_timer_data(double *timer_values[]);
int perftool_get_counter_names(char **counter_names[]);
int perftool_get_counter_metric_names(char **metric_names[]);
int perftool_get_counter_data(double *counter_values[]);
int perftool_get_metadata(char **names[], char **values[]);

#ifdef __cplusplus
}
#endif