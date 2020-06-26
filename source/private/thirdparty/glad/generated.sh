#!/bin/bash

function check_if_success
{
	if [ ! ${?} -eq 0 ]; then
		exit
	fi
}

sh_dir="$(realpath "$(dirname "${BASH_SOURCE[0]}")")"

cd "generated"

rm -r -f "release"
rm -r -f "debug"

python "${sh_dir}/generated.py" --gladdir="${1}" --outdir="${2}" --glx
python "${sh_dir}/generated.py" --gladdir="${1}" --outdir="${2}" --egl
python "${sh_dir}/generated.py" --gladdir="${1}" --outdir="${2}"

python "${sh_dir}/generated.py" --gladdir="${1}" --outdir="${2}" --glx --debug
python "${sh_dir}/generated.py" --gladdir="${1}" --outdir="${2}" --egl --debug
python "${sh_dir}/generated.py" --gladdir="${1}" --outdir="${2}" --debug

exit