// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/Tool.h"
#include <iostream>
using namespace std;

/* Function pointers */

extern "C" {

void perftool_init(void) { cout << "Tool: " << __func__ << endl; }

void perftool_register_thread(void)
{
    cout << "Tool: " << __func__ << endl;
}

void perftool_exit(void) { cout << "Tool: " << __func__ << endl; }

void perftool_dump_data(void) { cout << "Tool: " << __func__ << endl; }

void perftool_timer_start(const char *timer_name)
{
    cout << "Tool: " << __func__ << " " << timer_name << endl;
}

void perftool_timer_stop(const char *timer_name)
{
    cout << "Tool: " << __func__ << " " << timer_name << endl;
}

void perftool_static_phase_start(const char *phase_name)
{
    cout << "Tool: " << __func__ << " " << phase_name << endl;
}

void perftool_static_phase_stop(const char *phase_name)
{
    cout << "Tool: " << __func__ << " " << phase_name << endl;
}

void perftool_dynamic_phase_start(const char *phase_prefix,
    int iteration_index)
{
    cout << "Tool: " << __func__ << " " << phase_prefix << ", " <<
        iteration_index << endl;
}

void perftool_dynamic_phase_stop(const char *phase_prefix,
    int iteration_index)
{
    cout << "Tool: " << __func__ << " " << phase_prefix << ", " <<
        iteration_index << endl;
}

void perftool_sample_counter(const char *counter_name, double value)
{
    cout << "Tool: " << __func__ << " " << counter_name << " = " << 
        value << endl;
}

void perftool_metadata(const char *name, const char *value)
{
    cout << "Tool: " << __func__ << " " << name << " = " << value << endl;
}

void perftool_get_timer_data(perftool_timer_data_t * timer_data) {
    cout << "Tool: " << __func__ << endl;
    timer_data->num_timers = 1;
    timer_data->num_threads = 1;
    timer_data->num_metrics = 3;
    timer_data->timer_names = (char**)(calloc(1, sizeof(char*)));
    timer_data->metric_names = (char**)(calloc(3, sizeof(char*)));
    timer_data->values = (double*)(calloc(3, sizeof(double)));
    timer_data->timer_names[0] = strdup("Main Timer");
    timer_data->metric_names[0] = strdup("Calls");
    timer_data->metric_names[1] = strdup("Inclusive Time");
    timer_data->metric_names[2] = strdup("Exclusive Time");
    timer_data->values[0] = 1.0;
    timer_data->values[1] = 20.0;
    timer_data->values[2] = 10.0;
    return;
}

void perftool_get_counter_data(perftool_counter_data_t *counter_data) {
    cout << "Tool: " << __func__ << endl;
    counter_data->num_counters = 1;
    counter_data->num_threads = 1;
    counter_data->counter_names = (char**)(calloc(1, sizeof(char*)));
    counter_data->num_samples = (double*)(calloc(1, sizeof(double)));
    counter_data->value_total = (double*)(calloc(1, sizeof(double)));
    counter_data->value_min = (double*)(calloc(1, sizeof(double)));
    counter_data->value_max = (double*)(calloc(1, sizeof(double)));
    counter_data->value_stddev = (double*)(calloc(1, sizeof(double)));
    counter_data->counter_names[0] = strdup("A Counter");
    counter_data->num_samples[0] = 3.0;
    counter_data->value_total[0] = 6.0;
    counter_data->value_min[0] = 1.0;
    counter_data->value_max[0] = 3.0;
    counter_data->value_stddev[0] = 1.414213562373095;
    return;
}

void perftool_get_metadata(perftool_metadata_t * metadata) {
    cout << "Tool: " << __func__ << endl;
    metadata->num_values = 1;
    metadata->names = (char**)(calloc(1, sizeof(char*)));
    metadata->values = (char**)(calloc(1, sizeof(char*)));
    metadata->names[0] = strdup("Name string");
    metadata->values[0] = strdup("Value string");
    return;
}


}
