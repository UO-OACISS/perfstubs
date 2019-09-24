// Copyright (c) 2019 University of Oregon
// Distributed under the BSD Software License
// (See accompanying file LICENSE.txt)

#define PERFSTUBS_USE_TIMERS
#include "perfstubs_api/Timer_c.h"

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

#define PERFSTUBS_UNKNOWN 0
#define PERFSTUBS_SUCCESS 1
#define PERFSTUBS_FAILURE 2

/* Make sure that the Timer singleton is constructed when the
 * library is loaded.  This will ensure (on linux, anyway) that
 * we can assert that we have m_Initialized on the main thread. */
// static void __attribute__((constructor)) InitializeLibrary(void);

int perfstubs_initialized = PERFSTUBS_UNKNOWN;
__thread int m_ThreadSeen = 0;

/* Function pointer types */

/* Logistical functions */
typedef void PerfStubsInitType(void);
typedef void PerfStubsRegisterThreadType(void);
typedef void PerfStubsExitType(void);
typedef void PerfStubsDumpDataType(void);
/* Data entry functions */
typedef void* PerfStubsTimerCreateType(const char *);
typedef void PerfStubsTimerStartType(const void *);
typedef void PerfStubsTimerStopType(const void *);
typedef void PerfStubsSetParameterType(const char *, int64_t);
typedef void PerfStubsDynamicPhaseStartType(const char *, int);
typedef void PerfStubsDynamicPhaseStopType(const char *, int);
typedef void* PerfStubsCreateCounterType(const char *);
typedef void PerfStubsSampleCounterType(const void *, double);
typedef void PerfStubsMetaDataType(const char *, const char *);
/* Data Query Functions */
typedef void PerfStubsGetTimerDataType(perftool_timer_data_t *);
typedef void PerfStubsGetCounterDataType(perftool_counter_data_t *);
typedef void PerfStubsGetMetaDataType(perftool_metadata_t *);
typedef void PerfStubsFreeTimerDataType(perftool_timer_data_t *);
typedef void PerfStubsFreeCounterDataType(perftool_counter_data_t *);
typedef void PerfStubsFreeMetaDataType(perftool_metadata_t *);

/* Function pointers */

PerfStubsInitType *MyPerfStubsInit = NULL;
PerfStubsRegisterThreadType *MyPerfStubsRegisterThread = NULL;
PerfStubsExitType *MyPerfStubsExit = NULL;
PerfStubsDumpDataType *MyPerfStubsDumpData = NULL;
PerfStubsTimerCreateType *MyPerfStubsTimerCreate = NULL;
PerfStubsTimerStartType *MyPerfStubsTimerStart = NULL;
PerfStubsTimerStopType *MyPerfStubsTimerStop = NULL;
PerfStubsSetParameterType *MyPerfStubsSetParameter = NULL;
PerfStubsDynamicPhaseStartType *MyPerfStubsDynamicPhaseStart = NULL;
PerfStubsDynamicPhaseStopType *MyPerfStubsDynamicPhaseStop = NULL;
PerfStubsCreateCounterType *MyPerfStubsCreateCounter = NULL;
PerfStubsSampleCounterType *MyPerfStubsSampleCounter = NULL;
PerfStubsMetaDataType *MyPerfStubsMetaData = NULL;
PerfStubsGetTimerDataType *MyPerfStubsGetTimerData = NULL;
PerfStubsGetCounterDataType *MyPerfStubsGetCounterData = NULL;
PerfStubsGetMetaDataType *MyPerfStubsGetMetaData = NULL;
PerfStubsFreeTimerDataType *MyPerfStubsFreeTimerData = NULL;
PerfStubsFreeCounterDataType *MyPerfStubsFreeCounterData = NULL;
PerfStubsFreeMetaDataType *MyPerfStubsFreeMetaData = NULL;

#define UNUSED(expr) do { (void)(expr); } while (0);

#ifdef PERFSTUBS_USE_STATIC

#if defined(__clang__) && defined(__APPLE__)
#define PS_WEAK_PRE extern
#define PS_WEAK_POST __attribute__((weak_import))
#define PS_WEAK_POST_NULL __attribute__((weak_import))
#else
#define PS_WEAK_PRE extern __attribute__((weak))
#define PS_WEAK_POST
#define PS_WEAK_POST_NULL
#endif

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
PS_WEAK_PRE void perftool_get_counter_data(perftool_counter_data_t *) PS_WEAK_POST;
PS_WEAK_PRE void perftool_get_metadata(perftool_metadata_t *) PS_WEAK_POST;
PS_WEAK_PRE void perftool_free_timer_data(perftool_timer_data_t *) PS_WEAK_POST;
PS_WEAK_PRE void perftool_free_counter_data(perftool_counter_data_t *) PS_WEAK_POST;
PS_WEAK_PRE void perftool_free_metadata(perftool_metadata_t *) PS_WEAK_POST;
#endif

int AssignFunctionPointers(void)
{
#ifdef PERFSTUBS_USE_STATIC
    /* The initialization function is the only required one */
    MyPerfStubsInit = &perftool_init;
    if (MyPerfStubsInit == NULL)
    {
        fprintf(stderr, "perftool_init not defined, profiling not initialized.\n");
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsRegisterThread = &perftool_register_thread;
    MyPerfStubsExit = &perftool_exit;
    MyPerfStubsDumpData = &perftool_dump_data;
    MyPerfStubsTimerCreate = &perftool_timer_create;
    MyPerfStubsTimerStart = &perftool_timer_start;
    MyPerfStubsTimerStop = &perftool_timer_stop;
    MyPerfStubsSetParameter = &perftool_set_parameter;
    MyPerfStubsDynamicPhaseStart = &perftool_dynamic_phase_start;
    MyPerfStubsDynamicPhaseStop = &perftool_dynamic_phase_stop;
    MyPerfStubsCreateCounter = &perftool_create_counter;
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
    if (MyPerfStubsInit == NULL)
    {
        return PERFSTUBS_FAILURE;
    }
    MyPerfStubsRegisterThread = (PerfStubsRegisterThreadType *)dlsym(
        RTLD_DEFAULT, "perftool_register_thread");
    MyPerfStubsDumpData =
        (PerfStubsDumpDataType *)dlsym(RTLD_DEFAULT, "perftool_dump_data");
    MyPerfStubsTimerCreate =
        (PerfStubsTimerCreateType *)dlsym(RTLD_DEFAULT,
        "perftool_timer_create");
    MyPerfStubsTimerStart =
        (PerfStubsTimerStartType *)dlsym(RTLD_DEFAULT, "perftool_timer_start");
    MyPerfStubsTimerStop =
        (PerfStubsTimerStopType *)dlsym(RTLD_DEFAULT, "perftool_timer_stop");
    MyPerfStubsSetParameter =
        (PerfStubsSetParameterType *)dlsym(RTLD_DEFAULT, "perftool_set_parameter");
    MyPerfStubsDynamicPhaseStart = (PerfStubsDynamicPhaseStartType *)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_start");
    MyPerfStubsDynamicPhaseStop = (PerfStubsDynamicPhaseStopType *)dlsym(
        RTLD_DEFAULT, "perftool_dynamic_phase_stop");
    MyPerfStubsCreateCounter = (PerfStubsCreateCounterType *)dlsym(
        RTLD_DEFAULT, "perftool_create_counter");
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
        fprintf(stderr, "ERROR: Unable to initialize the perftool API\n");
#endif
        return PERFSTUBS_FAILURE;
    }
    /* Initialize the external tool */
    MyPerfStubsInit();
    return PERFSTUBS_SUCCESS;
}

// used internally to the class
inline void _RegisterThread(void)
{
    if (m_ThreadSeen == 0 && MyPerfStubsRegisterThread != NULL)
    {
        MyPerfStubsRegisterThread();
        m_ThreadSeen = 1;
    }
}

static void InitializeLibrary(void)
{
    // initialize the library
    if (PerfStubsStubInitializeSimple() == PERFSTUBS_SUCCESS)
    {
        // set the external flag
        perfstubs_initialized = PERFSTUBS_SUCCESS;
        _RegisterThread();
    }
    m_ThreadSeen = 1;
}

char * psMakeTimerName(const char * file, const char * func, int line) {
    // allocate enough length to hold the timer name
    int length = strlen(file) + strlen(func) + 32;
    char * tmpstr = (char*)(malloc(sizeof(char) * length));
    sprintf(tmpstr, "%s [{%s} {%d,0}]", func, file, line);
    return tmpstr;
}

#define CHECK_INITIALIZED \
if (perfstubs_initialized == PERFSTUBS_UNKNOWN) { \
    InitializeLibrary(); \
} else if (perfstubs_initialized == PERFSTUBS_FAILURE) { \
    return; \
} \
if (m_ThreadSeen == 0) _RegisterThread();

#define CHECK_INITIALIZED_NULL \
if (perfstubs_initialized == PERFSTUBS_UNKNOWN) { \
    InitializeLibrary(); \
} else if (perfstubs_initialized == PERFSTUBS_FAILURE) { \
    return NULL; \
} \
if (m_ThreadSeen == 0) _RegisterThread();

// external API calls

void psInit()
{
    CHECK_INITIALIZED
}

void psRegisterThread(void)
{
    CHECK_INITIALIZED
}

void* psTimerCreate(const char *timer_name)
{
    CHECK_INITIALIZED_NULL
    return (MyPerfStubsTimerCreate == NULL) ? NULL :
        MyPerfStubsTimerCreate(timer_name);
}

void psTimerStart(const void *timer)
{
    CHECK_INITIALIZED
    if (MyPerfStubsTimerStart != NULL)
        MyPerfStubsTimerStart(timer);
}

void psTimerStop(const void *timer)
{
    CHECK_INITIALIZED
    if (MyPerfStubsTimerStop != NULL)
        MyPerfStubsTimerStop(timer);
}

void psSetParameter(const char * parameter_name, int64_t parameter_value)
{
    CHECK_INITIALIZED
    if (MyPerfStubsSetParameter != NULL)
        MyPerfStubsSetParameter(parameter_name, parameter_value);
}

void psDynamicPhaseStart(const char *phase_prefix, int iteration_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsDynamicPhaseStart != NULL)
        MyPerfStubsDynamicPhaseStart(phase_prefix, iteration_index);
}

void psDynamicPhaseStop(const char *phase_prefix, int iteration_index)
{
    CHECK_INITIALIZED
    if (MyPerfStubsDynamicPhaseStop != NULL)
        MyPerfStubsDynamicPhaseStop(phase_prefix, iteration_index);
}

void* psCreateCounter(const char *name)
{
    CHECK_INITIALIZED_NULL
    return (MyPerfStubsCreateCounter == NULL) ? NULL :
        MyPerfStubsCreateCounter(name);
}

void psSampleCounter(const void *counter, const double value)
{
    CHECK_INITIALIZED
    if (MyPerfStubsSampleCounter != NULL)
        MyPerfStubsSampleCounter(counter, value);
}

void psMetaData(const char *name, const char *value)
{
    CHECK_INITIALIZED
    if (MyPerfStubsMetaData != NULL)
        MyPerfStubsMetaData(name, value);
}

void psDumpData(void)
{
    CHECK_INITIALIZED
    if (MyPerfStubsDumpData != NULL)
        MyPerfStubsDumpData();
}

void psFinalize(void)
{
    CHECK_INITIALIZED
    if (MyPerfStubsExit != NULL)
        MyPerfStubsExit();
}

void psGetTimerData(perftool_timer_data_t *timer_data)
{
    CHECK_INITIALIZED
    if (MyPerfStubsGetTimerData != NULL)
        MyPerfStubsGetTimerData(timer_data);
}

void psGetCounterData(perftool_counter_data_t *counter_data)
{
    CHECK_INITIALIZED
    if (MyPerfStubsGetCounterData != NULL)
        MyPerfStubsGetCounterData(counter_data);
}

void psGetMetaData(perftool_metadata_t *metadata)
{
    CHECK_INITIALIZED
    if (MyPerfStubsGetMetaData != NULL)
        MyPerfStubsGetMetaData(metadata);
}

void psFreeTimerData(perftool_timer_data_t *timer_data)
{
    CHECK_INITIALIZED
    if (MyPerfStubsFreeTimerData != NULL)
        MyPerfStubsFreeTimerData(timer_data);
}

void psFreeCounterData(perftool_counter_data_t *counter_data)
{
    CHECK_INITIALIZED
    if (MyPerfStubsFreeCounterData != NULL)
        MyPerfStubsFreeCounterData(counter_data);
}

void psFreeMetaData(perftool_metadata_t *metadata)
{
    CHECK_INITIALIZED
    if (MyPerfStubsFreeMetaData != NULL)
        MyPerfStubsFreeMetaData(metadata);
}

