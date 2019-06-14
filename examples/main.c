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
    return 0;
}
