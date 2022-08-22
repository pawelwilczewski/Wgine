workspace "Wgine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Includes relative to root
IncludeDir = {}
IncludeDir["GLFW"] = "Wgine/thirdparty/GLFW/include"

include "Wgine/thirdparty/GLFW"

project "Wgine"
	location "Wgine"
	kind "SharedLib"
	language "C++"
	staticruntime "off"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

	pchheader "WginePCH.h"
	pchsource "Wgine/src/WginePCH.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/thirdparty/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"WGINE_PLATFORM_WINDOWS",
			"WGINE_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "WGINE_DEBUG"
		symbols "On"
		runtime "Debug"

	filter "configurations:Release"
		defines "WGINE_RELEASE"
		optimize "On"
		runtime "Release"

	filter "configurations:Dist"
		defines "WGINE_DIST"
		optimize "On"
		runtime "Release"


project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
		"Wgine/thirdparty/spdlog/include",
		"Wgine/src"
	}

	links
	{
		"Wgine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines {
			"WGINE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "WGINE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "WGINE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "WGINE_DIST"
		optimize "On"
