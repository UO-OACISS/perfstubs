! hello.f
! --------
! This file contains code for testing the Fortran interface to TAU
! It works with the Cray T3E F90 compiler with TAU.
!-----------------------------------------------------------------------------

#define PERFSTUBS_USE_TIMERS 1
#include "perfstubs_api/Timer_f.h"

      subroutine HELLOWORLD(iVal)
        integer iVal

        PERFSTUBS_TIMER_START('HELLOWORLD')
        print *, "Iteration = ", iVal
        PERFSTUBS_TIMER_STOP('HELLOWORLD')
      end

      program main
        integer i

        PERFSTUBS_INIT()
        PERFSTUBS_TIMER_START('main')

        print *, "test program"

        do 10, i = 1, 10
        call HELLOWORLD(i)
10      continue
        PERFSTUBS_TIMER_START('main')
      end

