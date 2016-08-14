rem ## back up CWD
pushd "%~dp0"
cd "%~dp0"

del *.sln
del *.lnk
for /d %%a in ("PortableCMake*") do (rmdir /s /q %%~fa)
rem ## @rmdir /s /q CMake
rem ## @rmdir /s /q Purify
@rmdir /s /q "%~dp0/Build"
cd Binaries/
del *.exe
del *.dll
@rmdir /s /q Backup
@rmdir /s /q Libraries
@rmdir /s /q Shaders
cd ../
@rmdir /q Binaries