#!/bin/bash
set -e

echo "=== Setting up FiniteElementMethod project ==="

cd ..

echo ""
echo "[1/2] Installing vcpkg dependencies..."
External/vcpkg/vcpkg install --triplet=x64-linux

echo ""
echo "[2/2] Generating Makefiles..."
Vendor/Bin/Premake/Linux/premake5 --file=premake5.lua gmake2

echo ""
echo "=== Setup complete! ==="
echo "You can now build with: make config=debug"
echo "or: make config=release"
