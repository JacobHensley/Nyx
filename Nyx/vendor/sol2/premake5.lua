project "sol2"
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

    files
    {
        "lua/src/**.c"
    }

    excludes 
    {
        "lua/src/luac.c",
        "lua/src/lua.c"
    }

    filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"