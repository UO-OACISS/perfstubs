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
    sys.monitoring.set_events(PROFILER_ID, sys.monitoring.events.PY_START | sys.monitoring.events.PY_RETURN )
    perfstubs.initialize()

    # This is the actual PY_START event handler

    def pstubs_py_start_trace(code, instruction_offset):
        frame = sys._getframe(1)
        rc = perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    # This is the actual PY_STOP event handler

    def pstubs_py_stop_trace(code, instruction_offset, retval):
        frame = sys._getframe(1)
        rc = perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
        # If we should filter this event, do it
        if (rc == False):
            return sys.monitoring.DISABLE

    # This is the bootstrap PY_START event handler

    def pstubs_py_start_trace_bootstrap(code, instruction_offset):
        # Do nothing...until we get an event we should be tracing
        if code.co_name in ps.bootstrap_timers:
            # trace it!
            frame = sys._getframe(1)
            rc = perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
            # time to enable the real tracing...
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_START,
                pstubs_py_start_trace)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.PY_RETURN,
                pstubs_py_stop_trace)
            # Not sure about these two events yet...
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.CALL,
                pstubs_call_trace)
            sys.monitoring.register_callback(
                PROFILER_ID,
                sys.monitoring.events.C_RETURN,
                pstubs_c_return_trace)


    def pstubs_py_stop_trace_bootstrap(code, instruction_offset, retval):
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
        pstubs_py_start_trace_bootstrap)

    # Is this even necessary if we don't trace until after bootstrap?
    #sys.monitoring.register_callback(
    #    PROFILER_ID,
    #    sys.monitoring.events.PY_RETURN,
    #    pstubs_py_stop_trace_bootstrap)

def fini_tracing():
    PROFILER_ID = sys.monitoring.PROFILER_ID
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_START,
        None)
    sys.monitoring.register_callback(
        PROFILER_ID,
        sys.monitoring.events.PY_RETURN,
        None)
    perfstubs.finalize()

