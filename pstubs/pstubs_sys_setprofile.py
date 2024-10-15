#!/usr/bin/env python3

"""Python instrumentation interface for PerfStubs"""

__all__ = ["run", "runctx", "exitAllThreads", "help", "Profile"]

import os
import sys
import perfstubs
from pstubs import pstubs_common as ps

def get_python_version():
    return perfstubs.get_python_version()

def my_tracer(frame, event, arg = None):
    code = frame.f_code
    frame = sys._getframe(1)
    if event == 'call':
        perfstubs.start(code.co_name, code.co_filename, frame.f_lineno)
    elif event == 'return':
        perfstubs.stop(code.co_name, code.co_filename, frame.f_lineno)
    return my_tracer

def init_tracing():
    perfstubs.initialize()
    sys.setprofile(my_tracer)

def fini_tracing():
    perfstubs.finalize()

