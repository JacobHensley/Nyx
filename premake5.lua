workspace "Nyx"
	architecture "x64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["spdlog"] =    "Nyx/vendor/spdlog/include"
IncludeDir["glm"] =       "Nyx/vendor/glm"
IncludeDir["assimp"] =    "Nyx/vendor/assimp/include"
IncludeDir["GLFW"] =      "Nyx/vendor/GLFW/include"
IncludeDir["glad"] =      "Nyx/vendor/glad/include"
IncludeDir["imgui"] =     "Nyx/vendor/imgui"
IncludeDir["sol2"] =      "Nyx/vendor/sol2/include"
IncludeDir["lua"] =       "Nyx/vendor/sol2/lua/src"
IncludeDir["SPVCross"] =  "Nyx/vendor/SPIRV-Cross"
IncludeDir["Shaderc"] =   "Nyx/vendor/shaderc/libshaderc/include"
IncludeDir["yamlCPP"] =   "Nyx/vendor/yaml-cpp/include"
IncludeDir["EnTT"] =      "Nyx/vendor/entt/include"

include "Nyx/vendor/GLFW"
include "Nyx/vendor/glad"
include "Nyx/vendor/imgui"
include "Nyx/vendor/sol2"
include "Nyx/vendor/SPIRV-Cross"
include "Nyx/vendor/yaml-cpp"

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
		"%{IncludeDir.assimp}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glad}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.sol2}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.SPVCross}",
		"%{IncludeDir.Shaderc}",
		"%{IncludeDir.yamlCPP}",
		"%{IncludeDir.EnTT}"
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
			("{COPY} %{cfg.buildtarget.relpath} \"../bin/" .. outputdir .. "/Editor/\"")
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

project "Editor"
	location "Editor"
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

	excludes 
	{ 
		"%{prj.name}/src/layers/ViewerLayer.h",
		"%{prj.name}/src/layers/ViewerLayer.cpp"
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
		"%{IncludeDir.imgui}",
		"%{IncludeDir.sol2}",
		"%{IncludeDir.lua}",
		"%{IncludeDir.SPVCross}",
		"%{IncludeDir.Shaderc}",
		"%{IncludeDir.yamlCPP}",
		"%{IncludeDir.EnTT}"
	}

	links 
	{ 
		"Nyx",
		"GLFW",
		"glad",
		"imgui",
		"sol2",
		"SPIRV-Cross",
		"yaml-cpp",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"

		defines 
		{
			"NX_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "On"

		links 
		{ 
			"Nyx/vendor/assimp/lib/assimp-vc141-mt.lib",
		--	"Nyx/vendor/SPIRV-Cross/build/Debug/spirv-cross-c-sharedd.lib",
			"Nyx/vendor/shaderc/build/libshaderc/Debug/shaderc_combined.lib"
		}

		postbuildcommands
		{
			("{COPY} ../Nyx/vendor/assimp/lib/assimp-vc141-mt.dll \"../bin/" .. outputdir .. "/Editor/\""),
		--	("{COPY} ../Nyx/vendor/SPIRV-Cross/build/Debug/spirv-cross-c-sharedd.dll \"../bin/" .. outputdir .. "/Editor/\"")
		}

	filter "configurations:Release"
		runtime "Release"
		optimize "On"

		links 
		{ 
			"Nyx/vendor/assimp/lib/assimp-vc141-mt.lib",
		--	"Nyx/vendor/SPIRV-Cross/build/Release/spirv-cross-c-shared.lib",
			"Nyx/vendor/shaderc/build/libshaderc/Release/shaderc_combined.lib"
		}

		postbuildcommands
		{
			("{COPY} ../Nyx/vendor/assimp/lib/assimp-vc141-mt.dll \"../bin/" .. outputdir .. "/Editor/\""),
		--	("{COPY} ../Nyx/vendor/SPIRV-Cross/build/Release/spirv-cross-c-shared.dll \"../bin/" .. outputdir .. "/Editor/\"")
		}