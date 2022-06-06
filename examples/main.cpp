// Copyright (c) 2019-2022 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

// A simple program that computes the square root of a number
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <thread>
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/timer.h"

void threaded_function(void *param)
{
    PERFSTUBS_REGISTER_THREAD();
    PERFSTUBS_SCOPED_TIMER_FUNC();
    std::cout << "Hello from new thread!" << std::endl;
    return;
}

double compute(double value)
{
    PERFSTUBS_SCOPED_TIMER_FUNC();
    PERFSTUBS_SET_PARAMETER("value", (int64_t)(value));
    return sqrt(value);
}

double compute2(double value)
{
    PERFSTUBS_START_STRING(__func__);
    double tmp = sqrt(value);
    PERFSTUBS_STOP_CURRENT();
    return tmp;
}

double compute3(double value)
{
    PERFSTUBS_START_STRING(__func__);
    double tmp = sqrt(value);
    PERFSTUBS_STOP_STRING(__func__);
    return tmp;
}

int main(int argc, char *argv[])
{
    PERFSTUBS_INITIALIZE();
    PERFSTUBS_SCOPED_TIMER_FUNC();

    {
        PERFSTUBS_SCOPED_TIMER("Argument Validation");
        if (argc < 2) {
            std::cerr << argv[0] << " Version "
                    << PerfStubs_VERSION_MAJOR << "."
                    << PerfStubs_VERSION_MINOR << std::endl;
            std::cerr << "Usage: " << argv[0] << " number" << std::endl;
            return 1;
        }
    }
    std::thread example_thread(threaded_function, nullptr);

    double inputValue = atof(argv[1]);
    PERFSTUBS_SAMPLE_COUNTER("input", inputValue);

    double outputValue;
    for (int i = 0; i < 5; i++)
    {
        PERFSTUBS_DYNAMIC_PHASE_START("Loop", i);
        outputValue = compute(inputValue);
        outputValue = compute2(inputValue);
        outputValue = compute3(inputValue);
        PERFSTUBS_DYNAMIC_PHASE_STOP("Loop", i);
    }

    example_thread.join();
    std::cout << "The square root of " << inputValue << " is " << outputValue
              << std::endl;

    PERFSTUBS_DUMP_DATA();
    PERFSTUBS_FINALIZE();
    return 0;
}
