@echo off
echo === Setting up FiniteElementMethod project ===

pushd ..

echo.
echo [1/2] Installing vcpkg dependencies...
External\vcpkg\vcpkg install --triplet=x64-windows

if %errorlevel% neq 0 (
    popd
    echo ERROR: vcpkg install failed!
    pause
    exit /b %errorlevel%
)

echo.
echo [2/2] Generating Visual Studio solution...
Vendor\Bin\Premake\Windows\premake5.exe --file=premake5.lua vs2022

if %errorlevel% neq 0 (
    popd
    echo ERROR: premake5 failed!
    pause
    exit /b %errorlevel%
)

popd

echo.
echo === Setup complete! ===
echo You can now open FiniteElementMethod.sln in Visual Studio
pause
