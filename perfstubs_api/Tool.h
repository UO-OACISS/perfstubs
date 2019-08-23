// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

/* Declare the functions that a tool should implement. */

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct perftool_timer_data
    {
        unsigned int num_timers;
        unsigned int num_threads;
        unsigned int num_metrics;
        char **timer_names;
        char **metric_names;
        double *values;
    } perftool_timer_data_t;

    typedef struct perftool_counter_data
    {
        unsigned int num_counters;
        unsigned int num_threads;
        char **counter_names;
        double *num_samples;
        double *value_total;
        double *value_min;
        double *value_max;
        double *value_sumsqr;
    } perftool_counter_data_t;

    typedef struct perftool_metadata
    {
        unsigned int num_values;
        char **names;
        char **values;
    } perftool_metadata_t;

    // library function declarations
    void perftool_init(void);
    void perftool_register_thread(void);
    void perftool_exit(void);
    void perftool_dump(void);

    // measurement function declarations
    void perftool_timer_start_string(const char *timer_name);
    void perftool_timer_stop_string(const char *timer_name);
    void perftool_dynamic_phase_start(const char *iteration_prefix,
                                      int iteration_number);
    void perftool_dynamic_phase_stop(const char *iteration_prefix,
                                     int iteration_number);
    void perftool_sample_counter(const char *counter_name, double value);
    void perftool_metadata(const char *name, const char *value);

    // data query function declarations
    void perftool_get_timer_data(perftool_timer_data_t *timer_data);
    void perftool_free_counter_data(perftool_counter_data_t *timer_data);
    void perftool_get_timer_data(perftool_timer_data_t *timer_data);
    void perftool_free_timer_data(perftool_timer_data_t *timer_data);
    void perftool_get_metadata(perftool_metadata_t *metadata);
    void perftool_free_metadata(perftool_metadata_t *metadata);

#ifdef __cplusplus
}
#endif