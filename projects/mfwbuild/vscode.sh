#!/bin/bash

root_dir="../.."
mfwbuilder="${root_dir}/build_tools/mfwbuilder/builder_debug"

"${mfwbuilder}" --path="$(pwd)" --solutions="mfwengine" --plugins="vscode"

exit
