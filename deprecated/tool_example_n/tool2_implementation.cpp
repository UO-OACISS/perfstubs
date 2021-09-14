// Copyright (c) 2019-2020 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/tool.h"
#include <iostream>
#include <cstring>
#include <string>
#include <unordered_map>
#include <utility>
#include <mutex>

using namespace std;

namespace external {
    namespace ps_implementation2 {
        std::mutex my_mutex;
        bool enabled{true};

        class profiler {
            public:
                profiler(const std::string& name) : _name(name), _calls(0) {}
                void start() { _calls++; }
                void stop() { _calls++; }
                std::string _name;
                int _calls;
        };

        class counter {
            public:
                counter(const std::string& name) : _name(name) {}
                std::string _name;
        };

        std::unordered_map<std::string, profiler*> profilers;
        std::unordered_map<std::string, counter*> counters;

        void * find_timer(const char * timer_name) {
            std::string name(timer_name);
            std::lock_guard<std::mutex> guard(my_mutex);
            auto iter = profilers.find(name);
            if (iter == profilers.end()) {
                profiler * p = new profiler(name);
                profilers.insert(std::pair<std::string,profiler*>(name,p));
                return (void*)p;
            }
            return (void*)iter->second;
        }

        void * find_counter(const char * counter_name) {
            std::string name(counter_name);
            std::lock_guard<std::mutex> guard(my_mutex);
            auto iter = counters.find(name);
            if (iter == counters.end()) {
                counter * c = new counter(name);
                counters.insert(std::pair<std::string,counter*>(name,c));
                return (void*)c;
            }
            return (void*)iter->second;
        }

    }
}

namespace MINE = external::ps_implementation2;

/* Function pointers */

extern "C"
{

    // On some systems, can't write output during pre-initialization
    void ps_tool2_initialize(void) { /* cout << "Tool2: " << __func__ << endl; */ }

    // On some systems, can't write output during pre-initialization
    void ps_tool2_register_thread(void)
    {
        /* cout << "Tool2: " << __func__ << endl; */
    }

    void ps_tool2_finalize(void) { cout << "Tool2: " << __func__ << endl; }

    void ps_tool2_pause_measurement(void) { cout << "Tool: " << __func__ << endl; MINE::enabled = false; }

    void ps_tool2_resume_measurement(void) { cout << "Tool: " << __func__ << endl; MINE::enabled = true; }

    void ps_tool2_dump_data(void) { cout << "Tool2: " << __func__ << endl; }

    void * ps_tool2_timer_create(const char * timer_name)
    {
        cout << "Tool2: " << __func__ << " " << timer_name << endl;
        return MINE::find_timer(timer_name);
    }

    void ps_tool2_timer_start(void *profiler)
    {
        MINE::profiler* p = (MINE::profiler*) profiler;
        cout << "Tool2: " << __func__ << " " << p->_name << endl;
        p->start();
    }

    void ps_tool2_timer_stop(void *profiler)
    {
        MINE::profiler* p = (MINE::profiler*) profiler;
        cout << "Tool2: " << __func__ << " " << p->_name << endl;
        p->stop();
    }

    void ps_tool2_start_string(const char * timer_name)
    {
        cout << "Tool2: " << __func__ << " " << timer_name << endl;
    }

    void ps_tool2_stop_string(const char * timer_name)
    {
        cout << "Tool2: " << __func__ << " " << timer_name << endl;
    }

    void ps_tool2_stop_current(void)
    {
        cout << "Tool2: " << __func__ << " " << endl;
    }

    void ps_tool2_set_parameter(const char *parameter_name, int64_t parameter_value)
    {
        cout << "Tool2: " << __func__ << " " << parameter_name
             << " " << parameter_value << endl;
    }

    void ps_tool2_dynamic_phase_start(const char *phase_prefix,
                                      int iteration_index)
    {
        cout << "Tool2: " << __func__ << " " << phase_prefix << ", "
             << iteration_index << endl;
    }

    void ps_tool2_dynamic_phase_stop(const char *phase_prefix,
                                     int iteration_index)
    {
        cout << "Tool2: " << __func__ << " " << phase_prefix << ", "
             << iteration_index << endl;
    }

    void* ps_tool2_create_counter(const char *counter_name)
    {
        cout << "Tool2: " << __func__ << " " << counter_name << endl;
        return MINE::find_counter(counter_name);
    }

    void ps_tool2_sample_counter(void *counter, double value)
    {
        MINE::counter* c = (MINE::counter*) counter;
        cout << "Tool2: " << __func__ << " " << c->_name << " = " << value
             << endl;
    }

    void ps_tool2_set_metadata(const char *name, const char *value)
    {
        cout << "Tool2: " << __func__ << " " << name << " = " << value << endl;
    }

    void ps_tool2_get_timer_data(ps_tool_timer_data_t *timer_data)
    {
        cout << "Tool2: " << __func__ << endl;
        memset(timer_data, 0, sizeof(ps_tool_timer_data_t));
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

    void ps_tool2_free_timer_data(ps_tool_timer_data_t *timer_data)
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

    void ps_tool2_get_counter_data(ps_tool_counter_data_t *counter_data)
    {
        cout << "Tool2: " << __func__ << endl;
        memset(counter_data, 0, sizeof(ps_tool_counter_data_t));
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

    void ps_tool2_free_counter_data(ps_tool_counter_data_t *counter_data)
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

    void ps_tool2_get_metadata(ps_tool_metadata_t *metadata)
    {
        cout << "Tool2: " << __func__ << endl;
        memset(metadata, 0, sizeof(ps_tool_metadata_t));
        metadata->num_values = 1;
        metadata->names = (char **)(calloc(1, sizeof(char *)));
        metadata->values = (char **)(calloc(1, sizeof(char *)));
        metadata->names[0] = strdup("Name string");
        metadata->values[0] = strdup("Value string");
        return;
    }

    void ps_tool2_free_metadata(ps_tool_metadata_t *metadata)
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

/* If your implementation plans to support multiple tools, this is
 * what each tool needs to implement in order to pre-register itself
 * with the plugin system. */

static void __attribute__((constructor)) initme2(void);
static void __attribute__((destructor)) finime2(void);

static int tool_id;

static void initme2(void) {
    ps_plugin_data_t data;
    ps_register_t reg_function;
    reg_function = &ps_register_tool;
    if (reg_function != NULL) {
        memset(&data, 0, sizeof(ps_plugin_data_t));
        data.tool_name = strdup("tool two");
        /* Logistical functions */
        data.initialize = &ps_tool2_initialize;
        data.finalize = &ps_tool2_finalize;
        data.pause_measurement = &ps_tool2_pause_measurement;
        data.resume_measurement = &ps_tool2_resume_measurement;
        data.register_thread = &ps_tool2_register_thread;
        data.dump_data = &ps_tool2_dump_data;
        /* Data entry functions */
        data.timer_create = &ps_tool2_timer_create;
        data.timer_start = &ps_tool2_timer_start;
        data.timer_stop = &ps_tool2_timer_stop;
        data.start_string = &ps_tool2_start_string;
        data.stop_string = &ps_tool2_stop_string;
        data.stop_current = &ps_tool2_stop_current;
        data.set_parameter = &ps_tool2_set_parameter;
        data.dynamic_phase_start = &ps_tool2_dynamic_phase_start;
        data.dynamic_phase_stop = &ps_tool2_dynamic_phase_stop;
        data.create_counter = &ps_tool2_create_counter;
        data.sample_counter = &ps_tool2_sample_counter;
        data.set_metadata = &ps_tool2_set_metadata;
        /* Data Query Functions */
        data.get_timer_data = &ps_tool2_get_timer_data;
        data.get_counter_data = &ps_tool2_get_counter_data;
        data.get_metadata = &ps_tool2_get_metadata;
        data.free_timer_data = &ps_tool2_free_timer_data;
        data.free_counter_data = &ps_tool2_free_counter_data;
        data.free_metadata = &ps_tool2_free_metadata;
        tool_id = reg_function(&data);
    }
}

static void finime2(void) {
    ps_deregister_t dereg_function;
    dereg_function = &ps_deregister_tool;
    if (dereg_function != NULL) {
        dereg_function(tool_id);
    }
}

__attribute__((visibility("default")))
__attribute__((weak)) int ps_register_tool(ps_plugin_data_t * tool);

__attribute__((visibility("default")))
__attribute__((weak)) void ps_deregister_tool(int tool_id);

