project "App"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++23"

  defines {
    "EIGEN_USE_MKL_ALL"
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
  
  filter {}

  filter "system:windows"
    links { 
      "mkl_intel_lp64",
      "mkl_sequential",
      "mkl_core"
    }
  filter "system:linux"
    links {
      "mkl_intel_lp64",
      "mkl_sequential",
      "mkl_core",
      "pthread",
      "m",
      "dl"
    }
  
  filter {}
