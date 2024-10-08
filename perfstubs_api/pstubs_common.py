import perfstubs
import sys

# example filters
bootstrap_timers = ["_run_module_code"]

if sys.version_info[1] >= 12:
    import pstubs_sys_monitoring as pstubs_impl
else:
    import pstubs_sys_setprofile as pstubs_impl

def get_python_version():
    return perfstubs.get_python_version()

def pstubs_write_trace(in_trace):
    linenum = 0
    while in_trace:
        metadata_name = "PY-BACKTRACE( ) {0}".format(linenum)
        metadata_val = str(in_trace.pop())
        pytau.metadata(metadata_name, metadata_val)
        linenum = linenum + 1

import signal, sys, traceback,os
def pstubs_signal_handler(signum, frame):
    ext_trace = []
    for threadId, stack in sys._current_frames().items():
        for filename, lineno, funcname, line in traceback.extract_stack(stack):
            if funcname == sys._getframe().f_code.co_name:
                continue
            ext_trace.append("[{0}] [{1}:{2}]".format(funcname, filename, lineno))
    #Signals do not have the same value in different OS systems
    #and strsignal(signalnum) is not available until v3.8
    metadata_name = "PY-SIGNAL"
    if signum == signal.SIGILL:
        metadata_val = "Illegal Instruction"
    if signum == signal.SIGINT:
        metadava_val = "Keyboard Interruption"
    if signum == signal.SIGQUIT:
        metadava_val = "Quit signal"
    if signum == signal.SIGTERM:
        metadata_val = "Termination signal"
    if signum == signal.SIGPIPE:
        metadata_val = "Broken pipe"
    elif signum == signal.SIGABRT:
        metadata_val = "Abort signal"
    elif signum == signal.SIGFPE:
        metadata_val = "Floating-point exception"
    elif signum == signal.SIGBUS:
        metadata_val = "Bus error(bad memory access)"
    elif signum == signal.SIGSEGV :
        metadata_val = "Segmentation Fault"
    else:
        metadata_val = "Unknown signal"
    pytau.metadata(metadata_name, metadata_val)
    pstubs_write_trace(ext_trace)
    sys.exit(1)

def pstubs_excepthook(type, value, tb):
    ext_trace = []
    for filename, lineno, funcname, line in traceback.extract_tb(tb):
        ext_trace.append("[{0}] [{1}:{2}]".format(funcname, filename, lineno))
    metadata_name = "PY-Exception"
    metadata_val = str(value)
    pytau.metadata(metadata_name, metadata_val)
    pstubs_write_trace(ext_trace)

def pstubs_listen_signals():
    if os.getenv("TAU_TRACK_PYSIGNALS")=='1':
        signal.signal(signal.SIGILL,  pstubs_signal_handler)
        signal.signal(signal.SIGINT,  pstubs_signal_handler)
        signal.signal(signal.SIGQUIT, pstubs_signal_handler)
        signal.signal(signal.SIGTERM, pstubs_signal_handler)
        signal.signal(signal.SIGPIPE, pstubs_signal_handler)
        signal.signal(signal.SIGABRT, pstubs_signal_handler)
        signal.signal(signal.SIGFPE,  pstubs_signal_handler)
        #TauEnv_get_memdbg
        signal.signal(signal.SIGBUS,  pstubs_signal_handler)
        signal.signal(signal.SIGSEGV, pstubs_signal_handler)

def pstubs_enable_excepthook():
    if os.getenv("TAU_TRACK_PYSIGNALS")=='1':
        sys.excepthook = pstubs_excepthook

def run(statement):
    """
    run a statement specified by 'statement'.
    """
    pstubs_listen_signals()
    pstubs_enable_excepthook()

    try:
        pstubs_impl.init_tracing()
        exec(statement)
        pstubs_impl.fini_tracing()
    except SystemExit:
        pass

def runmodule(modname):
    """
    Compile and run a module specified by 'modname', setting __main__ to that module.
    """
    pstubs_listen_signals()
    pstubs_enable_excepthook()
    newname='__main__'
    try:
        import runpy
        pstubs_impl.init_tracing()
        runpy.run_module(modname, run_name=newname, alter_sys=True)
        pstubs_impl.fini_tracing()
    except SystemExit:
        pass

def runmoduledir(modname, filename=None):
    """
    Compile and run a module directory specified by 'modnamedir', setting __main__ to that module.
    """
    pstubs_listen_signals()
    pstubs_enable_excepthook()
    newname='__main__'
    try:
        import runpy
        pstubs_impl.init_tracing()
        runpy.run_module(modname, run_name=newname, alter_sys=True)
        pstubs_impl.fini_tracing()
    except SystemExit:
        pass

def runpath(path_name):
    """
    Compile and run a script specified by 'path_name', setting __main__ to that module.
    """
    pstubs_listen_signals()
    pstubs_enable_excepthook()
    newname='__main__'
    try:
        import runpy
        pstubs_impl.init_tracing()
        runpy.run_path(path_name, run_name=newname)
        pstubs_impl.fini_tracing()
    except SystemExit:
        pass

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
        pstubs_impl.init_tracing()
        #run('execfile(%r)' % (sys.argv[0],))
        print("profiling: ", sys.argv[0])
        run('exec(open(%r).read())' % (sys.argv[0],))
        pstubs_impl.fini_tracing()
    else:
        parser.print_usage()

