/* this file exists so that the pyproject.toml build doesn't have
   to build a mix of C and C++ files - because the -std=C++17 flag
   gets passed to both. Regardless, the ABI that is exposed to
   Python is extern "C" in python.cpp, and the timer.c code can
   have mangled symbols because they only get called by python.cpp
 */
#include "timer.c"