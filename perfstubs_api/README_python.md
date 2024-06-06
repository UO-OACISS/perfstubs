Python support is now included.

Building the perfstubs library will now also build `perfstubs.so`, which
is a Python C extension that uses the new low-overhead profiling support
in Python 3.12+, see
[https://docs.python.org/3/library/sys.monitoring.html](https://docs.python.org/3/library/sys.monitoring.html).
There is a python module, `pstubs.py` in the
perfstubs `<install-prefix>/lib` directory.

To test the Python support, do the following:

```bash
git clone --branch python-3.12 https://github.com/UO-OACISS/perfstubs.git
cd perfstubs
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=`pwd`/install
cmake --build build --parallel --target install
export PYTHONPATH=`pwd`/install/lib:$PYTHONPATH
```

To try it with a working TAU installation:

```
tau_exec -T pthread,serial python3 -m pstubs ./examples/firstprime_3.12.py
pprof -a
```

shoud give:

```
khuck@Kevins-MacBook-Air perfstubs % tau_exec -T pthread,serial python3 -m pstubs ./examples/firstprime_3.12.py
profiling:  ./examples/firstprime_3.12.py
The first prime number after 10000000 is 10000019
Success!
Done
khuck@Kevins-MacBook-Air perfstubs % pprof
Reading Profile files in profile.*

NODE 0;CONTEXT 0;THREAD 0:
---------------------------------------------------------------------------------------
%Time    Exclusive    Inclusive       #Call      #Subrs  Inclusive Name
              msec   total msec                          usec/call
---------------------------------------------------------------------------------------
100.0          172          177           1           1     177738 .TAU application
  2.9        0.812            5           1           3       5238 run
  2.5        0.061            4           1           1       4362 firstPrimeAfter
  2.4        0.812            4           1           3       4301 _find_and_load
  1.8        0.048            3           1           3       3184 _find_and_load_unlocked
  1.7        0.109            3           1           9       3100 _find_spec
  1.6        0.508            2          10          82        282 find_spec
  1.0        0.052            1           2           3        867 _get_spec
  0.9        0.139            1           9          16        180 _path_importer_cache
  0.7        0.038            1           1           3       1259 _load_unlocked
  0.7        0.034            1           1           2       1183 module_from_spec
  0.6        0.024            1           1           2       1059 create_module
  0.6            1            1           3           0        349 _call_with_frames_removed
  0.4        0.198        0.652          10           7         65 __init__
  0.2        0.064        0.395           5           3         79 __enter__
  0.2        0.024        0.365           1           1        365 _path_hooks
  0.1        0.038        0.266           1           2        266 path_hook_for_FileFinder
  0.1        0.107        0.232           1           4        232 acquire
  0.1        0.168        0.168          10           0         17 _path_stat
  0.1        0.147        0.147          32           0          5 _path_join
  0.1        0.138        0.138           1           0        138 _fill_cache
  0.1        0.134        0.134          34           0          4 _verbose_message
  0.1        0.103        0.134           2           3         67 _path_abspath
  0.1        0.054         0.09           1           3         90 _init_module_attrs
  0.0        0.031        0.065           1           2         65 setdefault
  0.0        0.042         0.06           1           2         60 spec_from_file_location
  0.0        0.019        0.053           2           2         26 _path_is_mode_type
  0.0        0.035        0.047           5           1          9 __exit__
  0.0         0.02        0.045           1           1         45 _path_isdir
  0.0         0.01        0.038           1           1         38 _path_isfile
  0.0        0.019        0.035           1           2         35 exec_module
  0.0        0.034        0.034           7           0          5 _relax_case
  0.0        0.026        0.034           1           1         34 _get_module_lock
  0.0        0.031        0.031           1           0         31 remove
  0.0        0.016        0.025           1           1         25 cached
  0.0        0.017        0.017           2           0          8 _path_isabs
  0.0        0.015        0.015           1           0         15 decode
  0.0        0.015        0.015           3           0          5 <genexpr>
  0.0        0.012        0.012           1           0         12 release
  0.0        0.009        0.009           1           0          9 _get_cached
  0.0        0.008        0.008           1           0          8 cb
  0.0        0.008        0.008           1           0          8 __new__
  0.0        0.006        0.006           1           0          6 parent
  0.0        0.005        0.005           1           0          5 has_location
```

To try it with a working APEX installation:

```
apex_exec --apex:csv python3 -m pstubs ./examples/firstprime_3.12.py
apex-summary.py
```

should give:

```
khuck@Kevins-MacBook-Air perfstubs % apex_exec python3 -m pstubs ./examples/firstprime_3.12.py
Enabling memory tracking!
          (            )
   (      )\ )      ( /(
   )\    (()/( (    )\())
((((_)(   /(_)))\  ((_)\
 )\ _ )\ (_)) ((_) __((_)
 (_)_\(_)| _ \| __|\ \/ /
  / _ \  |  _/| _|  >  <
 /_/ \_\ |_|  |___|/_/\_\
APEX Version: v2.6.5-2876220-develop
Built on: 11:07:57 Feb 22 2024 (Release)
C++ Language Standard version : 201703
Clang Compiler version : Apple LLVM 15.0.0 (clang-1500.1.0.2.5)
Configured features: Pthread, OTF2

profiling:  ./examples/firstprime_3.12.py
The first prime number after 10000000 is 10000019
Success!
Done

Start Date/Time: 06/06/2024 15:15:27
Elapsed time: 0.060933 seconds
Total processes detected: 1
HW Threads detected on rank 0: 0
Worker Threads observed on rank 0: 1
Available CPU time on rank 0: 0 seconds
Available CPU time on all ranks: 0 seconds

CPU Timers                                           : #calls|   mean |   total
--------------------------------------------------------------------------------
                                           APEX MAIN :      1     0.06     0.06
                  firstPrimeAfter [{<string>} {7,0}] :      1     0.00     0.00
_find_and_load [{<frozen importlib._bootstrap>} {13… :      1     0.00     0.00
_find_and_load_unlocked [{<frozen importlib._bootst… :      1     0.00     0.00
_find_spec [{<frozen importlib._bootstrap>} {1240,0… :      1     0.00     0.00
_load_unlocked [{<frozen importlib._bootstrap>} {91… :      1     0.00     0.00
module_from_spec [{<frozen importlib._bootstrap>} {… :      1     0.00     0.00
find_spec [{<frozen importlib._bootstrap_external>}… :      1     0.00     0.00
_path_importer_cache [{<frozen importlib._bootstrap… :      8     0.00     0.00
_get_spec [{<frozen importlib._bootstrap_external>}… :      1     0.00     0.00
create_module [{<frozen importlib._bootstrap_extern… :      1     0.00     0.00
_call_with_frames_removed [{<frozen importlib._boot… :      3     0.00     0.00
find_spec [{<frozen importlib._bootstrap_external>}… :      6     0.00     0.00
_path_hooks [{<frozen importlib._bootstrap_external… :      1     0.00     0.00
              __init__ [{<frozen zipimport>} {64,0}] :      1     0.00     0.00
path_hook_for_FileFinder [{<frozen importlib._boots… :      1     0.00     0.00
 __enter__ [{<frozen importlib._bootstrap>} {416,0}] :      1     0.00     0.00
_path_stat [{<frozen importlib._bootstrap_external>… :      9     0.00     0.00
__init__ [{<frozen importlib._bootstrap_external>} … :      1     0.00     0.00
_path_join [{<frozen importlib._bootstrap_external>… :     27     0.00     0.00
_fill_cache [{<frozen importlib._bootstrap_external… :      1     0.00     0.00
_verbose_message [{<frozen importlib._bootstrap>} {… :     29     0.00     0.00
_path_abspath [{<frozen importlib._bootstrap_extern… :      2     0.00     0.00
   acquire [{<frozen importlib._bootstrap>} {304,0}] :      1     0.00     0.00
_get_spec [{<frozen importlib._bootstrap_external>}… :      1     0.00     0.00
_init_module_attrs [{<frozen importlib._bootstrap>}… :      1     0.00     0.00
                __init__ [{<frozen codecs>} {309,0}] :      1     0.00     0.00
_path_is_mode_type [{<frozen importlib._bootstrap_e… :      2     0.00     0.00
 __enter__ [{<frozen importlib._bootstrap>} {162,0}] :      1     0.00     0.00
_path_isdir [{<frozen importlib._bootstrap_external… :      1     0.00     0.00
spec_from_file_location [{<frozen importlib._bootst… :      1     0.00     0.00
_path_isfile [{<frozen importlib._bootstrap_externa… :      1     0.00     0.00
setdefault [{<frozen importlib._bootstrap>} {124,0}] :      1     0.00     0.00
exec_module [{<frozen importlib._bootstrap_external… :      1     0.00     0.00
_get_module_lock [{<frozen importlib._bootstrap>} {… :      1     0.00     0.00
_relax_case [{<frozen importlib._bootstrap_external… :      6     0.00     0.00
                  decode [{<frozen codecs>} {319,0}] :      1     0.00     0.00
  __exit__ [{<frozen importlib._bootstrap>} {420,0}] :      1     0.00     0.00
    cached [{<frozen importlib._bootstrap>} {632,0}] :      1     0.00     0.00
find_spec [{<frozen importlib._bootstrap>} {1128,0}] :      1     0.00     0.00
                __init__ [{<frozen codecs>} {260,0}] :      1     0.00     0.00
<genexpr> [{<frozen importlib._bootstrap_external>}… :      3     0.00     0.00
__enter__ [{<frozen importlib._bootstrap>} {1222,0}] :      3     0.00     0.00
 __exit__ [{<frozen importlib._bootstrap>} {1226,0}] :      3     0.00     0.00
   release [{<frozen importlib._bootstrap>} {372,0}] :      1     0.00     0.00
_path_isabs [{<frozen importlib._bootstrap_external… :      2     0.00     0.00
 find_spec [{<frozen importlib._bootstrap>} {982,0}] :      1     0.00     0.00
  __init__ [{<frozen importlib._bootstrap>} {232,0}] :      1     0.00     0.00
_get_cached [{<frozen importlib._bootstrap_external… :      1     0.00     0.00
    __new__ [{<frozen importlib._bootstrap>} {74,0}] :      1     0.00     0.00
        cb [{<frozen importlib._bootstrap>} {445,0}] :      1     0.00     0.00
     remove [{<frozen importlib._bootstrap>} {82,0}] :      1     0.00     0.00
  __init__ [{<frozen importlib._bootstrap>} {412,0}] :      1     0.00     0.00
   __init__ [{<frozen importlib._bootstrap>} {79,0}] :      1     0.00     0.00
    parent [{<frozen importlib._bootstrap>} {645,0}] :      1     0.00     0.00
  __init__ [{<frozen importlib._bootstrap>} {599,0}] :      1     0.00     0.00
  __exit__ [{<frozen importlib._bootstrap>} {173,0}] :      1     0.00     0.00
__init__ [{<frozen importlib._bootstrap_external>} … :      1     0.00     0.00
has_location [{<frozen importlib._bootstrap>} {653,… :      1     0.00     0.00
  __init__ [{<frozen importlib._bootstrap>} {158,0}] :      1     0.00     0.00
--------------------------------------------------------------------------------


--------------------------------------------------------------------------------
                                        Total timers : 149
```
