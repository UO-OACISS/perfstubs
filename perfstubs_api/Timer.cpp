// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#include "perfstubs_api/Timer.h"

/* If not enabled, macro out all of the code in this file. */
#if defined(PERFSTUBS_USE_TIMERS)

#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <iostream>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <thread>

#define PERFSTUBS_SUCCESS 0
#define PERFSTUBS_FAILURE 1

/* Make sure that the Timer singleton is constructed when the
 * library is loaded.  This will ensure (on linux, anyway) that
 * we can assert that we have m_Initialized on the main thread. */
// static void __attribute__((constructor)) InitializeLibrary(void);

/* Function pointer types */

/* Logistical functions */
typedef void (*PerfStubsInitType)(void);
typedef void (*PerfStubsRegisterThreadType)(void);
typedef void (*PerfStubsExitType)(void);
/* Data entry functions */
typedef void (*PerfStubsStartType)(const char *);
typedef void (*PerfStubsStopType)(const char *);
typedef void (*PerfStubsSampleCounterType)(const char *, double);
typedef void (*PerfStubsMetadataType)(const char *, const char *);
// typedef std::vector<double> (*PerfStubsGetProfileType)(const char *, bool
// allThreads);

/* Function pointers */

void (*MyPerfStubsInit)(void) = nullptr;
void (*MyPerfStubsRegisterThread)(void) = nullptr;
void (*MyPerfStubsExit)(void) = nullptr;
void (*MyPerfStubsStart)(const char *) = nullptr;
void (*MyPerfStubsStop)(const char *) = nullptr;
void (*MyPerfStubsSampleCounter)(const char *, double) = nullptr;
void (*MyPerfStubsMetadata)(const char *, const char *) = nullptr;
// std::vector<double> (*MyPerfStubsGetProfile)(const char *, bool) = nullptr;

#define UNUSED(expr)                                                           \
    do                                                                         \
    {                                                                          \
        (void)(expr);                                                          \
    } while (0)

static void InitializeLibrary(void)
{
    // initialize the library by creating the singleton
    static external::profiler::Timer &tt = external::profiler::Timer::Get();
    UNUSED(tt);
}

#ifdef PERFSTUBS_USE_STATIC
extern "C" void perftool_init(void) __attribute((weak));
extern "C" void perftool_register_thread(void) __attribute((weak));
extern "C" void perftool_exit(void) __attribute((weak));
extern "C" void perftool_start(const char *) __attribute((weak));
extern "C" void perftool_stop(const char *) __attribute((weak));
extern "C" void perftool_sample_counter(const char *, double)
    __attribute((weak));
extern "C" void perftool_metadata(const char *, const char *)
    __attribute((weak));
#endif

int AssignFunctionPointers(void)
{
#ifdef PERFSTUBS_USE_STATIC
    MyPerfStubsInit = &perftool_init;
    if (MyPerfStubsInit == nullptr)
    {
        std::cout << "perftool_init not defined" << std::endl;
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsRegisterThread = perftool_register_thread;
    MyPerfStubsStart = &perftool_start;
    MyPerfStubsStop = &perftool_stop;
    MyPerfStubsExit = &perftool_exit;
    MyPerfStubsSampleCounter = &perftool_sample_counter;
    MyPerfStubsMetadata = &perftool_metadata;
#else
    MyPerfStubsInit = (PerfStubsInitType)dlsym(RTLD_DEFAULT, "perftool_init");
    if (MyPerfStubsInit == nullptr)
    {
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsRegisterThread = (PerfStubsRegisterThreadType)dlsym(
        RTLD_DEFAULT, "perftool_register_thread");
    MyPerfStubsStart =
        (PerfStubsStartType)dlsym(RTLD_DEFAULT, "perftool_start");
    MyPerfStubsStop = (PerfStubsStopType)dlsym(RTLD_DEFAULT, "perftool_stop");
    MyPerfStubsExit = (PerfStubsExitType)dlsym(RTLD_DEFAULT, "perftool_exit");
    MyPerfStubsSampleCounter = (PerfStubsSampleCounterType)dlsym(
        RTLD_DEFAULT, "perftool_sample_counter");
    MyPerfStubsMetadata =
        (PerfStubsMetadataType)dlsym(RTLD_DEFAULT, "perftool_metadata");
#endif
    return PERFSTUBS_SUCCESS;
}

int PerfStubsStubInitializeSimple(void)
{
    if (AssignFunctionPointers() == PERFSTUBS_FAILURE)
    {
        std::cerr << "ERROR: Unable to initialize the perftool API"
                  << std::endl;
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsInit();
    return PERFSTUBS_SUCCESS;
}

namespace external
{

namespace profiler
{

thread_local bool Timer::m_ThreadSeen(false);

// constructor
Timer::Timer(void) : m_Initialized(false)
{
    if (PerfStubsStubInitializeSimple() == PERFSTUBS_SUCCESS)
    {
        m_Initialized = true;
    }
    m_ThreadSeen = true;
}

// The only way to get an instance of this class
Timer &Timer::Get(void)
{
    static std::unique_ptr<Timer> instance(new Timer);
    if (!m_ThreadSeen && instance.get()->m_Initialized)
    {
        _RegisterThread();
    }
    return *instance;
}

// used internally to the class
inline void Timer::_RegisterThread(void)
{
    if (!m_ThreadSeen)
    {
        MyPerfStubsRegisterThread();
        m_ThreadSeen = true;
    }
}

// external API call
void Timer::RegisterThread(void)
{
    if (!Timer::Get().m_Initialized)
        return;
    _RegisterThread();
}

void Timer::Start(const char *timer_name)
{
    if (!Timer::Get().m_Initialized)
        return;
    MyPerfStubsStart(timer_name);
}

void Timer::Start(const std::string &timer_name)
{
    if (!Timer::Get().m_Initialized)
        return;
    MyPerfStubsStart(timer_name.c_str());
}

void Timer::Stop(const char *timer_name)
{
    if (!Timer::Get().m_Initialized)
        return;
    MyPerfStubsStop(timer_name);
}

void Timer::Stop(const std::string &timer_name)
{
    if (!Timer::Get().m_Initialized)
        return;
    MyPerfStubsStop(timer_name.c_str());
}

void Timer::SampleCounter(const char *name, const double value)
{
    if (!Timer::Get().m_Initialized)
        return;
    MyPerfStubsSampleCounter(const_cast<char *>(name), value);
}

void Timer::MetaData(const char *name, const char *value)
{
    if (!Timer::Get().m_Initialized)
        return;
    MyPerfStubsMetadata(name, value);
}

Timer::~Timer(void)
{
    if (MyPerfStubsExit == nullptr)
        return;
    MyPerfStubsExit();
}

} // namespace profiler

} // namespace external

/* Expose the API to C */

extern "C" { // C Bindings

void TimerInit() { InitializeLibrary(); }

void TimerRegisterThread() { external::profiler::Timer::RegisterThread(); }

void TimerStart(const char *timerName)
{
    external::profiler::Timer::Start(timerName);
}

void TimerStop(const char *timerName)
{
    external::profiler::Timer::Stop(timerName);
}

void TimerSampleCounter(const char *name, const double value)
{
    external::profiler::Timer::SampleCounter(name, value);
}

void TimerMetaData(const char *name, const char *value)
{
    external::profiler::Timer::MetaData(name, value);
}

/* End of C function definitions */

} // extern "C"

#endif // defined(PERFSTUBS_USE_TIMERS)
