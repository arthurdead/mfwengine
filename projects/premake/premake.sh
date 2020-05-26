#!/bin/bash

#rm -rf generated
mkdir -p generated
cd generated

#premake5 --file="../premake.lua" clean
#premake5 --file="../premake.lua" gmake
premake5 --file="../premake.lua" gmake2
#premake5 --file="../premake.lua" codelite

exit
