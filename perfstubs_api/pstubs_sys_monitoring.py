#!/usr/bin/env python3

"""Python instrumentation interface for PerfStubs"""

#__all__ = ["run", "runctx", "exitAllThreads", "help", "Profile"]

import os.path
import sys
import perfstubs
import pstubs_common as ps

def init_tracing():
    PROFILER_ID = sys.monitoring.PROFILER_ID
    sys.monitoring.use_tool_id(PROFILER_ID, "perfstubs")
    sys.monitoring.set_events(PROFILER_ID, sys.monitoring.events.PY_START | sys.monitoring.events.PY_RESUME | sys.monitoring.events.PY_RETURN | sys.monitoring.events.PY_YIELD | sys.monitoring.events.PY_UNWIND | sys.monitoring.events.PY_THROW )#| sys.monitoring.events.RAISE | sys.monitoring.events.STOP_ITERATION | sys.monitoring.events.EXCEPTION_HANDLED )
    perfstubs.initialize()

    # This is the actual PY_START event handler

    def pstubs_py_start(code, instruction_offset):
        frame = sys._getframe(1)
        #print("PY_START",code.co_name,code.co_filename, frame.f_lineno)
        rc = perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    def pstubs_py_resume(code, instruction_offset):
        frame = sys._getframe(1)
        #print("PY_RESUME",code.co_name,code.co_filename, frame.f_lineno)
        rc = perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    # This is the actual PY_STOP event handler

    def pstubs_py_return(code, instruction_offset, retval):
        frame = sys._getframe(1)
        #print("PY_STOP",code.co_name,code.co_filename, frame.f_lineno)
        rc = perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    def pstubs_py_yield(code, instruction_offset, retval):
        frame = sys._getframe(1)
        #print("PY_YIELD",code.co_name,code.co_filename, frame.f_lineno)
        rc = perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    # This is the PY_THROW / RAISE / PY_UNWIND event handler

    def pstubs_py_unwind(code, instruction_offset, exception):
        frame = sys._getframe(1)
        #print("PY_UNWIND",code.co_name,code.co_filename, frame.f_lineno)
        perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
        # Cannot local filter functions from this event, so just return.

    def pstubs_py_throw(code, instruction_offset, exception):
        frame = sys._getframe(1)
        #print("PY_THROW!",code.co_name,code.co_filename, frame.f_lineno)
        # Yes! Unintuitively, we want to START a timer on a throw. We are "returning" to the caller
        rc = perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    def pstubs_raise(code, instruction_offset, exception):
        #frame = sys._getframe(1)
        #print("RAISE!",code.co_name,code.co_filename, frame.f_lineno)
        pass

    def pstubs_py_stop_iteration(code, instruction_offset, exception):
        #frame = sys._getframe(1)
        #print("STOP_ITERATION!",code.co_name,code.co_filename, frame.f_lineno)
        pass

    def pstubs_py_exception_handled(code, instruction_offset, exception):
        #frame = sys._getframe(1)
        #print("EXCEPTION_HANDLED!",code.co_name,code.co_filename, frame.f_lineno)
        pass

    # This is the bootstrap PY_START event handler

    def pstubs_py_start_bootstrap(code, instruction_offset):
        # Do nothing...until we get an event we should be tracing
        if code.co_name in ps.bootstrap_timers:
            # trace it!
            frame = sys._getframe(1)
            rc = perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
            # time to enable the real tracing...
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_START,
                pstubs_py_start)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_RESUME,
                pstubs_py_resume)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_RETURN,
                pstubs_py_return)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_YIELD,
                pstubs_py_yield)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_UNWIND,
                pstubs_py_unwind)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_THROW,
                pstubs_py_throw)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.RAISE,
                pstubs_raise)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.STOP_ITERATION,
                pstubs_py_stop_iteration)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.EXCEPTION_HANDLED,
                pstubs_py_exception_handled)
            # Not sure about these two events yet...
            #sys.monitoring.register_callback(
            #    PROFILER_ID,
            #    sys.monitoring.events.CALL,
            #    pstubs_call_trace)
            #sys.monitoring.register_callback(
            #    PROFILER_ID,
            #    sys.monitoring.events.C_RETURN,
            #    pstubs_c_return_trace)


    def pstubs_py_return_bootstrap(code, instruction_offset, retval):
        # do nothing...yet
        return

    def pstubs_call_trace(code, instruction_offset, callable, arg0):
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    def pstubs_c_return_trace(code, instruction_offset, callable, arg0):
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)

    # Initially, we want to call the bootstrap callbacks, until we see
    # an event that we want to start tracing - to avoid the 18k timers
    # we would get during the runtime startup.
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_START,
        pstubs_py_start_bootstrap)

    # Is this even necessary if we don't trace until after bootstrap?
    #sys.monitoring.register_callback(
    #    PROFILER_ID,
    #    sys.monitoring.events.PY_RETURN,
    #    pstubs_py_return_bootstrap)

def fini_tracing():
    PROFILER_ID = sys.monitoring.PROFILER_ID
    sys.monitoring.set_events(PROFILER_ID, 0)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_START,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_RESUME,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_RETURN,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_YIELD,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_UNWIND,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_THROW,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.RAISE,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.STOP_ITERATION,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.EXCEPTION_HANDLED,
        None)
    sys.monitoring.free_tool_id(PROFILER_ID)
    perfstubs.finalize()

