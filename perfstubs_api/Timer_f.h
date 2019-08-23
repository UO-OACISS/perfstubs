! Copyright (c) 2019 University of Oregon
! Distributed under the BSD Software License
! (See accompanying file LICENSE.txt)

#ifdef PERFSTUBS_USE_TIMERS

!/*
!    Macro API for option of entirely disabling at compile time
!    To use this API, set the Macro PERFSTUBS_USE_TIMERS on the command
!    line or in a config.h file, however your project does it
! */

#define PERFSTUBS_INIT() call psinit()
#define PERFSTUBS_DUMP_DATA() call psdumpdata()
#define PERFSTUBS_REGISTER_THREAD() call psregisterthread()
#define PERFSTUBS_TIMER_START(_timer_name) \
    call pstimerstart(_timer_name//CHAR(0))
#define PERFSTUBS_TIMER_STOP(_timer_name) \
    call pstimerstop(_timer_name//CHAR(0))
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index) \
    call psdynamicphasestart(_phase_prefix//CHAR(0), _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index) \
    call psdynamicphasestop(_phase_prefix//CHAR(0), _iteration_index)
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value) \
    call pssamplecounter(_name//CHAR(0), _value)
#define PERFSTUBS_METADATA(_name, _value) \
    call psmetadata(_name//CHAR(0), _value)

! // defined(PERFSTUBS_USE_TIMERS)
#else

#define PERFSTUBS_INIT()
#define PERFSTUBS_DUMP_DATA()
#define PERFSTUBS_REGISTER_THREAD()
#define PERFSTUBS_TIMER_START(_timer_name)
#define PERFSTUBS_TIMER_STOP(_timer_name)
#define PERFSTUBS_DYNAMIC_PHASE_START(_phase_prefix, _iteration_index)
#define PERFSTUBS_DYNAMIC_PHASE_STOP(_phase_prefix, _iteration_index)
#define PERFSTUBS_TIMER_START_FUNC()
#define PERFSTUBS_TIMER_STOP_FUNC()
#define PERFSTUBS_SAMPLE_COUNTER(_name, _value)
#define PERFSTUBS_METADATA(_name, _value)

! // defined(PERFSTUBS_USE_TIMERS)
#endif

