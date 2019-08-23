// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#pragma once

#include "perfstubs_api/Config.h"
#include "perfstubs_api/Tool.h"

/* ------------------------------------------------------------------ */
/* Define the C++ API and PerfStubs glue class first */
/* ------------------------------------------------------------------ */

#ifdef __cplusplus

#if defined(PERFSTUBS_USE_TIMERS)

/*
 * We allow the namespace to be changed, so that different libraries
 * can include their own implementation and not have a namespace collision.
 * For example, library A and executable B could both include the 
 * perfstubs_api code in their source tree, and change the namespace
 * respectively, instead of linking in the perfstubs library.
 */

#if defined(PERFSTUBS_NAMESPACE)
#define PERFSTUBS_INTERNAL_NAMESPACE PERFSTUBS_NAMESPACE
#else
#define PERFSTUBS_INTERNAL_NAMESPACE perfstubs_profiler
#endif

#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>

namespace external
{

namespace PERFSTUBS_INTERNAL_NAMESPACE
{

class Timer
{
public:
    static void RegisterThread(void);
    static void DumpData(void);
    // data measurement api
    static void Start(const char *timer_name);
    static void Start(const std::string &timer_name);
    static void DynamicPhaseStart(const char *phase_prefix,
                                  int iteration_index);
    static void DynamicPhaseStart(const std::string &phase_prefix,
                                  int iteration_index);
    static void Stop(const char *timer_name);
    static void Stop(const std::string &timer_name);
    static void DynamicPhaseStop(const char *phase_prefix, int iteration_index);
    static void DynamicPhaseStop(const std::string &phase_prefix,
                                 int iteration_index);
    static void SampleCounter(const char *name, const double value);
    static void MetaData(const char *name, const char *value);

    // data query api
    // data query function declarations
    static void GetTimerData(perftool_timer_data_t *timer_data);
    static void GetCounterData(perftool_counter_data_t *counter_data);
    static void GetMetaData(perftool_metadata_t *metadata);
    static void FreeTimerData(perftool_timer_data_t *timer_data);
    static void FreeCounterData(perftool_counter_data_t *counter_data);
    static void FreeMetaData(perftool_metadata_t *metadata);

    // The only way to get an instance of this class
    static Timer &Get(void);
    // destructor
    ~Timer(void);
    bool m_Initialized;
    static thread_local bool m_ThreadSeen;

private:
    static void _RegisterThread(void);
    // Private constructor to prevent construction
    Timer(void);
    // Prevent copies
    Timer(const Timer &old);
    const Timer &operator=(const Timer &old);
};

class ScopedTimer
{
private:
    std::string m_Name;

public:
    ScopedTimer(const std::string &name) : m_Name(name)
    {
        Timer::Start(m_Name);
    }
    ~ScopedTimer() { Timer::Stop(m_Name); }
};

} // namespace PERFSTUBS_INTERNAL_NAMESPACE

} // namespace external

#ifdef __GNUC__
#define __PERFSTUBS_FUNCTION__ __PRETTY_FUNCTION__
#else
#define __PERFSTUBS_FUNCTION__ __func__
#endif

namespace PSNS = external::PERFSTUBS_INTERNAL_NAMESPACE;

#define PERFSTUBS_INIT() PSNS::Timer::Get();
#define PERFSTUBS_DUMP_DATA() PSNS::Timer::DumpData();
#define PERFSTUBS_REGISTER_THREAD() PSNS::Timer::RegisterThread();
#define PERFSTUBS_TIMER_START_STRING(_timer_name) \
    PSNS::Timer::Start(_timer_name);
#define PERFSTUBS_TIMER_STOP_STRING(_timer_name) \
    PSNS::Timer::Stop(_timer_name);
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index) \
    PSNS::Timer::DynamicPhaseStart( _phase_prefix, _iteration_index);
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index) \
    PSNS::Timer::DynamicPhaseStop( _phase_prefix, _iteration_index);
#define PERFSTUBS_TIMER_START_FUNC() \
    std::stringstream __perfstubsFuncNameSS; \
    __perfstubsFuncNameSS << __PERFSTUBS_FUNCTION__ \
        << " [{" << __FILE__ << "} {" << __LINE__ << ",0}]"; \
    std::string __perfStubsFuncName(__perfStubsFuncNameSS); \
    PSNS::Timer::Start(__perfstubsFuncName);
#define PERFSTUBS_TIMER_STOP_FUNC() PSNS::Timer::Stop(__perfstubsFuncName);
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value) \
    PSNS::Timer::SampleCounter(_name, _value);
#define PERFSTUBS_METADATA(_name, _value) PSNS::Timer::MetaData(_name, _value);
#define PERFSTUBS_SCOPED_TIMER(__name) PSNS::ScopedTimer __var##finfo(__name);

#define PERFSTUBS_SCOPED_TIMER_FUNC() \
    std::stringstream __ss##finfo; \
    __ss##finfo << __PERFSTUBS_FUNCTION__ << " [{" << __FILE__ \
        << "} {" << __LINE__ << ",0}]"; \
    PSNS::ScopedTimer \
        __var##finfo(__ss##finfo.str());

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_DUMP_DATA()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START_STRING(_timer_name)
#define PERFSTUBS_TIMER_STOP_STRING(_timer_name)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, iteration_index)
#define PERFSTUBS_TIMER_START_FUNC()
#define PERFSTUBS_TIMER_STOP_FUNC()
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)
#define PERFSTUBS_SCOPED_TIMER(__name)
#define PERFSTUBS_SCOPED_TIMER_FUNC()

#endif // defined(PERFSTUBS_USE_TIMERS)

#else // ifdef __cplusplus

/* ------------------------------------------------------------------ */
/* Now define the C API */
/* ------------------------------------------------------------------ */

#if defined(PERFSTUBS_USE_TIMERS)

/* regular C API */

void psInit(void);
void psRegisterThread(void);
void psDumpData(void);
void psTimerStartString(const char *timerName);
void psTimerStopString(const char *timerName);
void psDynamicPhaseStart(const char *phasePrefix, int iterationIndex);
void psDynamicPhaseStop(const char *phasePrefix, int iterationIndex);
void psSampleCounter(const char *name, const double value);
void psMetaData(const char *name, const char *value);

/* data query API */

void psGetTimerData(perftool_timer_data_t *timer_data);
void psGetCounterData(perftool_counter_data_t *counter_data);
void psGetMetaData(perftool_metadata_t *metadata);
void psFreeTimerData(perftool_timer_data_t *timer_data);
void psFreeCounterData(perftool_counter_data_t *counter_data);
void psFreeMetaData(perftool_metadata_t *metadata);

/* Macro API for option of entirely disabling at compile time
 * To use this API, set the Macro PERFSTUBS_USE_TIMERS on the command
 * line or in a config.h file, however your project does it
 */

#define PERFSTUBS_INIT() psInit();
#define PERFSTUBS_DUMP_DATA() psDumpData();
#define PERFSTUBS_REGISTER_THREAD() psRegisterThread();
#define PERFSTUBS_TIMER_START_STRING(_timer_name) \
    psTimerStartString(_timer_name);
#define PERFSTUBS_TIMER_STOP_STRING(_timer_name) \
    psTimerStopString(_timer_name);
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index) \
    psDynamicPhaseStart(_phase_prefix, _iteration_index);
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index) \
    psDynamicPhaseStop(_phase_prefix, _iteration_index);
#define PERFSTUBS_TIMER_START_FUNC() \
    char __perfstubsFuncName[1024]; \
    sprintf(__perfstubsFuncName, "%s [{%s} {%d,0}]", __func__, __FILE__, \
            __LINE__); \
    psTimerStartString(__perfstubsFuncName);
#define PERFSTUBS_TIMER_STOP_FUNC() psTimerStopString(__perfstubsFuncName);
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value) psSampleCounter(_name, _value);
#define PERFSTUBS_METADATA(_name, _value) psMetaData(_name, _value);

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_DUMP_DATA()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START_STRING(_timer_name)
#define PERFSTUBS_TIMER_STOP_STRING(_timer_name)
#define PERFSTUBS_DYNAMIC_PHASE_START_STRING(_phase_prefix, _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP_STRING(_phase_prefix, _iteration_index)
#define PERFSTUBS_TIMER_START_FUNC()
#define PERFSTUBS_TIMER_STOP_FUNC()
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)

#endif // defined(PERFSTUBS_USE_TIMERS)

#endif // ifdef __cplusplus
