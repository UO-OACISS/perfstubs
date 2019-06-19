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
    cout << "Tool: " << __func__ << " " << name << " = " << 
        value << endl;
}

int perftool_get_timer_names(char **timer_names[]) {
    (*timer_names) = (char**)(calloc(1, sizeof(char*)));
    (*timer_names)[0] = strdup("Main Timer");
    return 1;
}

int perftool_get_timer_metric_names(char **metric_names[]) {
    (*metric_names) = (char**)(calloc(3, sizeof(char*)));
    (*metric_names)[0] = strdup("Calls");
    (*metric_names)[1] = strdup("Inclusive Time");
    (*metric_names)[2] = strdup("Exclusive Time");
    return 3;
}

int perftool_get_thread_count(void) {
    return 1;
}

int perftool_get_timer_data(double *timer_values[]) {
    (*timer_values) = (double*)(calloc(3, sizeof(double)));
    (*timer_values)[0] = 1.0;
    (*timer_values)[1] = 20.0;
    (*timer_values)[2] = 10.0;
    return 3;
}

int perftool_get_counter_names(char **counter_names[]) {
    (*counter_names) = (char**)(calloc(1, sizeof(char*)));
    (*counter_names)[0] = strdup("A Counter");
    return 1;
}

int perftool_get_counter_metric_names(char **metric_names[]) {
    (*metric_names) = (char**)(calloc(5, sizeof(char*)));
    (*metric_names)[0] = strdup("Samples");
    (*metric_names)[1] = strdup("Minimum");
    (*metric_names)[2] = strdup("Maximum");
    (*metric_names)[3] = strdup("Total");
    (*metric_names)[4] = strdup("Stddev");
    return 5;
}

int perftool_get_counter_data(double *counter_values[]) {
    (*counter_values) = (double*)(calloc(5, sizeof(double)));
    (*counter_values)[0] = 3.0;
    (*counter_values)[1] = 1.0;
    (*counter_values)[2] = 3.0;
    (*counter_values)[3] = 6.0;
    (*counter_values)[4] = 1.414213562373095;
    return 5;
}

int perftool_get_metadata(char **names[], char **values[]) {
    (*names) = (char**)(calloc(1, sizeof(char*)));
    (*names)[0] = strdup("Name string");
    (*values) = (char**)(calloc(1, sizeof(char*)));
    (*values)[0] = strdup("Value string");
    return 1;
}


}
