#!/bin/bash

function check_if_success
{
    if [ ! $? -eq 0 ]; then
        exit
    fi
}

filename=${0##*/}
basename=${filename%.*}

cd "${basename}"

gcc_args="-DCMAKE_C_COMPILER=/bin/gcc -DCMAKE_CXX_COMPILER=/bin/g++"
clang_args="-DCMAKE_C_COMPILER=/bin/clang -DCMAKE_CXX_COMPILER=/bin/clang++ -D_CMAKE_TOOLCHAIN_PREFIX=llvm-"

function build
{
	mkdir -p "build_${1}"
	cd "build_${1}"

	cmake ".." -DCMAKE_BUILD_TYPE="${1}" ${gcc_args} -DCMAKE_EXPORT_COMPILE_COMMANDS=True
	check_if_success

	make -s -S #2>&1 | tee "build.log"
	check_if_success

	cd ".."
}

build "Debug"
#build "RelWithDebInfo"
#build "Release"

exit
