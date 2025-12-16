local vcpkg = require "vcpkg-manifest"

local debugLibraries = {
  "fmtd",
  "spdlogd",
  "gmsh.dll.lib",
  "xxhash",
  "mkl_intel_lp64",
  "mkl_intel_thread",
  "mkl_core",
  "libiomp5md"
}

local releaseLibraries = {
  "fmt",
  "spdlog",
  "gmsh.dll.lib",
  "xxhash",
  "mkl_intel_lp64",
  "mkl_intel_thread",
  "mkl_core",
  "libiomp5md"
}

workspace "FiniteElementMethod"
  architecture "x64"
  startproject "App"
  configurations {
    "Debug",
    "Release"
  }

  flags { "MultiProcessorCompile" }

  filter "system:windows"
    buildoptions { "/utf-8", "/openmp", "/external:W0" }
    staticruntime "off"
    links { "psapi" }
    externalwarnings "Off"
  filter "system:linux"
    buildoptions { "-fopenmp" }
    linkoptions { "-fopenmp" }
  filter {}

  OutputDir = "%{_WORKING_DIR}/build/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
  IntermediateDir = "%{_WORKING_DIR}/build/bin/int/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

  local vcpkgTriplet = ""
  if os.target() == "windows" then
    vcpkgTriplet = "x64-windows"
  elseif os.target() == "linux" then
    vcpkgTriplet = "x64-linux"
  end

  local vcpkgRoot = path.getabsolute("vcpkg_installed")

  VcpkgInstalled = vcpkgRoot .. "/" .. vcpkgTriplet

  externalincludedirs { VcpkgInstalled .. "/include" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    runtime "Debug"
    symbols "on"
    libdirs {
        VcpkgInstalled .. "/debug/lib",
    }
    links(debugLibraries)
  filter "configurations:Release"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "Full"
    libdirs { VcpkgInstalled .. "/lib" }
    links(releaseLibraries)
    linktimeoptimization "on"
  filter {}

  vcpkg.enableVcpkgManifest()

  include "FiniteElementMethod/premake5"
