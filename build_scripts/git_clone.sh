#!/bin/bash

function check_if_success
{
    if [ ! ${?} -eq 0 ]; then
        exit
    fi
}

url="${1}"
branch="${2}"
folder="${3}"
commit="${4}"

if [ ! -d "${folder}/.git" ]; then
	git clone --depth=1 --single-branch --no-tags --shallow-submodules --branch="${branch}" "${url}" "${folder}"
    check_if_success
fi

cd "${folder}"

git fetch --quiet --depth=1 --no-tags --prune --prune-tags --no-recurse-submodules origin
check_if_success

if [ -z "${commit}" ]; then
	git reset --hard "origin/${branch}"
    check_if_success
else
	git reset --hard "${commit}"
    check_if_success
fi

git clean --quiet --force -d
check_if_success

cd ".."

exit
