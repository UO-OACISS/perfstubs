#!/bin/bash -e
# Copyright (c) 2019 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

# where is this script?
if [ -z ${scriptdir} ] ; then
    scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

workdir="$( dirname "${scriptdir}" )"
echo $workdir

export CFLAGS="-Wall -Werror"
export CXXFLAGS="-Wall -Werror"

do_build() {
    rm -rf ${workdir}/build_${linktype}_${buildtype} ${workdir}/install_${linktype}_${buildtype}
    mkdir ${workdir}/build_${linktype}_${buildtype}
    cd ${workdir}/build_${linktype}_${buildtype}

    cmake \
    -DCMAKE_C_COMPILER=`which gcc` \
    -DCMAKE_CXX_COMPILER=`which g++` \
    -DCMAKE_Fortran_COMPILER=`which gfortran` \
    -DCMAKE_BUILD_TYPE=${buildtype} \
    -DCMAKE_INSTALL_PREFIX=${workdir}/install_${linktype}_${buildtype} \
    -DPERFSTUBS_USE_STATIC=${staticflag} \
    ..
    make -j
    make test
    make install
}

buildtype=Debug
linktype=static
staticflag=ON
do_build
buildtype=Release
do_build

buildtype=Debug
linktype=dynamic
staticflag=OFF
do_build
buildtype=Release
do_build
