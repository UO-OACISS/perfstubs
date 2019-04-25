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
    PERFSTUBS_START_FUNC();
    printf("Hello from new thread!\n");
    PERFSTUBS_STOP_FUNC();
    return NULL;
}

int main(int argc, char *argv[])
{
    PERFSTUBS_INIT();
    PERFSTUBS_START_FUNC();

    if (argc < 2)
    {
        fprintf(stderr, "%s Version %d.%d\n", argv[0], PerfStubs_VERSION_MAJOR,
                PerfStubs_VERSION_MINOR);
        fprintf(stderr, "Usage: %s number\n", argv[0]);
        PERFSTUBS_STOP_FUNC();
        return 1;
    }

    pthread_t example_thread;
    pthread_create(&example_thread, NULL, threaded_function, NULL);

    double inputValue = atof(argv[1]);
    PERFSTUBS_SAMPLE_COUNTER("input", inputValue);

    double outputValue = sqrt(inputValue);

    printf("The square root of %f is %f\n", inputValue, outputValue);

    pthread_join(example_thread, NULL);
    PERFSTUBS_STOP_FUNC();
    return 0;
}
