project "yaml-cpp"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.cpp",
        "src/**.h",
        "include/**.h"
    }

    includedirs
    {
        "src",
        "include"
    }
    
    filter "system:windows"
        systemversion "latest"

        defines 
        { 
            "WIN32",
            "_WINDOWS"
        }

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"