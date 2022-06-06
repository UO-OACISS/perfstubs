/* Copyright (c) 2019-2022 University of Oregon
 * Distributed under the BSD Software License
 * (See accompanying file LICENSE.txt) */

#include <iostream>
#include <thread>
#include <vector>
#include <memory>
#include <unistd.h>
#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/timer.h"

void foo()
{
    PERFSTUBS_REGISTER_THREAD();
    PERFSTUBS_SCOPED_TIMER_FUNC();
    // do stuff...
    usleep(1);
}

int main(int argc, char* argv[])
{
    PERFSTUBS_INITIALIZE();
    PERFSTUBS_SCOPED_TIMER_FUNC();
    /* Get the number of cores on this machine */
    unsigned int cores = std::thread::hardware_concurrency();
    cores = cores > 0 ? cores : sysconf(_SC_NPROCESSORS_ONLN);

    // launch threads:
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < cores ; i++) {
        std::thread t(foo);
        threads.push_back(std::move(t));
    }
    // synchronize threads:
    for (auto & t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
}


