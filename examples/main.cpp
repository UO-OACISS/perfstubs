// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

// A simple program that computes the square root of a number
#include "perfstubs_api/Timer.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <thread>

void threaded_function(void * param) {
    PERFSTUBS_REGISTER_THREAD();
    PERFSTUBS_SCOPED_TIMER_FUNC();
    //std::cout << "Hello from new thread!" << std::endl;
}

int main (int argc, char *argv[]) {
    PERFSTUBS_INIT();
    PERFSTUBS_SCOPED_TIMER_FUNC();
    
    if (argc < 2) {
        std::cerr << argv[0] << " Version "
                  << PerfStubs_VERSION_MAJOR << "."
                  << PerfStubs_VERSION_MINOR << std::endl;
        std::cerr << "Usage: " << argv[0]
                  << " number" << std::endl;
        return 1;
    }
    std::thread example_thread(threaded_function, nullptr);

    double inputValue = atof(argv[1]);
    PERFSTUBS_SAMPLE_COUNTER("input", inputValue);

    double outputValue = sqrt(inputValue);

    std::cout << "The square root of " << inputValue
              << " is " << outputValue << std::endl;

    example_thread.join();
    return 0;
}
