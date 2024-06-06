Python support is now included.

To test the Python support, do the following:

```bash
git clone --branch python-3.12 https://github.com/UO-OACISS/perfstubs.git
cd perfstubs
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=`pwd`/install
cmake --build build --parallel
export PYTHONPATH=`pwd`/build:$PYTHONPATH
```

To try it with a working TAU installation:

```
tau_exec -T serial,pthread python3 ./examples/perfstubs_py.py
pprof -a
```

To try it with a working APEX installation:

```
apex_exec --apex:csv python3 ./examples/perfstubs_py.py
apex-summary.py
```
