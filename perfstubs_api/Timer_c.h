// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#pragma once

#include <stdint.h>
#include "perfstubs_api/Config.h"
#include "perfstubs_api/Tool.h"

/* These macros help generate unique variable names */
#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)

/* ------------------------------------------------------------------ */
/* Define the C API first */
/* ------------------------------------------------------------------ */

#ifdef __GNUC__
#define __PERFSTUBS_FUNCTION__ __PRETTY_FUNCTION__
#else
#define __PERFSTUBS_FUNCTION__ __func__
#endif

extern int perfstubs_initialized;

#include <string.h>
#include <stdio.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Now define the C API */
/* ------------------------------------------------------------------ */

#if defined(PERFSTUBS_USE_TIMERS)

/* regular C API */

void psInit(void);
void psRegisterThread(void);
void psDumpData(void);
void* psTimerCreate(const char *timerName);
void psTimerStart(const void *timer);
void psTimerStop(const void *timer);
void psSetParameter(const char *parameter_name, int64_t parameter_value);
void psDynamicPhaseStart(const char *phasePrefix, int iterationIndex);
void psDynamicPhaseStop(const char *phasePrefix, int iterationIndex);
void* psCreateCounter(const char *name);
void psSampleCounter(const void *counter, const double value);
void psMetaData(const char *name, const char *value);

/* data query API */

void psGetTimerData(perftool_timer_data_t *timer_data);
void psGetCounterData(perftool_counter_data_t *counter_data);
void psGetMetaData(perftool_metadata_t *metadata);
void psFreeTimerData(perftool_timer_data_t *timer_data);
void psFreeCounterData(perftool_counter_data_t *counter_data);
void psFreeMetaData(perftool_metadata_t *metadata);

char * psMakeTimerName(const char * file, const char * func, int line);

/* Macro API for option of entirely disabling at compile time
 * To use this API, set the Macro PERFSTUBS_USE_TIMERS on the command
 * line or in a config.h file, however your project does it
 */

#define PERFSTUBS_INIT() psInit();

#define PERFSTUBS_DUMP_DATA() psDumpData();

#define PERFSTUBS_REGISTER_THREAD() psRegisterThread();

#define PERFSTUBS_TIMER_START(_timer, _timer_name) \
    static void * _timer = NULL; \
    if (perfstubs_initialized == 1) { \
        if (_timer == NULL) { \
            _timer = psTimerCreate(_timer_name); \
        } \
        psTimerStart(_timer); \
    };

#define PERFSTUBS_TIMER_STOP(_timer) \
    if (perfstubs_initialized == 1) psTimerStop(_timer); \

#define PERFSTUBS_SET_PARAMETER(_parameter, _value) \
    if (perfstubs_initialized == 1) psSetParameter(_parameter, _value);

#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index) \
    if (perfstubs_initialized == 1) \
    psDynamicPhaseStart(_phase_prefix, _iteration_index);

#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index) \
    if (perfstubs_initialized == 1) \
    psDynamicPhaseStop(_phase_prefix, _iteration_index);

#define PERFSTUBS_TIMER_START_FUNC(_timer) \
    static void * _timer = NULL; \
    if (perfstubs_initialized == 1) { \
        if (_timer == NULL) { \
            char * tmpstr = psMakeTimerName(__FILE__, \
            __PERFSTUBS_FUNCTION__, __LINE__); \
            _timer = psTimerCreate(tmpstr); \
            free(tmpstr); \
        } \
        psTimerStart(_timer); \
    };

#define PERFSTUBS_TIMER_STOP_FUNC(_timer) \
    if (perfstubs_initialized == 1) psTimerStop(_timer);

#define PERFSTUBS_SAMPLE_COUNTER(_name, _value) \
    static void * CONCAT(__var,__LINE__) =  NULL; \
    if (perfstubs_initialized == 1) { \
        if (CONCAT(__var,__LINE__) == NULL) { \
            CONCAT(__var,__LINE__) = psCreateCounter(_name); \
        } \
        psSampleCounter(CONCAT(__var,__LINE__), _value); \
    };

#define PERFSTUBS_METADATA(_name, _value) \
    if (perfstubs_initialized == 1) psMetaData(_name, _value);

#else // defined(PERFSTUBS_USE_TIMERS)

#define PERFSTUBS_INIT()
#define PERFSTUBS_DUMP_DATA()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START(_timer, _timer_name)
#define PERFSTUBS_TIMER_STOP(_timer_name)
#define PERFSTUBS_SET_PARAMETER(_parameter, _value)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)
#define PERFSTUBS_TIMER_START_FUNC(_timer)
#define PERFSTUBS_TIMER_STOP_FUNC(_timer)
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)

#endif // defined(PERFSTUBS_USE_TIMERS)

