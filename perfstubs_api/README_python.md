Python support is now included.

Building the perfstubs library will now also build `perfstubs.so`, which
is a Python C extension. There is a python module, `pstubs.py` in the
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

To try it with a working APEX installation:

```
apex_exec --apex:csv python3 -m pstubs ./examples/firstprime_3.12.py
apex-summary.py
```
