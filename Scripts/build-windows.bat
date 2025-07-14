@echo off

pushd ..
Vendor\Bin\Premake\Windows\premake5.exe --file=premake5.lua vs2022
popd
pause
