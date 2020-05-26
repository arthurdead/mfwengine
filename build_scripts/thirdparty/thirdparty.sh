#!/bin/bash

function check_if_success
{
	if [ ! ${?} -eq 0 ]; then
		exit
	fi
}

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

build_scripts="${sh_dir}/.."

sh "${build_scripts}/pip_install.sh" requests

exit
