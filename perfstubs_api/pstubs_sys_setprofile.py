#!/usr/bin/env python3

"""Python instrumentation interface for PerfStubs"""

__all__ = ["run", "runctx", "exitAllThreads", "help", "Profile"]

import os
import sys
import perfstubs
import pstubs_common as ps

def get_python_version():
    print("pstubs_sys_setprofile get_python_version")
    return perfstubs.get_python_version()

def my_tracer(frame, event, arg = None):
    code = frame.f_code
    if code.co_name in ps.exclude_timers:
        return my_tracer
    if not ps.internal_timers and (ps.python_system_path in code.co_filename or ps.python_frozen_path in code.co_filename):
        return my_tracer

    if event == 'call':
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
        #return my_tracer
    elif event == 'return':
        frame = sys._getframe(1)
        perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
    return my_tracer

def init_tracing():
    print("pstubs_sys_setprofile init_tracing")
    perfstubs.initialize()
    sys.setprofile(my_tracer)

def fini_tracing():
    print("pstubs_sys_setprofile fini_tracing")
    perfstubs.finalize()

