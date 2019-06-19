// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

// A simple program that computes the square root of a number
#include "perfstubs_api/Timer.h"
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void *threaded_function(void *param)
{
    PERFSTUBS_REGISTER_THREAD();
    PERFSTUBS_TIMER_START_FUNC();
    printf("Hello from new thread!\n");
    PERFSTUBS_TIMER_STOP_FUNC();
    return NULL;
}

double compute(double value) {
    PERFSTUBS_TIMER_START_FUNC();
    double tmp = sqrt(value);
    PERFSTUBS_TIMER_STOP_FUNC();
    return tmp;
}

int main(int argc, char *argv[])
{
    PERFSTUBS_INIT();
    PERFSTUBS_TIMER_START_FUNC();

    PERFSTUBS_STATIC_PHASE_START("Argument Validation");
    if (argc < 2)
    {
        fprintf(stderr, "%s Version %d.%d\n", argv[0], PerfStubs_VERSION_MAJOR,
                PerfStubs_VERSION_MINOR);
        fprintf(stderr, "Usage: %s number\n", argv[0]);
        PERFSTUBS_TIMER_STOP_FUNC();
        return 1;
    }
    PERFSTUBS_STATIC_PHASE_STOP("Argument Validation");

    pthread_t example_thread;
    pthread_create(&example_thread, NULL, threaded_function, NULL);

    double inputValue = atof(argv[1]);
    PERFSTUBS_SAMPLE_COUNTER("input", inputValue);

    double outputValue;
    for (int i = 0 ; i < 5 ; i++ ) {
        PERFSTUBS_DYNAMIC_PHASE_START("Loop", i);
        outputValue = compute(inputValue);
        PERFSTUBS_DYNAMIC_PHASE_STOP("Loop", i);
    }

    pthread_join(example_thread, NULL);
    printf("The square root of %f is %f\n", inputValue, outputValue);

    PERFSTUBS_TIMER_STOP_FUNC();

    PERFSTUBS_DUMP_DATA();
    /* output some dummy data */

    char** timer_names;
    int num_timers = psGetTimerNames(&timer_names);
    char** metric_names;
    int num_metrics = psGetTimerMetricNames(&metric_names);
    int num_threads = psGetThreadCount();
    double* timer_values;
    int num_timer_values = psGetTimerData(&timer_values);
    int index = 0;
    for (int i = 0 ; i < num_timers ; i++) {
        for (int j = 0 ; j < num_metrics ; j++) {
            for (int k = 0 ; k < num_threads ; k++) {
                printf("%s %s %d = %f\n", timer_names[i], metric_names[j], k, timer_values[index]);
                index = index + 1;
                if (index >= num_timer_values) break;
            }
            if (index >= num_timer_values) break;
        }
        if (index >= num_timer_values) break;
    }

    index = 0;
    char** counter_names;
    int num_counters = psGetCounterNames(&counter_names);
    char** counter_metric_names;
    num_metrics = psGetCounterMetricNames(&counter_metric_names);
    double* counter_values;
    int num_counter_values = psGetCounterData(&counter_values);
    for (int i = 0 ; i < num_counters ; i++) {
        for (int j = 0 ; j < num_metrics ; j++) {
            for (int k = 0 ; k < num_threads ; k++) {
                printf("%s %s %d = %f\n", counter_names[i], counter_metric_names[j], k, counter_values[index]);
                index = index + 1;
                if (index >= num_counter_values) break;
            }
            if (index >= num_counter_values) break;
        }
        if (index >= num_timer_values) break;
    }

    index = 0;
    char** metadata_names;
    char** metadata_values;
    int num_meta_data = psGetMetaData(&metadata_names, &metadata_values);
    for (int i = 0 ; i < num_meta_data ; i++) {
        printf("%s = %s\n", metadata_names[i], metadata_values[index]);
    }

    return 0;
}
