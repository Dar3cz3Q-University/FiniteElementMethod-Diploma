workspace "FiniteElementMethod"
  architecture "x64"
  startproject "App"

  configurations {
    "Debug",
    "Release"
  }

  flags { "MultiProcessorCompile" }

  OutputDir = "%{_WORKING_DIR}/build/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
  IntermediateDir = "%{_WORKING_DIR}/build/bin/int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

  local vcpkgTriplet = "x64-windows"
  local vcpkgInstalled = path.getabsolute("vcpkg_installed/" .. vcpkgTriplet)

  includedirs { vcpkgInstalled .. "/include" }
  libdirs { vcpkgInstalled .. "/lib" }

  includedirs { vcpkgInclude }
  libdirs { vcpkgLib }

  links {}

  include "FiniteElementMethod/premake5"
