workspace "Punch"
    architecture "x64"
    configurations
    {
	 	"Debug",
	 	"Release"
    }
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "Core"
    location "%{prj.name}"
    kind "WindowedApp"
    language "C++"
	targetname "Punch"
    targetdir ("bin/".. outputdir)
    objdir ("%{prj.name}/int/".. outputdir)

    files
    {
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}
	
	includedirs
	{
		"%{prj.name}/pch",
        "%{prj.name}/include",
        "%{prj.name}/include/ImGui"
	}
    pchheader "pch.h"
    pchsource "%{prj.name}/pch/pch.cpp"

	libdirs "%{prj.name}/lib"
	
	links
	{
		"opengl32",
		"winmm",
		"gdi32",
		"Shell32"
	}
	
	defines { "SFML_STATIC", "YAML_CPP_STATIC_DEFINE" }
		

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"
		defines { "WIN32" }

    filter "configurations:Debug"
        defines { "_DEBUG", "_CONSOLE" }
		kind "ConsoleApp"
        symbols "On"
	    links
	    {
			"sfml-system-s-d.lib",
			"sfml-graphics-s-d.lib",
			"sfml-window-s-d.lib",
			"imgui-sfml-s-d.lib",
			"yaml-cpp-d.lib"
	    }
    
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
	    links
	    {
			"sfml-system-s.lib",
			"sfml-graphics-s.lib",
			"sfml-window-s.lib",
			"imgui-sfml-s.lib",
			"yaml-cpp.lib"
		}