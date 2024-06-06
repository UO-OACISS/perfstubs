#!/usr/bin/env python3

"""Python instrumentation interface for PerfStubs"""

__all__ = ["run", "runctx", "exitAllThreads", "help", "Profile"]

import os.path
import sys
import perfstubs

def init_tracing():
    DEBUGGER_ID = sys.monitoring.DEBUGGER_ID
    # example filter
    filter_out = ["fini_tracing","init_tracing","<module>"]
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

def run(statement):
    """Run statement under profiler"
    This function takes a single argument that can be passed to the
    "exec" statement, and an optional file name.  In all cases this
    routine attempts to "exec" its first argument and gather profiling
    statistics from the execution. If no file name is present, then this
    function automatically prints a simple profiling report, sorted by the
    standard name string (file/line/function-name) that is presented in
    each line.
    """

    try:
        print("executing: ", statement)
        exec(statement)
        print("Success!")
    except SystemExit:
        print("Failure!")
        pass
    finally:
        print("Done")

if __name__ == '__main__':
    # run the script, yo
    from optparse import OptionParser
    usage = "perfstubs.py scriptfile [arg] ..."
    parser = OptionParser(usage=usage)
    parser.allow_interspersed_args = False

    if not sys.argv[1:]:
        parser.print_usage()
        sys.exit(2)

    (options, args) = parser.parse_args()
    sys.argv[:] = args

    if (len(sys.argv) > 0):
        sys.path.insert(0, os.path.dirname(sys.argv[0]))
        init_tracing()
        #run('execfile(%r)' % (sys.argv[0],))
        run('exec(open(%r).read())' % (sys.argv[0],))
        fini_tracing()
    else:
        parser.print_usage()
