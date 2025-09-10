project "App"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++23"
  staticruntime "off"

  targetdir (OutputDir .. "/%{prj.name}")
  objdir (IntermediateDir .. "/%{prj.name}")

  files {
    "src/**.h",
    "src/**.cpp"
  }

  includedirs {
    "src",
  }

  filter "configurations:Debug"
    defines { "DEBUG" }
    runtime "Debug"
    symbols "On"

  filter "configurations:Release"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"
    symbols "On"
