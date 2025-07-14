project "App"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"

  targetdir (OutputDir .. "/%{prj.name}")
  objdir (IntermediateDir .. "/%{prj.name}")

  files {
    "src/**.h",
    "src/**.cpp"
  }
