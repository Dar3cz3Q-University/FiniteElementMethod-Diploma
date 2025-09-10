local vcpkg = require "vcpkg-manifest"

workspace "FiniteElementMethod"
  architecture "x64"
  startproject "App"

  configurations {
    "Debug",
    "Release"
  }

  flags { "MultiProcessorCompile" }

  buildoptions { "/utf-8" }

  OutputDir = "%{_WORKING_DIR}/build/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
  IntermediateDir = "%{_WORKING_DIR}/build/bin/int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

  local vcpkgTriplet = "x64-windows-static"
  local vcpkgInstalled = path.getabsolute("vcpkg_installed/" .. vcpkgTriplet)

  includedirs { vcpkgInstalled .. "/include" }

  filter "configurations:Debug"
    libdirs { vcpkgInstalled .. "/debug/lib" }

  filter "configurations:Release"
    libdirs { vcpkgInstalled .. "/lib" }

  links {
    "fmt",
    "spdlog"
  }

  vcpkg.enableVcpkgManifest()

  include "FiniteElementMethod/premake5"
