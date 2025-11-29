#!/bin/bash

echo "=== Cleaning FiniteElementMethod project ==="

cd ..

echo ""
echo "Removing build directories..."
rm -rf build

echo "Removing Makefiles..."
shopt -s globstar
rm -f **/Makefile
rm -f **/*.make

echo "Removing vcpkg installed packages..."
rm -rf vcpkg_installed

echo ""
echo "=== Clean complete! ==="
