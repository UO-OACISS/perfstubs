# Copyright 2013-2022 Lawrence Livermore National Security, LLC and other
# Spack Project Developers. See the top-level COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack import *


class Perfstubs(Package):
    """Profiling API for adding external tool instrumentation support to any project.

This was motivated by the need to quickly add instrumentation to the
[ADIOS2](https://github.com/ornladios/ADIOS2) library without adding a build
dependency, or tying to a specific measurement tool.

The initial prototype implementation was tied to TAU, but evolved to this more
generic version, which was extracted as a separate repository for testing and
demonstration purposes.
"""

    # proper url for your package's homepage here.
    homepage = "https://github.com/khuck/perfstubs"
    url      = "https://github.com/khuck/perfstubs.git"

    variant('static', default=False, description='Build static executable support')

    version('develop',
            git='https://github.com/khuck/perfstubs.git',
            branch='master',
            submodules=True,
            preferred=True)

    depends_on("cmake", type='build')

    def install(self, spec, prefix):
        with working_dir('spack-build', create=True):
            cmake_args = []
            for arg in std_cmake_args:
                cmake_args.append(arg)
            if "+static" in spec:
                cmake_args.append('-DPERFSTUBS_USE_STATIC=ON')
            cmake_args.append('..')
            print("Configuring Perfstubs...")
            cmake(*cmake_args)
            print("Building Perfstubs...")
            make()
            print("Installing Perfstubs...")
            make('install')

