// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
//#include <threads.h>
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/timer.h"

#define MAX_TOOLS 10

/* Globals for the plugin API */

int perfstubs_initialized = 0;
int num_tools_registered = 0;
/* Keep track of whether the thread has been registered */
__thread int thread_seen = 0;

/* Function pointers */

ps_initialize_t initialize_functions[MAX_TOOLS];
ps_register_thread_t register_thread_functions[MAX_TOOLS];
ps_finalize_t finalize_functions[MAX_TOOLS];
ps_dump_data_t dump_data_functions[MAX_TOOLS];
ps_timer_create_t timer_create_functions[MAX_TOOLS];
ps_timer_start_t timer_start_functions[MAX_TOOLS];
ps_timer_stop_t timer_stop_functions[MAX_TOOLS];
ps_set_parameter_t set_parameter_functions[MAX_TOOLS];
ps_dynamic_phase_start_t dynamic_phase_start_functions[MAX_TOOLS];
ps_dynamic_phase_stop_t dynamic_phase_stop_functions[MAX_TOOLS];
ps_create_counter_t create_counter_functions[MAX_TOOLS];
ps_sample_counter_t sample_counter_functions[MAX_TOOLS];
ps_set_metadata_t set_metadata_functions[MAX_TOOLS];
ps_get_timer_data_t get_timer_data_functions[MAX_TOOLS];
ps_get_counter_data_t get_counter_data_functions[MAX_TOOLS];
ps_get_metadata_t get_metadata_functions[MAX_TOOLS];
ps_free_timer_data_t free_timer_data_functions[MAX_TOOLS];
ps_free_counter_data_t free_counter_data_functions[MAX_TOOLS];
ps_free_metadata_t free_metadata_functions[MAX_TOOLS];

int ps_register_tool(ps_plugin_data_t * tool) {
    if (num_tools_registered > MAX_TOOLS) {
        /* Handle error */
        return -1;
    }
    /* Set the new tool id */
    int tool_id = num_tools_registered;
    /* Logistical functions */
    initialize_functions[num_tools_registered] = tool->initialize;
    finalize_functions[num_tools_registered] = tool->finalize;
    register_thread_functions[num_tools_registered] = tool->register_thread;
    dump_data_functions[num_tools_registered] = tool->dump_data;
    /* Data entry functions */
    timer_create_functions[num_tools_registered] = tool->timer_create;
    timer_start_functions[num_tools_registered] = tool->timer_start;
    timer_stop_functions[num_tools_registered] = tool->timer_stop;
    set_parameter_functions[num_tools_registered] = tool->set_parameter;
    dynamic_phase_start_functions[num_tools_registered] = tool->dynamic_phase_start;
    dynamic_phase_stop_functions[num_tools_registered] = tool->dynamic_phase_stop;
    create_counter_functions[num_tools_registered] = tool->create_counter;
    sample_counter_functions[num_tools_registered] = tool->sample_counter;
    set_metadata_functions[num_tools_registered] = tool->set_metadata;
    /* Data Query Functions */
    get_timer_data_functions[num_tools_registered] = tool->get_timer_data;
    get_counter_data_functions[num_tools_registered] = tool->get_counter_data;
    get_metadata_functions[num_tools_registered] = tool->get_metadata;
    free_timer_data_functions[num_tools_registered] = tool->free_timer_data;
    free_counter_data_functions[num_tools_registered] = tool->free_counter_data;
    free_metadata_functions[num_tools_registered] = tool->free_metadata;
    /* Let the API know that at least one tool exists */
    perfstubs_initialized = 1;
    /* Increment the number of tools and return the tool ID */
    num_tools_registered = num_tools_registered + 1;
    return (tool_id);
}

/* Long term - the tool should be removed from the array.  However,
 * the index of other tools (returned during registration) > tool_id
 * will then be off by 1.  The way to fix that is to provide some
 * mapping for things like deregistration and data requests.  But 
 * let's not spend too much time on that now.  For now, just disable
 * those function pointers, until we know there's a requirement to
 * dynamically enable/disable tools during an execution. */

void ps_deregister_tool(int tool_id) {
    /* Logistical functions */
    initialize_functions[num_tools_registered] = NULL;
    finalize_functions[num_tools_registered] = NULL;
    register_thread_functions[num_tools_registered] = NULL;
    dump_data_functions[num_tools_registered] = NULL;
    /* Data entry functions */
    timer_create_functions[num_tools_registered] = NULL;
    timer_start_functions[num_tools_registered] = NULL;
    timer_stop_functions[num_tools_registered] = NULL;
    set_parameter_functions[num_tools_registered] = NULL;
    dynamic_phase_start_functions[num_tools_registered] = NULL;
    dynamic_phase_stop_functions[num_tools_registered] = NULL;
    create_counter_functions[num_tools_registered] = NULL;
    sample_counter_functions[num_tools_registered] = NULL;
    set_metadata_functions[num_tools_registered] = NULL;
    /* Data Query Functions */
    get_timer_data_functions[num_tools_registered] = NULL;
    get_counter_data_functions[num_tools_registered] = NULL;
    get_metadata_functions[num_tools_registered] = NULL;
    free_timer_data_functions[num_tools_registered] = NULL;
    free_counter_data_functions[num_tools_registered] = NULL;
    free_metadata_functions[num_tools_registered] = NULL;
}

char * ps_make_timer_name_(const char * file,
    const char * func, int line) {
    /* The length of the line number as a string is floor(log10(abs(num))) */
    int string_length = (strlen(file) + strlen(func) + floor(log10(abs(line))) + 11);
    char * name = calloc(string_length, sizeof(char));
    sprintf(name, "%s [{%s} {%d,0}]", func, file, line);
    return (name);
}

// used internally to the class
void ps_register_thread_internal(void) {
    int i;
    for (i = 0 ; i < num_tools_registered ; i++) {
        register_thread_functions[i]();
    }
    thread_seen = 1;
}

/* Initialization */
void ps_initialize_(void) {
    int i;
    for (i = 0 ; i < num_tools_registered ; i++) {
        initialize_functions[i]();
    }
    /* Register the main thread */
    ps_register_thread_internal();
}

void ps_finalize_(void) {
    int i;
    for (i = 0 ; i < num_tools_registered ; i++) {
        finalize_functions[i]();
    }
}

void ps_register_thread_(void) {
    if (thread_seen == 0) {
        ps_register_thread_internal();
    }
}

void* ps_timer_create_(const char *timer_name) {
    void ** objects = (void**)calloc(num_tools_registered, sizeof(void*));
    int i;
    for (i = 0 ; i < num_tools_registered ; i++) {
        objects[i] = (void*)timer_create_functions[i](timer_name);
    }
    return (void*)(objects);
}

void ps_timer_create_fortran_(void ** object, const char *timer_name) {
    *object = ps_timer_create_(timer_name);
}

void ps_timer_start_(const void *timer) {
    void ** objects = (void**)(timer);
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        timer_start_functions[i](objects[i]);
    }
}

void ps_timer_stop_(const void *timer) {
    void ** objects = (void**)(timer);
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        timer_stop_functions[i](objects[i]);
    }
}

void ps_set_parameter_(const char * parameter_name, int64_t parameter_value) {
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        set_parameter_functions[i](parameter_name, parameter_value);
    }
}

void ps_dynamic_phase_start_(const char *phase_prefix, int iteration_index) {
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        dynamic_phase_start_functions[i](phase_prefix, iteration_index);
    }
}

void ps_dynamic_phase_stop_(const char *phase_prefix, int iteration_index) {
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        dynamic_phase_stop_functions[i](phase_prefix, iteration_index);
    }
}

void* ps_create_counter_(const char *name) {
    void ** objects = (void**)calloc(num_tools_registered, sizeof(void*));
    int i;
    for (i = 0 ; i < num_tools_registered ; i++) {
        objects[i] = (void*)create_counter_functions[i](name);
    }
    return (void*)(objects);
}

void ps_create_counter_fortran_(void ** object, const char *name) {
    *object = ps_create_counter_(name);
}

void ps_sample_counter_(const void *counter, const double value) {
    void ** objects = (void**)(counter);
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        sample_counter_functions[i](objects[i], value);
    }
}

void ps_set_metadata_(const char *name, const char *value) {
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        set_metadata_functions[i](name, value);
    }
}

void ps_dump_data_(void) {
    int i;
    for (i = 0; i < num_tools_registered ; i++) {
        dump_data_functions[i]();
    }
}

void ps_get_timer_data_(ps_tool_timer_data_t *timer_data, int tool_id) {
    if (tool_id < num_tools_registered) {
        get_timer_data_functions[tool_id](timer_data);
    }
}

void ps_get_counter_data_(ps_tool_counter_data_t *counter_data, int tool_id) {
    if (tool_id < num_tools_registered) {
        get_counter_data_functions[tool_id](counter_data);
    }
}

void ps_get_metadata_(ps_tool_metadata_t *metadata, int tool_id) {
    if (tool_id < num_tools_registered) {
        get_metadata_functions[tool_id](metadata);
    }
}

void ps_free_timer_data_(ps_tool_timer_data_t *timer_data, int tool_id) {
    if (tool_id < num_tools_registered) {
        free_timer_data_functions[tool_id](timer_data);
    }
}

void ps_free_counter_data_(ps_tool_counter_data_t *counter_data, int tool_id) {
    if (tool_id < num_tools_registered) {
        free_counter_data_functions[tool_id](counter_data);
    }
}

void ps_free_metadata_(ps_tool_metadata_t *metadata, int tool_id) {
    if (tool_id < num_tools_registered) {
        free_metadata_functions[tool_id](metadata);
    }
}

