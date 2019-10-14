workspace "Nyx"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] = "Nyx/vendor/spdlog/include"
IncludeDir["glm"] =    "Nyx/vendor/glm"
IncludeDir["assimp"] = "Nyx/vendor/assimp/include"
IncludeDir["GLFW"] =   "Nyx/vendor/GLFW/include"
IncludeDir["glad"] =   "Nyx/vendor/glad/include"
IncludeDir["imgui"] =  "Nyx/vendor/imgui"

include "Nyx/vendor/GLFW"
include "Nyx/vendor/glad"
include "Nyx/vendor/imgui"

project "Nyx"
	location "Nyx"
	kind "StaticLib"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

	pchheader "NXpch.h"
	pchsource "Nyx/src/NXpch.cpp"	

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/stbimage/**.h",
		"%{prj.name}/vendor/stbimage/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}"
	}


	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines 
		{
			"NX_PLATFORM_WINDOWS",
			"GLFW_INCLUDE_NONE",
			"_CRT_SECURE_NO_WARNINGS"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		defines 
		{
			"NX_ENABLE_ASSERTS"
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/src",
		"Nyx/src",
		"Nyx/vendor",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}"
	}

	links 
	{ 
		"GLFW",
		"glad",
		"imgui",
		"Nyx",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		links 
		{ 
			"Nyx/vendor/assimp/Debug/assimp-vc142-mtd.lib"
		}

		postbuildcommands
		{
			("{COPY} ../Nyx/vendor/assimp/Debug/assimp-vc142-mtd.dll \"../bin/" .. outputdir .. "/Sandbox/\"")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		links 
		{ 
			"Nyx/vendor/assimp/Release/assimp-vc142-mt.lib"
		}

		postbuildcommands
		{
			("{COPY} ../Nyx/vendor/assimp/Release/assimp-vc142-mt.dll \"../bin/" .. outputdir .. "/Sandbox/\"")
		}