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
typedef void PerfStubsInitType(void);
typedef void PerfStubsRegisterThreadType(void);
typedef void PerfStubsExitType(void);
typedef void PerfStubsDumpDataType(void);
/* Data entry functions */
typedef void PerfStubsTimerStartType(const char *);
typedef void PerfStubsTimerStopType(const char *);
typedef void PerfStubsDynamicPhaseStartType(const char *, int);
typedef void PerfStubsDynamicPhaseStopType(const char *, int);
typedef void PerfStubsSampleCounterType(const char *, double);
typedef void PerfStubsMetaDataType(const char *, const char *);
/* Data Query Functions */
typedef void PerfStubsGetTimerDataType(perftool_timer_data_t *);
typedef void PerfStubsGetCounterDataType(perftool_counter_data *);
typedef void PerfStubsGetMetaDataType(perftool_metadata_t *);
typedef void PerfStubsFreeTimerDataType(perftool_timer_data_t *);
typedef void PerfStubsFreeCounterDataType(perftool_counter_data *);
typedef void PerfStubsFreeMetaDataType(perftool_metadata_t *);

/* Function pointers */

PerfStubsInitType *MyPerfStubsInit = nullptr;
PerfStubsRegisterThreadType *MyPerfStubsRegisterThread = nullptr;
PerfStubsExitType *MyPerfStubsExit = nullptr;
PerfStubsDumpDataType *MyPerfStubsDumpData = nullptr;
PerfStubsTimerStartType *MyPerfStubsTimerStart = nullptr;
PerfStubsTimerStopType *MyPerfStubsTimerStop = nullptr;
PerfStubsDynamicPhaseStartType *MyPerfStubsDynamicPhaseStart = nullptr;
PerfStubsDynamicPhaseStopType *MyPerfStubsDynamicPhaseStop = nullptr;
PerfStubsSampleCounterType *MyPerfStubsSampleCounter = nullptr;
PerfStubsMetaDataType *MyPerfStubsMetaData = nullptr;
PerfStubsGetTimerDataType *MyPerfStubsGetTimerData = nullptr;
PerfStubsGetCounterDataType *MyPerfStubsGetCounterData = nullptr;
PerfStubsGetMetaDataType *MyPerfStubsGetMetaData = nullptr;
PerfStubsFreeTimerDataType *MyPerfStubsFreeTimerData = nullptr;
PerfStubsFreeCounterDataType *MyPerfStubsFreeCounterData = nullptr;
PerfStubsFreeMetaDataType *MyPerfStubsFreeMetaData = nullptr;

#define UNUSED(expr) do { (void)(expr); } while (0);

static void InitializeLibrary(void)
{
    // initialize the library by creating the singleton
    static external::PERFSTUBS_INTERNAL_NAMESPACE::Timer &tt =
        external::PERFSTUBS_INTERNAL_NAMESPACE::Timer::Get();
    UNUSED(tt);
}

#ifdef PERFSTUBS_USE_STATIC
extern "C"
{
    void perftool_init(void) __attribute((weak));
    void perftool_register_thread(void) __attribute((weak));
    void perftool_exit(void) __attribute((weak));
    void perftool_dump_data(void) __attribute((weak));
    void perftool_timer_start(const char *) __attribute((weak));
    void perftool_timer_stop(const char *) __attribute((weak));
    void perftool_dynamic_phase_start(const char *, int) __attribute((weak));
    void perftool_dynamic_phase_stop(const char *, int) __attribute((weak));
    void perftool_sample_counter(const char *, double) __attribute((weak));
    void perftool_metadata(const char *, const char *) __attribute((weak));
    void perftool_get_timer_data(perftool_timer_data_t *) __attribute((weak));
    void perftool_get_counter_data(perftool_counter_data_t *)
        __attribute((weak));
    void perftool_get_metadata(perftool_metadata_t *) __attribute((weak));
}
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
    MyPerfStubsRegisterThread = &perftool_register_thread;
    MyPerfStubsExit = &perftool_exit;
    MyPerfStubsDumpData = &perftool_dump_data;
    MyPerfStubsTimerStart = &perftool_timer_start;
    MyPerfStubsTimerStop = &perftool_timer_stop;
    MyPerfStubsDynamicPhaseStart = &perftool_dynamic_phase_start;
    MyPerfStubsDynamicPhaseStop = &perftool_dynamic_phase_stop;
    MyPerfStubsSampleCounter = &perftool_sample_counter;
    MyPerfStubsMetaData = &perftool_metadata;
    MyPerfStubsGetTimerData = &perftool_get_timer_data;
    MyPerfStubsGetCounterData = &perftool_get_counter_data;
    MyPerfStubsGetMetaData = &perftool_get_metadata;
    MyPerfStubsFreeTimerData = &perftool_free_timer_data;
    MyPerfStubsFreeCounterData = &perftool_free_counter_data;
    MyPerfStubsFreeMetaData = &perftool_free_metadata;
#else
    MyPerfStubsInit = (PerfStubsInitType *)dlsym(RTLD_DEFAULT, "perftool_init");
    if (MyPerfStubsInit == nullptr)
    {
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsRegisterThread = (PerfStubsRegisterThreadType *)dlsym(
        RTLD_DEFAULT, "perftool_register_thread");
    MyPerfStubsDumpData =
        (PerfStubsDumpDataType *)dlsym(RTLD_DEFAULT, "perftool_dump_data");
    MyPerfStubsTimerStart =
        (PerfStubsTimerStartType *)dlsym(RTLD_DEFAULT, "perftool_timer_start");
    MyPerfStubsTimerStop =
        (PerfStubsTimerStopType *)dlsym(RTLD_DEFAULT, "perftool_timer_stop");
    MyPerfStubsDynamicPhaseStart = (PerfStubsDynamicPhaseStartType *)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_start");
    MyPerfStubsDynamicPhaseStop = (PerfStubsDynamicPhaseStopType *)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_stop");
    MyPerfStubsSampleCounter = (PerfStubsSampleCounterType *)dlsym(
        RTLD_DEFAULT, "perftool_sample_counter");
    MyPerfStubsMetaData =
        (PerfStubsMetaDataType *)dlsym(RTLD_DEFAULT, "perftool_metadata");
    MyPerfStubsGetTimerData = (PerfStubsGetTimerDataType *)dlsym(
        RTLD_DEFAULT, "perftool_get_timer_data");
    MyPerfStubsGetCounterData = (PerfStubsGetCounterDataType *)dlsym(
        RTLD_DEFAULT, "perftool_get_counter_data");
    MyPerfStubsGetMetaData = (PerfStubsGetMetaDataType *)dlsym(
        RTLD_DEFAULT, "perftool_get_metadata");
    MyPerfStubsFreeTimerData = (PerfStubsFreeTimerDataType *)dlsym(
        RTLD_DEFAULT, "perftool_free_timer_data");
    MyPerfStubsFreeCounterData = (PerfStubsFreeCounterDataType *)dlsym(
        RTLD_DEFAULT, "perftool_free_counter_data");
    MyPerfStubsFreeMetaData = (PerfStubsFreeMetaDataType *)dlsym(
        RTLD_DEFAULT, "perftool_free_metadata");
#endif
    return PERFSTUBS_SUCCESS;
}

int PerfStubsStubInitializeSimple(void)
{
    if (AssignFunctionPointers() == PERFSTUBS_FAILURE)
    {
#if defined(DEBUG) || defined(_DEBUG)
        std::cerr << "ERROR: Unable to initialize the perftool API"
                  << std::endl;
#endif
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsInit();
    return PERFSTUBS_SUCCESS;
}

namespace external
{

namespace PERFSTUBS_INTERNAL_NAMESPACE
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
    if (!m_ThreadSeen && MyPerfStubsRegisterThread != nullptr)
    {
        MyPerfStubsRegisterThread();
        m_ThreadSeen = true;
    }
}

// external API call
void Timer::RegisterThread(void)
{
    static Timer &instance = Timer::Get();
    if (!instance.m_Initialized) { return; }
    _RegisterThread();
}

void Timer::Start(const char *timer_name)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsTimerStart != nullptr)
        MyPerfStubsTimerStart(timer_name);
}

void Timer::Start(const std::string &timer_name)
{
    Start(timer_name.c_str());
}

void Timer::DynamicPhaseStart(const char *phase_prefix, int iteration_index)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsDynamicPhaseStart != nullptr)
        MyPerfStubsDynamicPhaseStart(phase_prefix, iteration_index);
}

void Timer::DynamicPhaseStart(const std::string &phase_prefix,
                              int iteration_index)
{
    DynamicPhaseStart(phase_prefix.c_str(), iteration_index);
}

void Timer::Stop(const char *timer_name)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsTimerStop != nullptr)
        MyPerfStubsTimerStop(timer_name);
}

void Timer::Stop(const std::string &timer_name)
{
    Stop(timer_name.c_str());
}

void Timer::DynamicPhaseStop(const char *phase_prefix, int iteration_index)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsDynamicPhaseStop != nullptr)
        MyPerfStubsDynamicPhaseStop(phase_prefix, iteration_index);
}

void Timer::DynamicPhaseStop(const std::string &phase_prefix,
                             int iteration_index)
{
    DynamicPhaseStop(phase_prefix.c_str(), iteration_index);
}

void Timer::SampleCounter(const char *name, const double value)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsSampleCounter != nullptr)
        MyPerfStubsSampleCounter(const_cast<char *>(name), value);
}

void Timer::MetaData(const char *name, const char *value)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsMetaData != nullptr)
        MyPerfStubsMetaData(name, value);
}

void Timer::DumpData(void)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsDumpData != nullptr)
        MyPerfStubsDumpData();
}

Timer::~Timer(void)
{
    if (m_Initialized && MyPerfStubsExit != nullptr)
        MyPerfStubsExit();
}

void Timer::GetTimerData(perftool_timer_data_t *timer_data)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsGetTimerData != nullptr)
        MyPerfStubsGetTimerData(timer_data);
}

void Timer::GetCounterData(perftool_counter_data_t *counter_data)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsGetCounterData != nullptr)
        MyPerfStubsGetCounterData(counter_data);
}

void Timer::GetMetaData(perftool_metadata_t *metadata)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsGetMetaData != nullptr)
        MyPerfStubsGetMetaData(metadata);
}

void Timer::FreeTimerData(perftool_timer_data_t *timer_data)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsFreeTimerData != nullptr)
        MyPerfStubsFreeTimerData(timer_data);
}

void Timer::FreeCounterData(perftool_counter_data_t *counter_data)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsFreeCounterData != nullptr)
        MyPerfStubsFreeCounterData(counter_data);
}

void Timer::FreeMetaData(perftool_metadata_t *metadata)
{
    static Timer &instance = Timer::Get();
    if (instance.m_Initialized && MyPerfStubsFreeMetaData != nullptr)
        MyPerfStubsFreeMetaData(metadata);
}

} // namespace PERFSTUBS_INTERNAL_NAMESPACE

} // namespace external

/* Expose the API to C */

namespace PSNS = external::PERFSTUBS_INTERNAL_NAMESPACE;
extern "C"
{ // C Bindings

    void psInit() { InitializeLibrary(); }

    void psRegisterThread() { PSNS::Timer::RegisterThread(); }

    void psDumpData() { PSNS::Timer::DumpData(); }

    void psTimerStart(const char *timerName)
    {
        PSNS::Timer::Start(timerName);
    }

    void psTimerStop(const char *timerName)
    {
        PSNS::Timer::Stop(timerName);
    }

    void psDynamicPhaseStart(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStart(phase_prefix, iteration_index);
    }

    void psDynamicPhaseStop(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStop(phase_prefix, iteration_index);
    }

    void psSampleCounter(const char *name, const double value)
    {
        PSNS::Timer::SampleCounter(name, value);
    }

    void psMetaData(const char *name, const char *value)
    {
        PSNS::Timer::MetaData(name, value);
    }

    void psGetTimerData(perftool_timer_data_t *timer_data)
    {
        PSNS::Timer::GetTimerData(timer_data);
    }

    void psGetCounterData(perftool_counter_data_t *counter_data)
    {
        PSNS::Timer::GetCounterData(counter_data);
    }

    void psGetMetaData(perftool_metadata_t *metadata)
    {
        PSNS::Timer::GetMetaData(metadata);
    }

    void psFreeTimerData(perftool_timer_data_t *timer_data)
    {
        PSNS::Timer::FreeTimerData(timer_data);
    }

    void psFreeCounterData(perftool_counter_data_t *counter_data)
    {
        PSNS::Timer::FreeCounterData(counter_data);
    }

    void psFreeMetaData(perftool_metadata_t *metadata)
    {
        PSNS::Timer::FreeMetaData(metadata);
    }

    /* End of C function definitions */

    // Fortran Bindings
    void psinit_() { InitializeLibrary(); }

    void psregisterthread_() { PSNS::Timer::RegisterThread(); }

    void psdumpdata_() { PSNS::Timer::DumpData(); }

    void pstimerstart_(const char *timerName)
    {
        PSNS::Timer::Start(timerName);
    }
    void pstimerstop_(const char *timerName)
    {
        PSNS::Timer::Stop(timerName);
    }

    void psdynamicphasestart_(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStart(phase_prefix, iteration_index);
    }

    void psdynamicphasestop_(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStop(phase_prefix, iteration_index);
    }

    void pssamplecounter_(const char *name, const double value)
    {
        PSNS::Timer::SampleCounter(name, value);
    }

    void psmetadata_(const char *name, const char *value)
    {
        PSNS::Timer::MetaData(name, value);
    }

} // extern "C"

#endif // defined(PERFSTUBS_USE_TIMERS)
