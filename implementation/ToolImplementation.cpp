// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/Tool.h"
#include <iostream>
using namespace std;

/* Function pointers */

extern "C" void perftool_init(void) { cout << "Tool: " << __func__ << endl; }

extern "C" void perftool_register_thread(void)
{
    cout << "Tool: " << __func__ << endl;
}

extern "C" void perftool_exit(void) { cout << "Tool: " << __func__ << endl; }

extern "C" void perftool_timer_start(const char *timer_name)
{
    cout << "Tool: " << __func__ << " " << timer_name << endl;
}

extern "C" void perftool_timer_stop(const char *timer_name)
{
    cout << "Tool: " << __func__ << " " << timer_name << endl;
}

extern "C" void perftool_static_phase_start(const char *phase_name)
{
    cout << "Tool: " << __func__ << " " << phase_name << endl;
}

extern "C" void perftool_static_phase_stop(const char *phase_name)
{
    cout << "Tool: " << __func__ << " " << phase_name << endl;
}

extern "C" void perftool_dynamic_phase_start(const char *phase_prefix,
    int iteration_index)
{
    cout << "Tool: " << __func__ << " " << phase_prefix << ", " <<
        iteration_index << endl;
}

extern "C" void perftool_dynamic_phase_stop(const char *phase_prefix,
    int iteration_index)
{
    cout << "Tool: " << __func__ << " " << phase_prefix << ", " <<
        iteration_index << endl;
}

extern "C" void perftool_sample_counter(const char *counter_name, double value)
{
    cout << "Tool: " << __func__ << " " << counter_name << " = " << 
        value << endl;
}

extern "C" void perftool_metadata(const char *name, const char *value)
{
    cout << "Tool: " << __func__ << " " << name << " = " << 
        value << endl;
}
