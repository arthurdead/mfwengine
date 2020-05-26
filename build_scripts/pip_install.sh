#!/bin/bash

function check_if_success
{
    if [ ! ${?} -eq 0 ]; then
        exit
    fi
}

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

. "${sh_dir}/enviroment_variables.sh"

pip="/bin/pip"

for arg in "${@}"; do
    "${pip}" --quiet install "${arg}"
    check_if_success
done
