project "App"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++23"

  defines {
    "EIGEN_USE_THREADS"
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
