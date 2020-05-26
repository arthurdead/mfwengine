#!/bin/bash

sh_dir=$(realpath "$(dirname "${BASH_SOURCE[0]}")")

script_name="$(basename "${BASH_SOURCE[0]}")"
script_name="${script_name%.*}"
export script_name
sh "${sh_dir}/execute_python_script.sh" "${@}"
