#!/bin/bash

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

export build_scripts="${sh_dir}"
cd "${build_scripts}"

export python="/bin/python"

export PATH="${PATH};${build_scripts}"
