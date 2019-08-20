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

rm -rf ${workdir}/build_static
mkdir ${workdir}/build_static
cd ${workdir}/build_static

#export CFLAGS="-fsanitize=address"
#export CXXFLAGS="-fsanitize=address"
#export LDFLAGS="-fsanitize=address"

cmake \
-DCMAKE_C_COMPILER=`which gcc` \
-DCMAKE_CXX_COMPILER=`which g++` \
-DCMAKE_Fortran_COMPILER=`which gfortran` \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_INSTALL_PREFIX=. \
-DPERFSTUBS_USE_STATIC=ON \
..
make
make test
make install

rm -rf ${workdir}/build_dynamic
mkdir ${workdir}/build_dynamic
cd ${workdir}/build_dynamic

cmake \
-DCMAKE_C_COMPILER=`which gcc` \
-DCMAKE_CXX_COMPILER=`which g++` \
-DCMAKE_Fortran_COMPILER=`which gfortran` \
-DCMAKE_BUILD_TYPE=Debug \
-DCMAKE_INSTALL_PREFIX=. \
-DPERFSTUBS_USE_STATIC=OFF \
..
make
make test
make install

