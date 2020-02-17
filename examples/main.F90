! hello.f
! --------
! This file contains code for testing the Fortran interface to TAU
! It works with the Cray T3E F90 compiler with TAU.
!-----------------------------------------------------------------------------

#define PERFSTUBS_USE_TIMERS 1
#include "perfstubs_api/timer_f.h"

      subroutine HELLOWORLD(iVal)
        integer iVal
        !integer profiler(2) / 0, 0 /
        integer profiler(2)
        save    profiler

        PERFSTUBS_TIMER_CREATE(profiler, 'HELLOWORLD')
        PERFSTUBS_TIMER_START(profiler)
        print *, "Iteration = ", iVal
        PERFSTUBS_TIMER_STOP(profiler)
      end

      subroutine HELLOWORLD2(iVal)
        integer iVal

        PERFSTUBS_START_STRING('HELLOWORLD2')
        print *, "Iteration = ", iVal
        PERFSTUBS_STOP_CURRENT()
      end

      subroutine HELLOWORLD3(iVal)
        integer iVal

        PERFSTUBS_START_STRING('HELLOWORLD3')
        print *, "Iteration = ", iVal
        PERFSTUBS_STOP_STRING('HELLOWORLD3')
      end

      program main
        integer i
        integer profiler(2)
        save    profiler
        integer counter(2)
        save    counter

        PERFSTUBS_INITIALIZE()
        PERFSTUBS_TIMER_CREATE(profiler, 'main')
        PERFSTUBS_TIMER_START(profiler)
        PERFSTUBS_CREATE_COUNTER(counter, 'test counter')
        PERFSTUBS_SAMPLE_COUNTER(counter, 25.0)
        PERFSTUBS_METADATA('foo', 'bar')

        print *, "test program"

        do 10, i = 1, 10
            call HELLOWORLD(i)
            call HELLOWORLD2(i)
            call HELLOWORLD3(i)
10      continue
        PERFSTUBS_TIMER_STOP(profiler)
        PERFSTUBS_DUMP_DATA()
        PERFSTUBS_FINALIZE()
      end

