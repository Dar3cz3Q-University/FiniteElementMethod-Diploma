local vcpkg = require "vcpkg-manifest"

local debugLibraries = {
  "fmtd",
  "spdlogd",
  "gmsh.dll"
}

local releaseLibraries = {
  "fmt",
  "spdlog",
  "gmsh.dll"
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

  VcpkgInstalled = path.getabsolute("vcpkg_installed/" .. vcpkgTriplet)

  externalincludedirs { VcpkgInstalled .. "/include" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    runtime "Debug"
    symbols "on"
    libdirs {
        VcpkgInstalled .. "/debug/lib",
        VcpkgInstalled .. "/lib"
    }
    links(debugLibraries)
  filter "configurations:Release"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "Full"
    libdirs { VcpkgInstalled .. "/lib" }
    links(releaseLibraries)
    linktimeoptimization "On"
  filter {}

  filter "system:windows"
    links { 
      "mkl_intel_lp64",
      "mkl_core",
      "mkl_intel_thread",
      "libiomp5md"
    }
  filter "system:linux"
    links {
      "mkl_intel_lp64",
      "mkl_core",
      "mkl_gnu_thread",
      "gomp",
      "pthread",
      "m",
      "dl"
    }
  filter {}

  vcpkg.enableVcpkgManifest()

  include "FiniteElementMethod/premake5"
