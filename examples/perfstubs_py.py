#!/usr/bin/env python3

import os.path
import sys
import perfstubs

def init_tracing():
    DEBUGGER_ID = sys.monitoring.DEBUGGER_ID
    # example filter
    filter_out = ["_verbose_message","fini_tracing","init_tracing","<module>"]
    sys.monitoring.use_tool_id(DEBUGGER_ID, "perfstubs")
    sys.monitoring.set_events(DEBUGGER_ID, sys.monitoring.events.PY_START | sys.monitoring.events.PY_RETURN )
    perfstubs.initialize()
    def pep_669_py_start_trace(code, instruction_offset):
        # If we should filter this event, do it
        if code.co_name in filter_out:
            return sys.monitoring.DISABLE
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    def pep_669_py_stop_trace(code, instruction_offset, retval):
        # If we should filter this event, do it
        if code.co_name in filter_out:
            return sys.monitoring.DISABLE
        frame = sys._getframe(1)
        perfstubs.stop(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    def pep_669_call_trace(code, instruction_offset, callable, arg0):
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    def pep_669_c_return_trace(code, instruction_offset, callable, arg0):
        frame = sys._getframe(1)
        perfstubs.start(code.co_name, os.path.basename(code.co_filename), frame.f_lineno)
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_START,
        pep_669_py_start_trace)
    sys.monitoring.register_callback(
        DEBUGGER_ID,
        sys.monitoring.events.PY_RETURN,
        pep_669_py_stop_trace)

    # Not sure about these two events yet...
    #sys.monitoring.register_callback(
    #    DEBUGGER_ID,
    #    sys.monitoring.events.CALL,
    #    pep_669_call_trace)
    #sys.monitoring.register_callback(
    #    DEBUGGER_ID,
    #    sys.monitoring.events.C_RETURN,
    #    pep_669_c_return_trace)

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

def firstPrimeAfter(x):
	import math
	current = x + 1
	sqrtno = math.sqrt(current)
	while True:
		#search for primes starting at x until it finds one
		#break once found a prime
		for potentialfactor in range(2,current):
			# start at 2 because 1 will always be a factor
			# go all the way up to the sqrt of current looking for a factor
			if current % potentialfactor == 0:
				# Found factor. not prime
				break # move on to next number
			elif potentialfactor >= sqrtno:
				print("The first prime number after {} is {}".format(x,current))
				return current
		current += 1

if __name__ == '__main__':
    init_tracing()
    firstPrimeAfter(10000000)
    fini_tracing()

