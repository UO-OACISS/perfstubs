/* This file contains Python support for the perfstubs API.
 */

// this line has to go first
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#define PERFSTUBS_USE_TIMERS
#include "timer.h"
#include "event_filter.hpp"
#include <queue>
#include <iostream>

std::deque<std::string>& timerStack(void) {
    static thread_local std::deque<std::string> theStack;
    return theStack;
}

extern "C" {

static PyObject* perfstubs_initialize([[maybe_unused]] PyObject *self, [[maybe_unused]] PyObject *args) {
    ps_initialize_();
    // we return no useful argument, so return the Python None object.
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* perfstubs_finalize([[maybe_unused]] PyObject *self, [[maybe_unused]] PyObject *args) {
    ps_finalize_();
    // we return no useful argument, so return the Python None object.
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* perfstubs_get_python_version([[maybe_unused]] PyObject *self, [[maybe_unused]] PyObject* noarg) {
  int major = PY_MAJOR_VERSION;
  int minor = PY_MINOR_VERSION;
  int micro = PY_MICRO_VERSION;
  PyObject * result = Py_BuildValue("iii", major, minor, micro);
  return result;
}

static inline bool exclude_it(const char * name, const char * file) {
    // disable perfstubs methods, no need to time them
    if (strstr(file, "pstubs_") != NULL ) { return true; }
    // disable keras/tensorflow callbacks
    if (strstr(file, "keras/callbacks.py") != NULL ) { return true; }
    /* Check to see whether we are excluding this timer. */
    if (perfstubs::python::event_filter::instance().have_filter &&
        perfstubs::python::event_filter::exclude(name, file)) {
        return true;
    }
    return false;
}

static PyObject* perfstubs_start([[maybe_unused]] PyObject *self, PyObject *args) {
    const char *timer_name;
    const char *file_name;
    uint64_t line_number;
    /* Parse the python arguments */
    if (PyArg_ParseTuple(args, "ssl", &timer_name, &file_name, &line_number)) {
        if (exclude_it(timer_name, file_name)) {
            /* return false, telling our python code to disable this timer, if possible */
            Py_INCREF(Py_False);
            return Py_False;
        }
        /* construct a timer name, then start the timer */
        char * tmpstr = nullptr;
            tmpstr = ps_make_timer_name_(file_name, timer_name, line_number);
        //std::cout << "START: " << std::string(timerStack().size(), ' ') << tmpstr << " {" << std::endl;
        ps_start_string_(tmpstr);
        /* save the timer name on the timer stack */
        timerStack().push_back(std::string(tmpstr));
        /* clean up memory */
        free(tmpstr);
    }
    // we return no useful argument, so return the Python None object.
    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* perfstubs_stop([[maybe_unused]] PyObject *self, [[maybe_unused]] PyObject *args) {
/* Ideally, we could construct the name, but the line nunber we get is the return
   statement, not the start of the function. So we maintain a thread_local stack
   and just pop-and-stop the current timer on this thread. */
    const char *timer_name;
    const char *file_name;
    uint64_t line_number;
    /* Parse the python arguments */
    if (PyArg_ParseTuple(args, "ssl", &timer_name, &file_name, &line_number)) {
        /* Check to see whether we are excluding this timer. If we excluded it at start,
         * we shouldn't get the stop...but you never know. */
        if (exclude_it(timer_name, file_name)) {
            /* return false, telling our python code to disable this timer, if possible */
            Py_INCREF(Py_False);
            return Py_False;
        }
        /* construct a timer name */
        char * tmpstr = nullptr;
            tmpstr = ps_make_timer_name_(file_name, timer_name, line_number);
        //std::cout << "STOP:  " << std::string(timerStack().size()-1, ' ') << tmpstr << " }" << std::endl;
        // should we handle this error?
        if (timerStack().size() > 0) {
            // get the top level timer
            auto timer = timerStack().back();
            /* timer should be "timer [{filename} {lineno}]"
             * so find this ----------------------^ bracket */
            size_t last_bracket = timer.find_last_of('{');
            /* We only want to compare the timer name file name, not line number */
            #if 1
            bool exists = false;
            if (timer.compare(0,last_bracket,tmpstr,last_bracket) != 0) {
                for ( auto t : timerStack()) {
                    if (t.compare(0,last_bracket,tmpstr,last_bracket) == 0) {
                        exists = true;
                    }
                }
                if (!exists) {
                    std::cerr << "\tError! no fix for timer " << tmpstr << std::endl;
                    // we return no useful argument, so return the Python None object.
                    Py_INCREF(Py_None);
                    return Py_None;
                }
            }
            bool print = false;
            while (timer.compare(0,last_bracket,tmpstr,last_bracket) != 0) {
                /* return false, telling our python code to disable this timer */
                if (!print) {
                std::cerr << "\tError! bracket = " << last_bracket << " Mismatched timer names from sys.monitoring / sys.profile!\n\t"
                          << timer << "\n\t\ton stack doesn't match with requested timer:\n\t\t\t" << tmpstr << std::endl;
                          print = true;
                }
                std::cout << "FIX:   " << std::string(timerStack().size()-1, ' ') << timer << " }" << std::endl;
                ps_stop_string_(timer.c_str());
                timerStack().pop_back();
                timer = timerStack().back();
                last_bracket = timer.find_last_of('{');
            }
            #else
            if (timer.compare(0,last_bracket,tmpstr,last_bracket) != 0) {
                std::cerr << "\tError! bracket = " << last_bracket << " Mismatched timer names from sys.monitoring / sys.profile!\n\t"
                          << timer << "\n\t\ton stack doesn't match with requested timer:\n\t\t\t" << tmpstr << std::endl;
            }
            #endif
            /* stop the timer */
            ps_stop_string_(timer.c_str());
            timerStack().pop_back();
        }
        /* Clean up memory */
        free(tmpstr);
    }
    // we return no useful argument, so return the Python None object.
    Py_INCREF(Py_None);
    return Py_None;
}

static PyMethodDef PerfStubsMethods[] = {
    {"get_python_version", perfstubs_get_python_version, METH_VARARGS,
     "Get compiled Python version for libperfstubs.so."},
    {"initialize", perfstubs_initialize, METH_VARARGS,
     "Initialize PerfStubs."},
    {"finalize", perfstubs_finalize, METH_VARARGS,
     "Finalize PerfStubs."},
    {"start", perfstubs_start, METH_VARARGS,
     "Start a PerfStubs timer."},
    {"stop", perfstubs_stop, METH_VARARGS,
     "Stop a PerfStubs timer."},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

PyModuleDef_Slot PerfStubsSlots[] = {
    {0, NULL}
};

static struct PyModuleDef perfstubsmodule = {
    PyModuleDef_HEAD_INIT,
    "perfstubs",   /* name of module */
    NULL, /* module documentation, may be NULL */ // fixme!
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    PerfStubsMethods /* the m_methods object, required */
#if 0
    PerfStubsSlots, /* the m_slots object, optional */
    NULL, /* the m_traverse function, optional */
    NULL, /* the m_clear function, optional */
    NULL /* the m_clear function, optional */
#endif
};

PyMODINIT_FUNC PyInit_perfstubs(void) {
    return PyModule_Create(&perfstubsmodule);
}

} // extern "C"


