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

double compute(double value)
{
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
    for (int i = 0; i < 5; i++)
    {
        PERFSTUBS_DYNAMIC_PHASE_START("Loop", i);
        outputValue = compute(inputValue);
        PERFSTUBS_DYNAMIC_PHASE_STOP("Loop", i);
    }

    pthread_join(example_thread, NULL);
    printf("The square root of %f is %f\n", inputValue, outputValue);

    PERFSTUBS_TIMER_STOP_FUNC();

    PERFSTUBS_DUMP_DATA();
    /* output some dummy data */

    perftool_timer_data_t timer_data;
    psGetTimerData(&timer_data);
    int index = 0;
    for (int i = 0; i < timer_data.num_timers; i++)
    {
        for (int j = 0; j < timer_data.num_metrics; j++)
        {
            for (int k = 0; k < timer_data.num_threads; k++)
            {
                printf("%s %s %d = %f\n", timer_data.timer_names[i],
                       timer_data.metric_names[j], k, timer_data.values[index]);
                index = index + 1;
            }
        }
    }
    // free(timer_data);

    index = 0;
    perftool_counter_data_t counter_data;
    psGetCounterData(&counter_data);
    for (int i = 0; i < counter_data.num_counters; i++)
    {
        for (int k = 0; k < counter_data.num_threads; k++)
        {
            printf("%s num_samples %d = %f\n", counter_data.counter_names[i], k,
                   counter_data.num_samples[index]);
            printf("%s value_total %d = %f\n", counter_data.counter_names[i], k,
                   counter_data.value_total[index]);
            printf("%s value_min %d = %f\n", counter_data.counter_names[i], k,
                   counter_data.value_min[index]);
            printf("%s value_max %d = %f\n", counter_data.counter_names[i], k,
                   counter_data.value_max[index]);
            printf("%s value_stddev %d = %f\n", counter_data.counter_names[i],
                   k, counter_data.value_stddev[index]);
            index = index + 1;
        }
    }
    // free(counter_values);

    index = 0;
    perftool_metadata_t metadata;
    psGetMetaData(&metadata);
    for (int i = 0; i < metadata.num_values; i++)
    {
        printf("'%s' = '%s'\n", metadata.names[i], metadata.values[index]);
    }
    // free(metadata_values);

    return 0;
}
