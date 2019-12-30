// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/Timer.h"

#include <unistd.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <iostream>
#include <thread>
#include <utility>
#include <unordered_map>

/* Make sure that the Timer singleton is constructed when the
 * library is loaded.  This will ensure (on linux, anyway) that
 * we can assert that we have m_Initialized on the main thread. */
static void __attribute__((constructor)) InitializeLibrary(void);

int perfstubs_initialized = PERFSTUBS_UNKNOWN;
bool static_constructor = true;

/* Function pointer types */

/* Logistical functions */
typedef void (*PerfStubsInitType)(void);
typedef void (*PerfStubsRegisterThreadType)(void);
typedef void (*PerfStubsExitType)(void);
typedef void (*PerfStubsDumpDataType)(void);
/* Data entry functions */
typedef void* (*PerfStubsTimerCreateType)(const char *);
typedef void (*PerfStubsTimerStartType)(const void *);
typedef void (*PerfStubsTimerStopType)(const void *);
typedef void (*PerfStubsSetParameterType)(const char *, int64_t);
typedef void (*PerfStubsDynamicPhaseStartType)(const char *, int);
typedef void (*PerfStubsDynamicPhaseStopType)(const char *, int);
typedef void* (*PerfStubsCreateCounterType)(const char *);
typedef void (*PerfStubsSampleCounterType)(const void *, double);
typedef void (*PerfStubsMetaDataType)(const char *, const char *);
/* Data Query Functions */
typedef void (*PerfStubsGetTimerDataType)(perftool_timer_data_t *);
typedef void (*PerfStubsGetCounterDataType)(perftool_counter_data_t *);
typedef void (*PerfStubsGetMetaDataType)(perftool_metadata_t *);
typedef void (*PerfStubsFreeTimerDataType)(perftool_timer_data_t *);
typedef void (*PerfStubsFreeCounterDataType)(perftool_counter_data_t *);
typedef void (*PerfStubsFreeMetaDataType)(perftool_metadata_t *);

/* Function pointers */

std::vector<PerfStubsInitType> MyPerfStubsInit;
std::vector<PerfStubsRegisterThreadType> MyPerfStubsRegisterThread;
std::vector<PerfStubsExitType> MyPerfStubsExit;
std::vector<PerfStubsDumpDataType> MyPerfStubsDumpData;
std::vector<PerfStubsTimerCreateType> MyPerfStubsTimerCreate;
std::vector<PerfStubsTimerStartType> MyPerfStubsTimerStart;
std::vector<PerfStubsTimerStopType> MyPerfStubsTimerStop;
std::vector<PerfStubsSetParameterType> MyPerfStubsSetParameter;
std::vector<PerfStubsDynamicPhaseStartType> MyPerfStubsDynamicPhaseStart;
std::vector<PerfStubsDynamicPhaseStopType> MyPerfStubsDynamicPhaseStop;
std::vector<PerfStubsCreateCounterType> MyPerfStubsCreateCounter;
std::vector<PerfStubsSampleCounterType> MyPerfStubsSampleCounter;
std::vector<PerfStubsMetaDataType> MyPerfStubsMetaData;
std::vector<PerfStubsGetTimerDataType> MyPerfStubsGetTimerData;
std::vector<PerfStubsGetCounterDataType> MyPerfStubsGetCounterData;
std::vector<PerfStubsGetMetaDataType> MyPerfStubsGetMetaData;
std::vector<PerfStubsFreeTimerDataType> MyPerfStubsFreeTimerData;
std::vector<PerfStubsFreeCounterDataType> MyPerfStubsFreeCounterData;
std::vector<PerfStubsFreeMetaDataType> MyPerfStubsFreeMetaData;

#define UNUSED(expr) do { (void)(expr); } while (0);

static void InitializeLibrary(void)
{
    // initialize the library by creating the singleton
    static external::PERFSTUBS_INTERNAL_NAMESPACE::Timer &tt =
        external::PERFSTUBS_INTERNAL_NAMESPACE::Timer::Get();
    UNUSED(tt);
}

#ifdef PERFSTUBS_USE_STATIC

#if defined(__clang__) && defined(__APPLE__)
#define PS_WEAK_PRE
#define PS_WEAK_POST __attribute__((weak_import))
#define PS_WEAK_POST_NULL __attribute__((weak_import))
#else
#define PS_WEAK_PRE __attribute__((weak))
#define PS_WEAK_POST
#define PS_WEAK_POST_NULL
#endif

extern "C"
{
    PS_WEAK_PRE void perftool_init(void) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_register_thread(void) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_exit(void) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_dump_data(void) PS_WEAK_POST;
    PS_WEAK_PRE void* perftool_timer_create(const char *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_timer_start(const void *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_timer_stop(const void *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_set_parameter(const char *, int64_t) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_dynamic_phase_start(const char *, int) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_dynamic_phase_stop(const char *, int) PS_WEAK_POST;
    PS_WEAK_PRE void* perftool_create_counter(const char *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_sample_counter(const void *, double) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_metadata(const char *, const char *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_get_timer_data(perftool_timer_data_t *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_get_counter_data(perftool_counter_data_t *)
        PS_WEAK_POST;
    PS_WEAK_PRE void perftool_get_metadata(perftool_metadata_t *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_free_timer_data(perftool_timer_data_t *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_free_counter_data(perftool_counter_data_t *) PS_WEAK_POST;
    PS_WEAK_PRE void perftool_free_metadata(perftool_metadata_t *) PS_WEAK_POST;
}
#endif

int AssignFunctionPointers(void)
{
#ifdef PERFSTUBS_USE_STATIC
    /* The initialization function is the only required one */
    MyPerfStubsInit.push_back(&perftool_init);
    if (MyPerfStubsInit == nullptr)
    {
        std::cout << "perftool_init not defined" << std::endl;
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsRegisterThread.push_back(&perftool_register_thread);
    MyPerfStubsExit.push_back(&perftool_exit);
    MyPerfStubsDumpData.push_back(&perftool_dump_data);
    MyPerfStubsTimerCreate.push_back(&perftool_timer_create);
    MyPerfStubsTimerStart.push_back(&perftool_timer_start);
    MyPerfStubsTimerStop.push_back(&perftool_timer_stop);
    MyPerfStubsSetParameter.push_back(&perftool_set_parameter);
    MyPerfStubsDynamicPhaseStart.push_back(&perftool_dynamic_phase_start);
    MyPerfStubsDynamicPhaseStop.push_back(&perftool_dynamic_phase_stop);
    MyPerfStubsCreateCounter.push_back(&perftool_create_counter);
    MyPerfStubsSampleCounter.push_back(&perftool_sample_counter);
    MyPerfStubsMetaData.push_back(&perftool_metadata);
    MyPerfStubsGetTimerData.push_back(&perftool_get_timer_data);
    MyPerfStubsGetCounterData.push_back(&perftool_get_counter_data);
    MyPerfStubsGetMetaData.push_back(&perftool_get_metadata);
    MyPerfStubsFreeTimerData.push_back(&perftool_free_timer_data);
    MyPerfStubsFreeCounterData.push_back(&perftool_free_counter_data);
    MyPerfStubsFreeMetaData.push_back(&perftool_free_metadata);
#else
    auto tmp = (PerfStubsInitType *)dlsym(RTLD_DEFAULT, "perftool_init");
    if (MyPerfStubsInit == nullptr)
    {
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsInit.push_back(tmp);
    MyPerfStubsRegisterThread.push_back((PerfStubsRegisterThreadType *)dlsym(
        RTLD_DEFAULT, "perftool_register_thread"));
    MyPerfStubsDumpData.push_back(
        (PerfStubsDumpDataType *)dlsym(RTLD_DEFAULT, "perftool_dump_data"));
    MyPerfStubsTimerCreate.push_back(
        (PerfStubsTimerCreateType *)dlsym(RTLD_DEFAULT,
        "perftool_timer_create"));
    MyPerfStubsTimerStart.push_back(
        (PerfStubsTimerStartType *)dlsym(RTLD_DEFAULT, "perftool_timer_start"));
    MyPerfStubsTimerStop.push_back(
        (PerfStubsTimerStopType *)dlsym(RTLD_DEFAULT, "perftool_timer_stop"));
    MyPerfStubsSetParameter.push_back(
        (PerfStubsSetParameterType *)dlsym(RTLD_DEFAULT, "perftool_set_parameter"));
    MyPerfStubsDynamicPhaseStart.push_back((PerfStubsDynamicPhaseStartType *)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_start"));
    MyPerfStubsDynamicPhaseStop.push_back((PerfStubsDynamicPhaseStopType *)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_stop"));
    MyPerfStubsCreateCounter.push_back((PerfStubsCreateCounterType *)dlsym(
        RTLD_DEFAULT, "perftool_create_counter"));
    MyPerfStubsSampleCounter.push_back((PerfStubsSampleCounterType *)dlsym(
        RTLD_DEFAULT, "perftool_sample_counter"));
    MyPerfStubsMetaData.push_back(
        (PerfStubsMetaDataType *)dlsym(RTLD_DEFAULT, "perftool_metadata"));
    MyPerfStubsGetTimerData.push_back((PerfStubsGetTimerDataType *)dlsym(
        RTLD_DEFAULT, "perftool_get_timer_data"));
    MyPerfStubsGetCounterData.push_back((PerfStubsGetCounterDataType *)dlsym(
        RTLD_DEFAULT, "perftool_get_counter_data"));
    MyPerfStubsGetMetaData.push_back((PerfStubsGetMetaDataType *)dlsym(
        RTLD_DEFAULT, "perftool_get_metadata"));
    MyPerfStubsFreeTimerData.push_back((PerfStubsFreeTimerDataType *)dlsym(
        RTLD_DEFAULT, "perftool_free_timer_data"));
    MyPerfStubsFreeCounterData.push_back((PerfStubsFreeCounterDataType *)dlsym(
        RTLD_DEFAULT, "perftool_free_counter_data"));
    MyPerfStubsFreeMetaData.push_back((PerfStubsFreeMetaDataType *)dlsym(
        RTLD_DEFAULT, "perftool_free_metadata"));
#endif
    return PERFSTUBS_SUCCESS;
}

int PerfStubsStubInitializeSimple(void)
{
    if (AssignFunctionPointers() == PERFSTUBS_FAILURE)
    {
#if defined(DEBUG) || defined(_DEBUG)
        // Can't write to std::cerr before main() is called...
        // so fail silently in that case.
        if (!static_constructor) {
            std::cerr << "ERROR: Unable to initialize the perftool API"
                      << std::endl;
        }
#endif
        return PERFSTUBS_FAILURE;
    }
    /* Initialize the external tool */
    MyPerfStubsInit();
    return PERFSTUBS_SUCCESS;
}

namespace external
{

namespace PERFSTUBS_INTERNAL_NAMESPACE
{

std::string Timer::MakeTimerName(const char * file,
    const char * func, int line) {
    std::stringstream ss;
    ss << func << " [{" << file << "} {" << line << ",0}]";
    std::string tmp(ss.str());
    return tmp;
}

thread_local bool Timer::m_ThreadSeen(false);

// constructor
Timer::Timer(void) : m_Initialized(false)
{
    if (PerfStubsStubInitializeSimple() == PERFSTUBS_SUCCESS)
    {
        m_Initialized = true;
        // set the external flag
        perfstubs_initialized = 1;
    }
    m_ThreadSeen = true;
}

// The only way to get an instance of this class
Timer &Timer::Get(void)
{
    static std::unique_ptr<Timer> instance(new Timer);
    if (!m_ThreadSeen && instance->m_Initialized)
    {
        _RegisterThread();
    }
    return *instance;
}

// used internally to the class
inline void Timer::_RegisterThread(void)
{
    if (!m_ThreadSeen) {
        for (auto f : MyPerfStubsRegisterThread)
        {
            f();
            m_ThreadSeen = true;
        }
    }
}

// external API call
void Timer::RegisterThread(void)
{
    static Timer &instance = Timer::Get();
    if (!instance.m_Initialized) { return; }
    _RegisterThread();
}

void* Timer::Create(const char *timer_name)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsTimerCreate != nullptr && instance.m_Initialized)
        return MyPerfStubsTimerCreate(timer_name);
    return nullptr;
}

void* Timer::Create(const std::string &timer_name)
{
    return Create(timer_name.c_str());
}

void Timer::Start(const void *timer)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsTimerStart != nullptr && instance.m_Initialized)
        MyPerfStubsTimerStart(timer);
}

void Timer::Stop(const void *timer)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsTimerStop != nullptr && instance.m_Initialized)
        MyPerfStubsTimerStop(timer);
}

void Timer::SetParameter(const char * parameter_name, int64_t parameter_value)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsSetParameter != nullptr && instance.m_Initialized)
        MyPerfStubsSetParameter(parameter_name, parameter_value);
}

void Timer::DynamicPhaseStart(const char *phase_prefix, int iteration_index)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsDynamicPhaseStart != nullptr && instance.m_Initialized)
        MyPerfStubsDynamicPhaseStart(phase_prefix, iteration_index);
}

void Timer::DynamicPhaseStart(const std::string &phase_prefix,
                              int iteration_index)
{
    DynamicPhaseStart(phase_prefix.c_str(), iteration_index);
}

void Timer::DynamicPhaseStop(const char *phase_prefix, int iteration_index)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsDynamicPhaseStop != nullptr && instance.m_Initialized)
        MyPerfStubsDynamicPhaseStop(phase_prefix, iteration_index);
}

void Timer::DynamicPhaseStop(const std::string &phase_prefix,
                             int iteration_index)
{
    DynamicPhaseStop(phase_prefix.c_str(), iteration_index);
}

void* Timer::CreateCounter(const char *name)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsCreateCounter != nullptr && instance.m_Initialized)
        return MyPerfStubsCreateCounter(const_cast<char *>(name));
    return nullptr;
}

void Timer::SampleCounter(const void *counter, const double value)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsSampleCounter != nullptr && instance.m_Initialized)
        MyPerfStubsSampleCounter(counter, value);
}

void Timer::MetaData(const char *name, const char *value)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsMetaData != nullptr && instance.m_Initialized)
        MyPerfStubsMetaData(name, value);
}

void Timer::DumpData(void)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsDumpData != nullptr && instance.m_Initialized)
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
    if (MyPerfStubsGetTimerData != nullptr && instance.m_Initialized)
        MyPerfStubsGetTimerData(timer_data);
}

void Timer::GetCounterData(perftool_counter_data_t *counter_data)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsGetCounterData != nullptr && instance.m_Initialized)
        MyPerfStubsGetCounterData(counter_data);
}

void Timer::GetMetaData(perftool_metadata_t *metadata)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsGetMetaData != nullptr && instance.m_Initialized)
        MyPerfStubsGetMetaData(metadata);
}

void Timer::FreeTimerData(perftool_timer_data_t *timer_data)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsFreeTimerData != nullptr && instance.m_Initialized)
        MyPerfStubsFreeTimerData(timer_data);
}

void Timer::FreeCounterData(perftool_counter_data_t *counter_data)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsFreeCounterData != nullptr && instance.m_Initialized)
        MyPerfStubsFreeCounterData(counter_data);
}

void Timer::FreeMetaData(perftool_metadata_t *metadata)
{
    static Timer &instance = Timer::Get();
    if (MyPerfStubsFreeMetaData != nullptr && instance.m_Initialized)
        MyPerfStubsFreeMetaData(metadata);
}

} // namespace PERFSTUBS_INTERNAL_NAMESPACE

} // namespace external

/* Expose the API to C */

namespace PSNS = external::PERFSTUBS_INTERNAL_NAMESPACE;

std::unordered_map<std::string, void*> fortran_timer_map;
std::unordered_map<std::string, void*> fortran_counter_map;

extern "C"
{ // C Bindings

    char * psMakeTimerName(const char * file, const char * func, int line) {
        // allocate enough length to hold the timer name
        int length = strlen(file) + strlen(func) + 32;
        char * tmpstr = (char*)(malloc(sizeof(char) * length));
        sprintf(tmpstr, "%s [{%s} {%d,0}]", func, file, line);
        return tmpstr;
    }

    void psInit() { static_constructor = false; InitializeLibrary(); }

    void psRegisterThread() { PSNS::Timer::RegisterThread(); }

    void psDumpData() { PSNS::Timer::DumpData(); }

    void* psTimerCreate(const char *timerName)
    {
        return PSNS::Timer::Create(timerName);
    }

    void psTimerStart(const void *timer)
    {
        PSNS::Timer::Start(timer);
    }

    void psTimerStop(const void *timer)
    {
        PSNS::Timer::Stop(timer);
    }

    void psSetParameter(const char *parameter_name, int64_t parameter_value)
    {
        PSNS::Timer::SetParameter(parameter_name, parameter_value);
    }

    void psDynamicPhaseStart(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStart(phase_prefix, iteration_index);
    }

    void psDynamicPhaseStop(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStop(phase_prefix, iteration_index);
    }

    void* psCreateCounter(const char *name)
    {
        return PSNS::Timer::CreateCounter(name);
    }

    void psSampleCounter(const char *counter, const double value)
    {
        PSNS::Timer::SampleCounter(counter, value);
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
    void psinit_() { static_constructor = false; InitializeLibrary(); }

    void psregisterthread_() { PSNS::Timer::RegisterThread(); }

    void psdumpdata_() { PSNS::Timer::DumpData(); }

    void * psfindtimer_(const char * timer_name)
    {
        std::string name(timer_name);
        auto iter = fortran_timer_map.find(name);
        if (iter == fortran_timer_map.end()) {
            void* p = PSNS::Timer::Create(timer_name);
            fortran_timer_map.insert(std::pair<std::string,void*>(name,p));
            return p;
        }
        return (void*)iter->second;
    }

    void pstimerstart_(const char *timerName)
    {
        PSNS::Timer::Start(psfindtimer_(timerName));
    }

    void pstimerstop_(const char *timerName)
    {
        PSNS::Timer::Stop(psfindtimer_(timerName));
    }

    void pssetparameter_(const char *parameterName, int64_t parameterValue)
    {
        PSNS::Timer::SetParameter(parameterName, parameterValue);
    }

    void psdynamicphasestart_(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStart(phase_prefix, iteration_index);
    }

    void psdynamicphasestop_(const char *phase_prefix, int iteration_index)
    {
        PSNS::Timer::DynamicPhaseStop(phase_prefix, iteration_index);
    }

    void * psfindcounter_(const char * counter_name)
    {
        std::string name(counter_name);
        auto iter = fortran_counter_map.find(name);
        if (iter == fortran_counter_map.end()) {
            void* p = PSNS::Timer::CreateCounter(counter_name);
            fortran_counter_map.insert(std::pair<std::string,void*>(name,p));
            return p;
        }
        return (void*)iter->second;
    }

    void pssamplecounter_(const char *name, const double value)
    {
        PSNS::Timer::SampleCounter(psfindcounter_(name), value);
    }

    void psmetadata_(const char *name, const char *value)
    {
        PSNS::Timer::MetaData(name, value);
    }

} // extern "C"

