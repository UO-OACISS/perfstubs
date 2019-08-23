// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/Tool.h"
#include <iostream>
#include <cstring>
#include <string>
using namespace std;

/* Function pointers */

extern "C"
{

    void perftool_init(void) { cout << "Tool: " << __func__ << endl; }

    void perftool_register_thread(void)
    {
        cout << "Tool: " << __func__ << endl;
    }

    void perftool_exit(void) { cout << "Tool: " << __func__ << endl; }

    void perftool_dump_data(void) { cout << "Tool: " << __func__ << endl; }

    void perftool_timer_start_string(const char *timer_name)
    {
        cout << "Tool: " << __func__ << " " << timer_name << endl;
    }

    void perftool_timer_stop_string(const char *timer_name)
    {
        cout << "Tool: " << __func__ << " " << timer_name << endl;
    }

    void perftool_dynamic_phase_start(const char *phase_prefix,
                                      int iteration_index)
    {
        cout << "Tool: " << __func__ << " " << phase_prefix << ", "
             << iteration_index << endl;
    }

    void perftool_dynamic_phase_stop(const char *phase_prefix,
                                     int iteration_index)
    {
        cout << "Tool: " << __func__ << " " << phase_prefix << ", "
             << iteration_index << endl;
    }

    void perftool_sample_counter(const char *counter_name, double value)
    {
        cout << "Tool: " << __func__ << " " << counter_name << " = " << value
             << endl;
    }

    void perftool_metadata(const char *name, const char *value)
    {
        cout << "Tool: " << __func__ << " " << name << " = " << value << endl;
    }

    void perftool_get_timer_data(perftool_timer_data_t *timer_data)
    {
        cout << "Tool: " << __func__ << endl;
        memset(timer_data, 0, sizeof(perftool_timer_data_t));
        timer_data->num_timers = 1;
        timer_data->num_threads = 1;
        timer_data->num_metrics = 3;
        timer_data->timer_names = (char **)(calloc(1, sizeof(char *)));
        timer_data->metric_names = (char **)(calloc(3, sizeof(char *)));
        timer_data->values = (double *)(calloc(3, sizeof(double)));
        timer_data->timer_names[0] = strdup("Main Timer");
        timer_data->metric_names[0] = strdup("Calls");
        timer_data->metric_names[1] = strdup("Inclusive Time");
        timer_data->metric_names[2] = strdup("Exclusive Time");
        timer_data->values[0] = 1.0;
        timer_data->values[1] = 20.0;
        timer_data->values[2] = 10.0;
        return;
    }

    void perftool_free_timer_data(perftool_timer_data_t *timer_data)
    {
        if (timer_data == nullptr)
        {
            return;
        }
        if (timer_data->timer_names != nullptr)
        {
            free(timer_data->timer_names);
            timer_data->timer_names = nullptr;
        }
        if (timer_data->metric_names != nullptr)
        {
            free(timer_data->metric_names);
            timer_data->metric_names = nullptr;
        }
        if (timer_data->values != nullptr)
        {
            free(timer_data->values);
            timer_data->values = nullptr;
        }
    }

    void perftool_get_counter_data(perftool_counter_data_t *counter_data)
    {
        cout << "Tool: " << __func__ << endl;
        memset(counter_data, 0, sizeof(perftool_counter_data_t));
        counter_data->num_counters = 1;
        counter_data->num_threads = 1;
        counter_data->counter_names = (char **)(calloc(1, sizeof(char *)));
        counter_data->num_samples = (double *)(calloc(1, sizeof(double)));
        counter_data->value_total = (double *)(calloc(1, sizeof(double)));
        counter_data->value_min = (double *)(calloc(1, sizeof(double)));
        counter_data->value_max = (double *)(calloc(1, sizeof(double)));
        counter_data->value_sumsqr = (double *)(calloc(1, sizeof(double)));
        counter_data->counter_names[0] = strdup("A Counter");
        counter_data->num_samples[0] = 3.0;
        counter_data->value_total[0] = 6.0;
        counter_data->value_min[0] = 1.0;
        counter_data->value_max[0] = 3.0;
        counter_data->value_sumsqr[0] = 1.414213562373095;
        return;
    }

    void perftool_free_counter_data(perftool_counter_data_t *counter_data)
    {
        if (counter_data == nullptr)
        {
            return;
        }
        if (counter_data->counter_names != nullptr)
        {
            free(counter_data->counter_names);
            counter_data->counter_names = nullptr;
        }
        if (counter_data->num_samples != nullptr)
        {
            free(counter_data->num_samples);
            counter_data->num_samples = nullptr;
        }
        if (counter_data->value_total != nullptr)
        {
            free(counter_data->value_total);
            counter_data->value_total = nullptr;
        }
        if (counter_data->value_min != nullptr)
        {
            free(counter_data->value_min);
            counter_data->value_min = nullptr;
        }
        if (counter_data->value_max != nullptr)
        {
            free(counter_data->value_max);
            counter_data->value_max = nullptr;
        }
        if (counter_data->value_sumsqr != nullptr)
        {
            free(counter_data->value_sumsqr);
            counter_data->value_sumsqr = nullptr;
        }
    }

    void perftool_get_metadata(perftool_metadata_t *metadata)
    {
        cout << "Tool: " << __func__ << endl;
        memset(metadata, 0, sizeof(perftool_metadata_t));
        metadata->num_values = 1;
        metadata->names = (char **)(calloc(1, sizeof(char *)));
        metadata->values = (char **)(calloc(1, sizeof(char *)));
        metadata->names[0] = strdup("Name string");
        metadata->values[0] = strdup("Value string");
        return;
    }

    void perftool_free_metadata(perftool_metadata_t *metadata)
    {
        if (metadata == nullptr)
        {
            return;
        }
        if (metadata->names != nullptr)
        {
            free(metadata->names);
            metadata->names = nullptr;
        }
        if (metadata->values != nullptr)
        {
            free(metadata->values);
            metadata->values = nullptr;
        }
    }
}
