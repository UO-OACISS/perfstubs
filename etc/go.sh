#!/bin/bash -e
# Copyright (c) 2019-2022 University of Oregon
# Distributed under the BSD Software License
# (See accompanying file LICENSE.txt)

# where is this script?
if [ -z ${scriptdir} ] ; then
    scriptdir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
fi

workdir="$( dirname "${scriptdir}" )"
echo $workdir

UNAME=`uname`
FORTRAN_COMPILER=""
if [ ${UNAME} != "Darwin" ] ; then
    FORTRAN_COMPILER="-DCMAKE_Fortran_COMPILER=`which gfortran`"
fi

do_build() {
    echo "Removing build_${linktype}_${buildtype} and install_${linktype}_${buildtype}"
    rm -rf ${workdir}/build_${linktype}_${buildtype} ${workdir}/install_${linktype}_${buildtype}
    mkdir ${workdir}/build_${linktype}_${buildtype}
    cd ${workdir}/build_${linktype}_${buildtype}

    set -x
    cmake \
    -DCMAKE_C_COMPILER=`which gcc` \
    -DCMAKE_CXX_COMPILER=`which g++` \
    ${FORTRAN_COMPILER} \
    -DCMAKE_BUILD_TYPE=${buildtype} \
    -DCMAKE_INSTALL_PREFIX=${workdir}/install_${linktype}_${buildtype} \
    -DPERFSTUBS_USE_STATIC=${staticflag} \
    -DPERFSTUBS_BUILD_EXAMPLES=TRUE \
    ..
    #-DPERFSTUBS_SANITIZE=${sanitize} \
    set +x
    make -j
    make test
    make install
}

buildtype=Debug
sanitize=ON

linktype=dynamic
staticflag=OFF
do_build

linktype=static
staticflag=ON
do_build

buildtype=Release
sanitize=OFF

linktype=dynamic
staticflag=OFF
do_build

linktype=static
staticflag=ON
do_build


