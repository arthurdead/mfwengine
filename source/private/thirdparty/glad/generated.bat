@echo off
setlocal

%~d0
set dir=%~dp0
cd "%dir%"

set pydir=C:\Program Files (x86)\Microsoft Visual Studio\Shared\Python37_64

cd "generated"

if exist "release" (
	rmdir "release" /s /q
)

if exist "debug" (
	rmdir "debug" /s /q
)

"%pydir%\python.exe" "%dir%\generated.py" --gladdir=%1 --outdir=%2 --wgl

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%dir%\generated.py" --gladdir=%1 --outdir=%2

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%dir%\generated.py" --gladdir=%1 --outdir=%2 --vulkan

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%dir%\generated.py" --gladdir=%1 --outdir=%2 --wgl --debug

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%dir%\generated.py" --gladdir=%1 --outdir=%2 --debug

if errorlevel 1 (
	pause
	exit
)

"%pydir%\python.exe" "%dir%\generated.py" --gladdir=%1 --outdir=%2 --vulkan --debug

if errorlevel 1 (
	pause
	exit
)

endlocal
exit