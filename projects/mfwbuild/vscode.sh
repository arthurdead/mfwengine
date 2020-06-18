#!/bin/bash

root_dir="../.."
mfwbuilder="${root_dir}/build_tools/mfwbuilder/builder_debug"
#mfwbuilder="${root_dir}/build_tools/mfwbuilder/builder_release"
#mfwbuilder="${root_dir}/build_tools/mfwbuilder/builder_release_debug"
#mfwbuilder="valgrind --tool=callgrind ${mfwbuilder}"
#mfwbuilder="gdb -ex run --args ${mfwbuilder}"

${mfwbuilder} --path="$(pwd)" --solutions="mfwengine" --plugins="vscode"

exit
