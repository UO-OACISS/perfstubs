#!/usr/bin/env python3

"""Python instrumentation interface for PerfStubs"""

#__all__ = ["run", "runctx", "exitAllThreads", "help", "Profile"]

import os.path
import sys
import perfstubs
import pstubs_common as ps

def init_tracing():
    DEBUGGER_ID = sys.monitoring.DEBUGGER_ID
    sys.monitoring.use_tool_id(DEBUGGER_ID, "perfstubs")
    sys.monitoring.set_events(DEBUGGER_ID, sys.monitoring.events.PY_START | sys.monitoring.events.PY_RETURN )
    perfstubs.initialize()

    # This is the actual PY_START event handler

    def pstubs_py_start_trace(code, instruction_offset):
        # If we should filter this event, do it
        if code.co_name in ps.exclude_timers:
            return sys.monitoring.DISABLE
        if not ps.internal_timers and (ps.python_system_path in code.co_filename or ps.python_frozen_path in code.co_filename):
            return sys.monitoring.DISABLE
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)

    # This is the actual PY_STOP event handler

    def pstubs_py_stop_trace(code, instruction_offset, retval):
        # If we should filter this event, do it
        if code.co_name in ps.exclude_timers:
            return sys.monitoring.DISABLE
        if not ps.internal_timers and (ps.python_system_path in code.co_filename or ps.python_frozen_path in code.co_filename):
            return sys.monitoring.DISABLE
        frame = sys._getframe(1)
        perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)

    # This is the bootstrap PY_START event handler

    def pstubs_py_start_trace_bootstrap(code, instruction_offset):
        # Do nothing...until we get an event we should be tracing
        if code.co_name in ps.bootstrap_timers:
            if ps.internal_timers:
                # trace it!
                frame = sys._getframe(1)
                perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
            # time to enable the real tracing...
            sys.monitoring.register_callback(
                DEBUGGER_ID,
                sys.monitoring.events.PY_START,
                pstubs_py_start_trace)
            sys.monitoring.register_callback(
                DEBUGGER_ID,
                sys.monitoring.events.PY_RETURN,
                pstubs_py_stop_trace)

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
        DEBUGGER_ID,
        sys.monitoring.events.PY_START,
        pstubs_py_start_trace_bootstrap)

    # Is this even necessary if we don't trace until after bootstrap?
    #sys.monitoring.register_callback(
    #    DEBUGGER_ID,
    #    sys.monitoring.events.PY_RETURN,
    #    pstubs_py_stop_trace_bootstrap)

    # Not sure about these two events yet...
    # sys.monitoring.register_callback(
    #     DEBUGGER_ID,
    #     sys.monitoring.events.CALL,
    #     pstubs_call_trace)
    # sys.monitoring.register_callback(
    #     DEBUGGER_ID,
    #     sys.monitoring.events.C_RETURN,
    #     pstubs_c_return_trace)

def fini_tracing():
    DEBUGGER_ID = sys.monitoring.DEBUGGER_ID
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_START,
        None)
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_RETURN,
        None)
    perfstubs.finalize()

