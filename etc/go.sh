#!/bin/bash -e

# where is this script?
if [ -z ${scriptdir} ] ; then
    scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

workdir="$( dirname "${scriptdir}" )"
echo $workdir

rm -rf ${workdir}/build_static
mkdir ${workdir}/build_static
cd ${workdir}/build_static

cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPERFSTUBS_USE_STATIC=ON ..
make -j
make test

rm -rf ${workdir}/build_dynamic
mkdir ${workdir}/build_dynamic
cd ${workdir}/build_dynamic

cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPERFSTUBS_USE_STATIC=OFF ..
make -j
make test

