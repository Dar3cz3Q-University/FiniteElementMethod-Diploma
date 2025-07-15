project "App"
  kind "ConsoleApp"
  language "C++"
  cppdialect "C++20"
  staticruntime "off"

  targetdir (OutputDir .. "/%{prj.name}")
  objdir (IntermediateDir .. "/%{prj.name}")

  pchheader "pch.h"
  pchsource "src/pch.cpp"

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
