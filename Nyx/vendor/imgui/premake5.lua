project "ImGui"
    kind "StaticLib"
    language "C++"
    
    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin/intermediates/" .. outputdir .. "/%{prj.name}")

    files
    {
        "imconfig.h",
        "imgui.h",
        "imgui.cpp",
        "imgui_draw.cpp",
        "imgui_internal.h",
        "imgui_widgets.cpp",
        "imstb_rectpack.h",
        "imstb_textedit.h",
        "imstb_truetype.h",
        "imgui_demo.cpp",
        "ImCurveEdit.cpp",
        "ImCurveEdit.h",
        "ImGradient.cpp",
        "ImGradient.h",
        "ImGuizmo.cpp",
        "ImGuizmo.h",
        "ImSequencer.cpp",
        "ImSequencer.h"
    }

    defines
    {
        "IMGUI_DEFINE_MATH_OPERATORS",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD",
        "_CRT_SECURE_NO_WARNINGS"
    }

	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "On"

    filter "configurations:Release"
        runtime "Release"
        optimize "On"

    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"