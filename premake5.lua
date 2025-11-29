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

  local vcpkgInstalled = path.getabsolute("vcpkg_installed/" .. vcpkgTriplet)

  externalincludedirs { vcpkgInstalled .. "/include" }

  filter "configurations:Debug"
    defines { "DEBUG" }
    runtime "Debug"
    symbols "on"
    libdirs { vcpkgInstalled .. "/debug/lib" }
    links(debugLibraries)
  filter "configurations:Release"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "on"
    libdirs { vcpkgInstalled .. "/lib" }
    links(releaseLibraries)
  filter {}

  filter "system:windows"
    links {}
  filter "system:linux"
    links {
      "pthread"
    }
  filter {}

  vcpkg.enableVcpkgManifest()

  include "FiniteElementMethod/premake5"
