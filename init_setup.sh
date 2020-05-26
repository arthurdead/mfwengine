#!/bin/bash

function check_if_success
{
	if [ ! ${?} -eq 0 ]; then
		exit
	fi
}

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

cd "${sh_dir}/thirdparty"
sh "./thirdparty.sh"
check_if_success
cd "${sh_dir}"

cd "${sh_dir}/build_scripts/thirdparty"
sh "./thirdparty.sh"
check_if_success
cd "${sh_dir}"

cd "${sh_dir}/projects/premake"
sh "./premake.sh"
check_if_success
cd "${sh_dir}"

exit
