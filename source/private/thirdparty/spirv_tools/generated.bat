@echo off
setlocal

%~d0
set dir=%~dp0
cd "%dir%"

set pydir=C:\Program Files (x86)\Microsoft Visual Studio\Shared\Python37_64
set spirv_tools=C:\VulkanSDK\1.1.114.0\spirv-tools

"%pydir%\python.exe" "%spirv_tools%\utils\generate_grammar_tables.py"^
 --spirv-core-grammar="%spirv_tools%\external\spirv-headers\include\spirv\unified1\spirv.core.grammar.json"^
 --extinst-debuginfo-grammar="%spirv_tools%\source\extinst.debuginfo.grammar.json"^
 --extinst-glsl-grammar="%spirv_tools%\external\spirv-headers\include\spirv\unified1\extinst.glsl.std.450.grammar.json"^
 --extinst-opencl-grammar="%spirv_tools%\external\spirv-headers\include\spirv\unified1\extinst.opencl.std.100.grammar.json"^
 --core-insts-output="generated\core.insts-unified1.inc"^
 --glsl-insts-output="generated\glsl.std.450.insts.inc"^
 --opencl-insts-output="generated\opencl.std.insts.inc"^
 --operand-kinds-output="generated\operand.kinds-unified1.inc"^
 --extension-enum-output="generated\extension_enum.inc"^
 --enum-string-mapping-output="generated\enum_string_mapping.inc"

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%spirv_tools%\utils\generate_grammar_tables.py"^
 --extinst-vendor-grammar="%spirv_tools%\source\extinst.spv-amd-gcn-shader.grammar.json"^
 --vendor-insts-output="generated\spv-amd-gcn-shader.insts.inc"

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%spirv_tools%\utils\generate_grammar_tables.py"^
 --extinst-vendor-grammar="%spirv_tools%\source\extinst.spv-amd-shader-ballot.grammar.json"^
 --vendor-insts-output="generated\spv-amd-shader-ballot.insts.inc"

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%spirv_tools%\utils\generate_grammar_tables.py"^
 --extinst-vendor-grammar="%spirv_tools%\source\extinst.spv-amd-shader-explicit-vertex-parameter.grammar.json"^
 --vendor-insts-output="generated\spv-amd-shader-explicit-vertex-parameter.insts.inc"

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%spirv_tools%\utils\generate_grammar_tables.py"^
 --extinst-vendor-grammar="%spirv_tools%\source\extinst.spv-amd-shader-trinary-minmax.grammar.json"^
 --vendor-insts-output="generated\spv-amd-shader-trinary-minmax.insts.inc"

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%spirv_tools%\utils\update_build_version.py" "%spirv_tools%" "generated\build-version.inc"

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%spirv_tools%\utils\generate_registry_tables.py"^
 --xml="%spirv_tools%\external\spirv-headers\include\spirv\spir-v.xml"^
 --generator-output="generated\generators.inc"

if errorlevel 1 (
	pause
	exit
)

endlocal
exit