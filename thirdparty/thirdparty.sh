#!/bin/bash

function check_if_success
{
	if [ ! ${?} -eq 0 ]; then
		exit
	fi
}

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

build_scripts="${sh_dir}/../build_scripts"

sh "${build_scripts}/git_download.sh" -u "https://github.com/gcc-mirror/gcc.git" -p "libstdc++-v3/libsupc++/tinfo.h" -o "${sh_dir}/libsupc++"

sh "${build_scripts}/git_clone.sh" "https://github.com/arthurdead/Implib.so.git" master "Implib.so"

#sh "${build_scripts}/git_clone.sh" "https://github.com/microsoft/GSL.git" master GSL
#sh "${build_scripts}/git_clone.sh" "https://github.com/foonathan/debug_assert.git" master debug_assert
#sh "${build_scripts}/git_clone.sh" "https://github.com/foonathan/type_safe.git" master type_safe

#sh "${build_scripts}/git_clone.sh" "https://github.com/asmjit/asmjit.git" master asmjit
#sh "${build_scripts}/git_clone.sh" "https://github.com/albertodemichelis/squirrel.git" master squirrel
#sh "${build_scripts}/git_clone.sh" "https://github.com/rttrorg/rttr.git" master rttr
#sh "${build_scripts}/git_clone.sh" "https://github.com/codecat/angelscript-mirror.git" master angelscript
#sh "${build_scripts}/git_clone.sh" "https://github.com/bluecataudio/AngelScript-JIT-Compiler.git" master "AngelScript-JIT-Compiler"

#sh "${build_scripts}/git_clone.sh" "https://github.com/hfinkel/llvm-project-cxxjit.git" "cxxjit-ni-9.0" "llvm-project-cxxjit"
#sh "${build_scripts}/git_clone.sh" "http://root.cern.ch/git/llvm.git" "cling-patches" llvm
#sh "${build_scripts}/git_clone.sh" "http://root.cern.ch/git/cling.git" master "llvm/tools/cling"
#sh "${build_scripts}/git_clone.sh" "http://root.cern.ch/git/clang.git" "cling-patches" "llvm/tools/clang"

sh "${build_scripts}/git_clone.sh" "https://github.com/zeux/pugixml.git" master pugixml
sh "${build_scripts}/git_clone.sh" "https://github.com/Tencent/rapidjson.git" master rapidjson

#sh "${build_scripts}/git_clone.sh" "https://github.com/fmtlib/fmt.git" master fmtlib
#sh "${build_scripts}/git_clone.sh" "https://github.com/electronicarts/EABase.git" master EABase
#sh "${build_scripts}/git_clone.sh" "https://github.com/electronicarts/EAAssert.git" master EAAssert
#sh "${build_scripts}/git_clone.sh" "https://github.com/electronicarts/EASTL.git" master EASTL
#sh "${build_scripts}/git_clone.sh" "https://github.com/electronicarts/EAStdC.git" master EAStdC
#sh "${build_scripts}/git_clone.sh" "https://github.com/electronicarts/EAThread.git" master EAThread
#sh "${build_scripts}/git_clone.sh" "https://github.com/eigenteam/eigen-git-mirror.git" master eigen
#sh "${build_scripts}/git_clone.sh" "https://github.com/g-truc/glm.git" master glm
#sh "${build_scripts}/git_clone.sh" "https://github.com/Neargye/magic_enum.git" master magic_enum

#sh "${build_scripts}/git_clone.sh" "https://github.com/Dav1dde/glad.git" glad2 glad
#sh "${build_scripts}/git_clone.sh" "https://github.com/FLIF-hub/FLIF.git" master FLIF
#sh "${build_scripts}/git_clone.sh" "https://github.com/webmproject/libwebm.git" master libwebm
#libopus

#sh "${build_scripts}/git_clone.sh" "https://github.com/lz4/lz4.git" dev lz4
#sh "${build_scripts}/git_clone.sh" "https://github.com/postgres/postgres.git" master postgres

#sh "${build_scripts}/git_clone.sh" "https://github.com/NVIDIAGameWorks/PhysX.git" "4.1" PhysX

if false; then
	mkdir -p "Ultralight"
	cd "Ultralight"

	wget -x -q -O "Debug-linux-x64.7z" "https://ultralight-sdk-dbg.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-linux-x64.7z"
	check_if_success

	wget -x -q -O "Release-linux-x64.7z" "https://ultralight-sdk.sfo2.cdn.digitaloceanspaces.com/ultralight-sdk-latest-linux-x64.7z"
	check_if_success

	unzip "Debug-linux-x64.7z" -d "Debug/x64"
	check_if_success

	unzip "Release-linux-x64.7z" -d "Release/x64"
	check_if_success

	rm "Debug-linux-x64.7z"
	check_if_success

	rm "Release-linux-x64.7z"
	check_if_success

	cd ".."
fi

exit
