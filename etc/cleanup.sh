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
