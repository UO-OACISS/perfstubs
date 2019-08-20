! hello.f
! --------
! This file contains code for testing the Fortran interface to TAU
! It works with the Cray T3E F90 compiler with TAU.
!-----------------------------------------------------------------------------

      subroutine HELLOWORLD(iVal)
        integer iVal

        call PSTIMERSTART('HELLOWORLD')
        print *, "Iteration = ", iVal
        call PSTIMERSTOP('HELLOWORLD')
      end

      program main
        integer i

        call PSINIT()
        call PSTIMERSTART('main')

        print *, "test program"

        do 10, i = 1, 10
        call HELLOWORLD(i)
10      continue
        call PSTIMERSTOP('main')
      end

