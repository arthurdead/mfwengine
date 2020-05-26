#!/bin/bash

function check_if_success
{
    if [ ! ${?} -eq 0 ]; then
        exit
    fi
}

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

. "${sh_dir}/enviroment_variables.sh"

"${python}" "${build_scripts}/${script_name}.py" "${@}"
check_if_success
