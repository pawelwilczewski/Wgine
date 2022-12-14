workspace "Wgine"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	startproject "Sandbox"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Includes relative to root
IncludeDir = {}
IncludeDir["GLFW"] = "Wgine/thirdparty/GLFW/include"
IncludeDir["Glad"] = "Wgine/thirdparty/Glad/include"
IncludeDir["ImGui"] = "Wgine/thirdparty/ImGui"
IncludeDir["glm"] = "Wgine/thirdparty/glm"
IncludeDir["stb_image"] = "Wgine/thirdparty/stb_image"
IncludeDir["spdlog"] = "Wgine/thirdparty/spdlog/include"
IncludeDir["cgltf"] = "Wgine/thirdparty/cgltf"

group "Dependencies"
	include "Wgine/thirdparty/GLFW"
	include "Wgine/thirdparty/Glad"
	include "Wgine/thirdparty/ImGui"

group ""
	project "Wgine"
		location "Wgine"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

		pchheader "WginePCH.h"
		pchsource "Wgine/src/WginePCH.cpp"

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
			"%{prj.name}/thirdparty/stb_image/**.h",
			"%{prj.name}/thirdparty/stb_image/**.cpp",
			"%{prj.name}/thirdparty/glm/glm/**.hpp",
			"%{prj.name}/thirdparty/glm/glm/**.inl",
			"%{prj.name}/thirdparty/cgltf/**.h",
			"%{prj.name}/thirdparty/cgltf/**.cpp"
		}

		includedirs
		{
			"%{prj.name}/src",
			"%{IncludeDir.spdlog}",
			"%{IncludeDir.GLFW}",
			"%{IncludeDir.Glad}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.glm}",
			"%{IncludeDir.stb_image}",
			"%{IncludeDir.cgltf}"
		}

		links
		{
			"GLFW",
			"Glad",
			"ImGui",
			"opengl32.lib"
		}

		defines {
			"_CRT_SECURE_NO_WARNINGS"
		}

		filter "system:windows"
			systemversion "latest"

			defines {
				"WGINE_PLATFORM_WINDOWS",
				"GLFW_INCLUDE_NONE",
			}

		filter "configurations:Debug"
			defines "WGINE_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "WGINE_RELEASE"
			runtime "Release"
			optimize "on"

		filter "configurations:Dist"
			defines "WGINE_DIST"
			runtime "Release"
			optimize "on"
			symbols "off"


	project "Sandbox"
		location "Sandbox"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "on"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("intermediate/" .. outputdir .. "/%{prj.name}")

		files
		{
			"%{prj.name}/src/**.h",
			"%{prj.name}/src/**.cpp",
		}

		includedirs
		{
			"Wgine/src",
			"%{IncludeDir.spdlog}",
			"%{IncludeDir.ImGui}",
			"%{IncludeDir.glm}"
		}

		links
		{
			"Wgine",
			"ImGui"
		}

		filter "system:windows"
			systemversion "latest"
			defines {
				"WGINE_PLATFORM_WINDOWS"
			}

		filter "configurations:Debug"
			defines "WGINE_DEBUG"
			symbols "on"

		filter "configurations:Release"
			defines "WGINE_RELEASE"
			optimize "on"

		filter "configurations:Dist"
			defines "WGINE_DIST"
			optimize "on"
			symbols "off"
