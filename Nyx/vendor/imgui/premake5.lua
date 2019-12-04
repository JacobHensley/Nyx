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
        --  ImGuizmo
        "ImSequencer.h",
        "ImSequencer.cpp",
        "ImGuizmo.h",
        "ImGuizmo.cpp",
        "ImGradient.h",
        "ImGradient.cpp",
        "ImCurveEdit.h",
        "ImCurveEdit.cpp"
    }

    defines 
    {
        "IMGUI_IMPL_OPENGL_LOADER_GLAD"
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