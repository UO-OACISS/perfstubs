/* Copyright (c) 2019-2022 University of Oregon
 * Distributed under the BSD Software License
 * (See accompanying file LICENSE.txt) */

// A simple program that computes the square root of a number
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <thread>
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/timer.h"

int main(int argc, char *argv[])
{
    PERFSTUBS_INITIALIZE();
    PERFSTUBS_REGISTER_THREAD()
    PERFSTUBS_METADATA("meta", "data")
    PERFSTUBS_SCOPED_TIMER_FUNC();
    PERFSTUBS_SET_PARAMETER("argc", argc);

    // starts
    PERFSTUBS_TIMER_START(timer2, "timer")

    {
        PERFSTUBS_SCOPED_TIMER("scoped");
    }
    for (int i = 0 ; i < 5; i++) {
        PERFSTUBS_DYNAMIC_PHASE_START("iter", i)
        PERFSTUBS_DYNAMIC_PHASE_STOP("iter", i)
    }

    // stops
    PERFSTUBS_TIMER_STOP(timer2)

    // pause
    PERFSTUBS_PAUSE_MEASUREMENT()
    PERFSTUBS_TIMER_START(timer3, "timer should be ignored")
    PERFSTUBS_TIMER_STOP(timer3)
    PERFSTUBS_RESUME_MEASUREMENT()

    PERFSTUBS_SAMPLE_COUNTER("counter", 15.0)
    PERFSTUBS_DUMP_DATA();
    PERFSTUBS_FINALIZE();
    return 0;
}
