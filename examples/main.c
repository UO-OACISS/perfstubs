// Copyright (c) 2019-2022 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

// A simple program that computes the square root of a number
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/timer.h"

void *threaded_function(void *param)
{
    PERFSTUBS_REGISTER_THREAD();
    PERFSTUBS_TIMER_START_FUNC(_timer);
    printf("Hello from new thread!\n");
    PERFSTUBS_TIMER_STOP_FUNC(_timer);
    return NULL;
}

double compute(double value)
{
    PERFSTUBS_TIMER_START_FUNC(_timer);
    PERFSTUBS_SET_PARAMETER("value", (int64_t)(value));
    double tmp = sqrt(value);
    PERFSTUBS_TIMER_STOP_FUNC(_timer);
    return tmp;
}

int main(int argc, char *argv[])
{
    PERFSTUBS_INITIALIZE();
    PERFSTUBS_TIMER_START_FUNC(_timer);

    PERFSTUBS_TIMER_START(_arg_timer, "Argument Validation");
    if (argc < 2)
    {
        fprintf(stderr, "%s Version %d.%d\n", argv[0], PerfStubs_VERSION_MAJOR,
                PerfStubs_VERSION_MINOR);
        fprintf(stderr, "Usage: %s number\n", argv[0]);
        PERFSTUBS_TIMER_STOP(_arg_timer);
        PERFSTUBS_TIMER_STOP_FUNC(_timer);
        return 1;
    }
    PERFSTUBS_TIMER_STOP(_arg_timer);

    pthread_t example_thread;
    pthread_create(&example_thread, NULL, threaded_function, NULL);

    double inputValue = atof(argv[1]);
    PERFSTUBS_SAMPLE_COUNTER("input", inputValue);

    double outputValue;
    int idx;
    for (idx = 0; idx < 5; idx++)
    {
        PERFSTUBS_DYNAMIC_PHASE_START("Loop", idx);
        outputValue = compute(inputValue);
        PERFSTUBS_DYNAMIC_PHASE_STOP("Loop", idx);
    }

    pthread_join(example_thread, NULL);
    printf("The square root of %f is %f\n", inputValue, outputValue);

    PERFSTUBS_TIMER_STOP_FUNC(_timer);

    PERFSTUBS_DUMP_DATA();
    /* output some dummy data */

#ifdef PERFSTUBS_USE_TIMERS
    ps_tool_timer_data_t timer_data;
    memset(&timer_data, 0, sizeof(ps_tool_timer_data_t));
    ps_get_timer_data_(&timer_data);
    int index = 0;
    uint32_t i,j,k;
    for (i = 0; i < timer_data.num_timers; i++)
    {
        for (k = 0; k < timer_data.num_threads; k++)
        {
            /* The first metric is num_calls, if 0 then don't print this one */
            if (timer_data.values[index] == 0.0) {
                index = index + timer_data.num_metrics;
            } else {
                for (j = 0; j < timer_data.num_metrics; j++)
                {
                    printf("'%s' '%s' %d = %f\n", timer_data.timer_names[i],
                        timer_data.metric_names[j], k,
                        timer_data.values[index]);
                    index = index + 1;
                }
            }
        }
    }
    ps_free_timer_data_(&timer_data);

    index = 0;
    ps_tool_counter_data_t counter_data;
    memset(&counter_data, 0, sizeof(ps_tool_counter_data_t));
    ps_get_counter_data_(&counter_data);
    for (i = 0; i < counter_data.num_counters; i++)
    {
        for (k = 0; k < counter_data.num_threads; k++)
        {
            /* The first metric is num_calls, if 0 then don't print this one */
            if (counter_data.num_samples[index] > 0.0) {
                printf("%s num_samples %d = %f\n",
                    counter_data.counter_names[i], k,
                    counter_data.num_samples[index]);
                printf("%s value_total %d = %f\n",
                    counter_data.counter_names[i], k,
                    counter_data.value_total[index]);
                printf("%s value_min %d = %f\n",
                    counter_data.counter_names[i], k,
                    counter_data.value_min[index]);
                printf("%s value_max %d = %f\n",
                    counter_data.counter_names[i], k,
                    counter_data.value_max[index]);
                printf("%s value_sumsqr %d = %f\n",
                    counter_data.counter_names[i], k,
                    counter_data.value_sumsqr[index]);
            }
            index = index + 1;
        }
    }
    ps_free_counter_data_(&counter_data);

    ps_tool_metadata_t metadata;
    memset(&metadata, 0, sizeof(ps_tool_metadata_t));
    ps_get_metadata_(&metadata);
    for (i = 0; i < metadata.num_values; i++)
    {
        printf("'%s' = '%s'\n", metadata.names[i], metadata.values[i]);
    }
    ps_free_metadata_(&metadata);
#endif
    PERFSTUBS_FINALIZE();
    return 0;
}
