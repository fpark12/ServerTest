@echo off

rem ## back up CWD
pushd "%~dp0"

set CMAKE_BUILD_FLAG="LITE"
CALL xGenerateProjectFiles.bat

:Exit
rem ## Restore original CWD in case we change it
popd

