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
#include <vector>

/* Make sure that the Timer singleton is constructed when the
 * library is loaded.  This will ensure (on linux, anyway) that
 * we can assert that we have m_Initialized on the main thread. */
//static void __attribute__((constructor)) InitializeLibrary(void);
/* Disabled, because the vectors that get initialized before main()
 * aren't the same vectors that get evaluated after main()...? */

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
    auto init_function = &perftool_init;
    if (init_function == nullptr)
    {
        std::cout << "perftool_init not defined" << std::endl;
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsInit.push_back(init_function);
    if (&perftool_register_thread != nullptr) {
        MyPerfStubsRegisterThread.push_back(&perftool_register_thread);
    }
    if (&perftool_exit != nullptr) {
        MyPerfStubsExit.push_back(&perftool_exit);
    }
    if (&perftool_dump_data != nullptr) {
        MyPerfStubsDumpData.push_back(&perftool_dump_data);
    }
    if (&perftool_timer_create != nullptr) {
        MyPerfStubsTimerCreate.push_back(&perftool_timer_create);
    }
    if (&perftool_timer_start != nullptr) {
        MyPerfStubsTimerStart.push_back(&perftool_timer_start);
    }
    if (&perftool_timer_stop != nullptr) {
        MyPerfStubsTimerStop.push_back(&perftool_timer_stop);
    }
    if (&perftool_set_parameter != nullptr) {
        MyPerfStubsSetParameter.push_back(&perftool_set_parameter);
    }
    if (&perftool_dynamic_phase_start != nullptr) {
        MyPerfStubsDynamicPhaseStart.push_back(&perftool_dynamic_phase_start);
    }
    if (&perftool_dynamic_phase_stop != nullptr) {
        MyPerfStubsDynamicPhaseStop.push_back(&perftool_dynamic_phase_stop);
    }
    if (&perftool_create_counter != nullptr) {
        MyPerfStubsCreateCounter.push_back(&perftool_create_counter);
    }
    if (&perftool_sample_counter != nullptr) {
        MyPerfStubsSampleCounter.push_back(&perftool_sample_counter);
    }
    if (&perftool_metadata != nullptr) {
        MyPerfStubsMetaData.push_back(&perftool_metadata);
    }
    if (&perftool_get_timer_data != nullptr) {
        MyPerfStubsGetTimerData.push_back(&perftool_get_timer_data);
    }
    if (&perftool_get_counter_data != nullptr) {
        MyPerfStubsGetCounterData.push_back(&perftool_get_counter_data);
    }
    if (&perftool_get_metadata != nullptr) {
        MyPerfStubsGetMetaData.push_back(&perftool_get_metadata);
    }
    if (&perftool_free_timer_data != nullptr) {
        MyPerfStubsFreeTimerData.push_back(&perftool_free_timer_data);
    }
    if (&perftool_free_counter_data != nullptr) {
        MyPerfStubsFreeCounterData.push_back(&perftool_free_counter_data);
    }
    if (&perftool_free_metadata != nullptr) {
        MyPerfStubsFreeMetaData.push_back(&perftool_free_metadata);
    }
#else
    auto tmp = (PerfStubsInitType)dlsym(RTLD_DEFAULT, "perftool_init");
    if (tmp == nullptr)
    {
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsInit.push_back(tmp);
    MyPerfStubsRegisterThread.push_back((PerfStubsRegisterThreadType)dlsym(
        RTLD_DEFAULT, "perftool_register_thread"));
    MyPerfStubsDumpData.push_back(
        (PerfStubsDumpDataType)dlsym(RTLD_DEFAULT, "perftool_dump_data"));
    MyPerfStubsTimerCreate.push_back(
        (PerfStubsTimerCreateType)dlsym(RTLD_DEFAULT,
        "perftool_timer_create"));
    MyPerfStubsTimerStart.push_back(
        (PerfStubsTimerStartType)dlsym(RTLD_DEFAULT, "perftool_timer_start"));
    MyPerfStubsTimerStop.push_back(
        (PerfStubsTimerStopType)dlsym(RTLD_DEFAULT, "perftool_timer_stop"));
    MyPerfStubsSetParameter.push_back(
        (PerfStubsSetParameterType)dlsym(RTLD_DEFAULT, "perftool_set_parameter"));
    MyPerfStubsDynamicPhaseStart.push_back((PerfStubsDynamicPhaseStartType)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_start"));
    MyPerfStubsDynamicPhaseStop.push_back((PerfStubsDynamicPhaseStopType)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_stop"));
    MyPerfStubsCreateCounter.push_back((PerfStubsCreateCounterType)dlsym(
        RTLD_DEFAULT, "perftool_create_counter"));
    MyPerfStubsSampleCounter.push_back((PerfStubsSampleCounterType)dlsym(
        RTLD_DEFAULT, "perftool_sample_counter"));
    MyPerfStubsMetaData.push_back(
        (PerfStubsMetaDataType)dlsym(RTLD_DEFAULT, "perftool_metadata"));
    MyPerfStubsGetTimerData.push_back((PerfStubsGetTimerDataType)dlsym(
        RTLD_DEFAULT, "perftool_get_timer_data"));
    MyPerfStubsGetCounterData.push_back((PerfStubsGetCounterDataType)dlsym(
        RTLD_DEFAULT, "perftool_get_counter_data"));
    MyPerfStubsGetMetaData.push_back((PerfStubsGetMetaDataType)dlsym(
        RTLD_DEFAULT, "perftool_get_metadata"));
    MyPerfStubsFreeTimerData.push_back((PerfStubsFreeTimerDataType)dlsym(
        RTLD_DEFAULT, "perftool_free_timer_data"));
    MyPerfStubsFreeCounterData.push_back((PerfStubsFreeCounterDataType)dlsym(
        RTLD_DEFAULT, "perftool_free_counter_data"));
    MyPerfStubsFreeMetaData.push_back((PerfStubsFreeMetaDataType)dlsym(
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
    for (auto f : MyPerfStubsInit) {
        f();
    }
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

// Macro to do boilerplate initialization check
#define CHECK_INITIALIZED \
static Timer &instance = Timer::Get(); \
if (!instance.m_Initialized) { return; }

#define CHECK_INITIALIZED_RETURN_NULLPTR \
static Timer &instance = Timer::Get(); \
if (!instance.m_Initialized) { return nullptr; }


// external API call
void Timer::RegisterThread(void)
{
    CHECK_INITIALIZED
    _RegisterThread();
}

void* Timer::Create(const char *timer_name)
{
    CHECK_INITIALIZED_RETURN_NULLPTR
    size_t num_tools = MyPerfStubsTimerCreate.size();
    void ** objects = (void**)calloc(num_tools, sizeof(void*));
    int index = 0;
    for (auto f : MyPerfStubsTimerCreate) {
        if (f != nullptr) {
            objects[index] = f(timer_name);
        }
        index++;
    }
    return (void*)(objects);
}

void* Timer::Create(const std::string &timer_name)
{
    return Create(timer_name.c_str());
}

void Timer::Start(const void *timer)
{
    CHECK_INITIALIZED
    void ** objects = (void**)timer;
    int index = 0;
    for(auto f : MyPerfStubsTimerStart) {
        if (f != nullptr) {
            f(objects[index]);
        }
        index++;
    }
}

void Timer::Stop(const void *timer)
{
    CHECK_INITIALIZED
    void ** objects = (void**)timer;
    int index = 0;
    for(auto f : MyPerfStubsTimerStop) {
        if (f != nullptr) {
            f(objects[index++]);
        }
        index++;
    }
}

void Timer::SetParameter(const char * parameter_name, int64_t parameter_value)
{
    CHECK_INITIALIZED
    for(auto f : MyPerfStubsSetParameter) {
        if (f != nullptr) {
            f(parameter_name, parameter_value);
        }
    }
}

void Timer::DynamicPhaseStart(const char *phase_prefix, int iteration_index)
{
    CHECK_INITIALIZED
    for(auto f : MyPerfStubsDynamicPhaseStart) {
        if (f != nullptr) {
            f(phase_prefix, iteration_index);
        }
    }
}

void Timer::DynamicPhaseStart(const std::string &phase_prefix,
                              int iteration_index)
{
    DynamicPhaseStart(phase_prefix.c_str(), iteration_index);
}

void Timer::DynamicPhaseStop(const char *phase_prefix, int iteration_index)
{
    CHECK_INITIALIZED
    for(auto f : MyPerfStubsDynamicPhaseStop) {
        if (f != nullptr) {
            f(phase_prefix, iteration_index);
        }
    }
}

void Timer::DynamicPhaseStop(const std::string &phase_prefix,
                             int iteration_index)
{
    DynamicPhaseStop(phase_prefix.c_str(), iteration_index);
}

void* Timer::CreateCounter(const char *name)
{
    CHECK_INITIALIZED_RETURN_NULLPTR
    size_t num_tools = MyPerfStubsTimerCreate.size();
    void ** objects = (void**)calloc(num_tools, sizeof(void*));
    int index = 0;
    for(auto f : MyPerfStubsCreateCounter) {
        if (f != nullptr) {
            objects[index] = f(const_cast<char *>(name));
        }
        index++;
    }
    return (void*)(objects);
}

void Timer::SampleCounter(const void *counter, const double value)
{
    CHECK_INITIALIZED
    void ** objects = (void**)(counter);
    int index = 0;
    for(auto f : MyPerfStubsSampleCounter) {
        if (f != nullptr) {
            f(objects[index], value);
        }
        index++;
    }
}

void Timer::MetaData(const char *name, const char *value)
{
    CHECK_INITIALIZED
    for(auto f : MyPerfStubsMetaData) {
        if (f != nullptr) {
            f(name, value);
        }
    }
}

void Timer::DumpData(void)
{
    CHECK_INITIALIZED
    for(auto f : MyPerfStubsDumpData) {
        if (f != nullptr) {
            f();
        }
    }
}

Timer::~Timer(void)
{
    for(auto f : MyPerfStubsExit) {
        if (f != nullptr) {
            f();
        }
    }
}

void Timer::GetTimerData(perftool_timer_data_t *timer_data, int tool_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsGetTimerData[tool_index] != nullptr) {
        MyPerfStubsGetTimerData[tool_index](timer_data);
    }
}

void Timer::GetCounterData(perftool_counter_data_t *counter_data, int tool_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsGetCounterData[tool_index] != nullptr) {
        MyPerfStubsGetCounterData[tool_index](counter_data);
    }
}

void Timer::GetMetaData(perftool_metadata_t *metadata, int tool_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsGetMetaData[tool_index] != nullptr) {
        MyPerfStubsGetMetaData[tool_index](metadata);
    }
}

void Timer::FreeTimerData(perftool_timer_data_t *timer_data, int tool_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsFreeTimerData[tool_index] != nullptr) {
        MyPerfStubsFreeTimerData[tool_index](timer_data);
    }
}

void Timer::FreeCounterData(perftool_counter_data_t *counter_data, int tool_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsFreeCounterData[tool_index] != nullptr) {
        MyPerfStubsFreeCounterData[tool_index](counter_data);
    }
}

void Timer::FreeMetaData(perftool_metadata_t *metadata, int tool_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsFreeMetaData[tool_index] != nullptr) {
        MyPerfStubsFreeMetaData[tool_index](metadata);
    }
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

    void psGetTimerData(perftool_timer_data_t *timer_data, int tool_index)
    {
        PSNS::Timer::GetTimerData(timer_data, tool_index);
    }

    void psGetCounterData(perftool_counter_data_t *counter_data, int tool_index)
    {
        PSNS::Timer::GetCounterData(counter_data, tool_index);
    }

    void psGetMetaData(perftool_metadata_t *metadata, int tool_index)
    {
        PSNS::Timer::GetMetaData(metadata, tool_index);
    }

    void psFreeTimerData(perftool_timer_data_t *timer_data, int tool_index)
    {
        PSNS::Timer::FreeTimerData(timer_data, tool_index);
    }

    void psFreeCounterData(perftool_counter_data_t *counter_data, int tool_index)
    {
        PSNS::Timer::FreeCounterData(counter_data, tool_index);
    }

    void psFreeMetaData(perftool_metadata_t *metadata, int tool_index)
    {
        PSNS::Timer::FreeMetaData(metadata, tool_index);
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

