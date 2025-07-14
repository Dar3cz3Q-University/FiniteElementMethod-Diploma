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

  include "FiniteElementMethod/app"
