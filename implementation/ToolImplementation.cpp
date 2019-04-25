// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/Tool.h"
#include <iostream>
using namespace std; 

/* Function pointers */

extern "C" void perftool_init(void) {
    cout << "Tool: perftool_init!" << endl;
}

extern "C" void perftool_register_thread(void) {
    cout << "Tool: perftool_register_thread!" << endl;
}

extern "C" void perftool_exit(void) {
    cout << "Tool: perftool_exit!" << endl;
}

extern "C" void perftool_start(const char * timer_name) {
    cout << "Tool: perftool_start " << timer_name << endl;
}

extern "C" void perftool_stop(const char * timer_name) {
    cout << "Tool: perftool_stop " << timer_name << endl;
}

extern "C" void perftool_sample_counter(const char * counter_name, double value) {
    cout << "Tool: perftool_sample_counter " << counter_name << " = " << value << endl;
}

extern "C" void perftool_metadata(const char * name, const char * value) {
    cout << "Tool: perftool_metadata " << name << " = " << value << endl;
}


