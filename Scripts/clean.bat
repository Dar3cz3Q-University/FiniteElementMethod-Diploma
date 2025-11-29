@echo off
echo === Cleaning FiniteElementMethod project ===

pushd ..

echo.
echo Removing build directories...
if exist build rmdir /s /q build

echo Removing Visual Studio files...
for /r %%i in (*.sln) do del /q "%%i"
for /r %%i in (*.vcxproj) do del /q "%%i"
for /r %%i in (*.vcxproj.filters) do del /q "%%i"
for /r %%i in (*.vcxproj.user) do del /q "%%i"
if exist .vs rmdir /s /q .vs

echo Removing vcpkg installed packages...
if exist vcpkg_installed rmdir /s /q vcpkg_installed

popd

echo.
echo === Clean complete! ===
pause
