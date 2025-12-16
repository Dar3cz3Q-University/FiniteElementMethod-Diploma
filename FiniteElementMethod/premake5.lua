project "App"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++23"

  defines {
    "EIGEN_USE_MKL_ALL",
    "MKL_DIRECT_CALL"
  }

  targetdir (OutputDir .. "/%{prj.name}")
  objdir (IntermediateDir .. "/%{prj.name}")

  files {
    "src/**.h",
    "src/**.cpp"
  }

  includedirs {
    "src",
  }

  filter "system:windows"
    links { 
      "mkl_intel_lp64",
      "mkl_intel_thread",
      "mkl_core",
      "libiomp5md"
    }
  filter "system:linux"
    links {
      "mkl_intel_lp64",
      "mkl_gnu_thread",
      "mkl_core",
      "gomp",
      "pthread",
      "m",
      "dl"
    }
  filter {}
