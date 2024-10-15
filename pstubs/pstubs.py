# -*- coding: utf-8 -*-
#
# Copyright (c) 2015, ParaTools, Inc.
# Copyright (c) 2024, University of Oregon
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# (1) Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
# (2) Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
# (3) Neither the name of ParaTools, Inc. nor the names of its contributors may
#     be used to endorse or promote products derived from this software without
#     specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
usage = """PerfStubs python instrumentation wrapper.

usage: python3 -m pstubs SCRIPT
                         | -c PYTHON_STATEMENT
                         | -m MODULE

Runs SCRIPT, PYTHON_STATEMENT or MODULE with automatic Python instrumentation.

Examples:
    python3 -m pstubs myscript.py
    python3 -m pstubs -c 'print(4*3)'
    python3 -m pstubs -m mymodule
"""

import os
import sys
from pstubs import pstubs_common as pstubs_common

def dieInFlames(msg):
    print(msg, file=sys.stderr)
    sys.exit(-1)

if len(sys.argv) == 1:
    dieInFlames(usage)
    sys.exit(-1)

try:
    modname = sys.argv[1]
except IndexError:
    dieInFlames('Usage: %s <modulename>' % sys.argv[0])

try:
    if sys.version_info[0] < 3:
        raise Exception("Must be using Python 3")
    else:
        if sys.version_info[1] >= 12:
            from pstubs import pstubs_sys_monitoring as pstubs_impl
        else:
            from pstubs import pstubs_sys_setprofile as pstubs_impl
except ImportError:
    dieInFlames("modules 'pstubs_common', 'pstubs_sys_monitoring' or 'pstubs_sys_setprofile' not found in PYTHONPATH")
except:
    dieInFlames("Unknown exception while importing pstubs: %s" % sys.exc_info()[0])

compile_time_python_ver = pstubs_common.get_python_version()
runtime_python_ver = sys.version_info[0:3]
if compile_time_python_ver != runtime_python_ver:
    dieInFlames("Error: PerfStubs was compiled against Python {}.{}.{}, but version {}.{}.{} was used at runtime. The compile and runtime versions of Python must be identical.".format(compile_time_python_ver[0], compile_time_python_ver[1], compile_time_python_ver[2], runtime_python_ver[0], runtime_python_ver[1], runtime_python_ver[2]))

print("PerfStubs detected Python version {}.{}.{}".format(runtime_python_ver[0], runtime_python_ver[1], runtime_python_ver[2]))

if sys.argv[1] == '-c':
    # python3 -c 'some python commmand'
    command = sys.argv[2]
    del sys.argv[2]
    del sys.argv[0]
    pstubs_common.run(command)
elif sys.argv[1] == '-m':
    # python3 -m moduleName.foo
    modname = sys.argv[2]
    try:
        import pkgutil
        pkg_loader = pkgutil.get_loader(modname)
    except Exception as e:
        dieInFlames("The name '{}' does not name a module in $PYTHONPATH or $PWD".format(modname))
    if pkg_loader is None:
        dieInFlames("The name '{}' does not name a module in $PYTHONPATH or $PWD".format(modname))
    # When python is run with -m, current directory is added to search path
    sys.path.append(os.getcwd())
    # Find out the path to the module we are launching
    filename = ""
    try:
        # New way of getting package filename
        filename = pkg_loader.get_filename()
    except Exception as e:
        # old way
        filename = pkg_loader.filename
    sys.argv[0] = filename
    # remove the args
    del sys.argv[1]
    del sys.argv[1]
    if os.path.exists(modname) and filename[-3:].lower() != '.py':
        pstubs_common.runmodule(sys.argv[0])
    else:
        pstubs_common.runmoduledir(modname)
else:
    # If we launched a Python script using the normal method,
    # argv would have the path to the script in argv[0].
    # Fix argv to be as it would have been without wrapper script

    ## the "right way"
    sys.argv = sys.argv[1:]
    pstubs_common.runpath(modname)

    # The "way that doesn't create 1800 internal timers"
    """
    if os.path.exists(modname):
        path = os.path.dirname(modname)
        modname = os.path.basename(modname)
        if modname[-3:].lower() != '.py':
            dieInFlames("Sorry, I don't know how to instrument '%s'" % modname)
        modname = modname[:-3]
        sys.path.append(path)
        sys.argv = sys.argv[1:]
    pstubs_common.runmodule(modname)
    """

