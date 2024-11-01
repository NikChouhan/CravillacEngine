target("CravillacSource")

    set_default(false)
    -- Set the target kind
    set_kind("static")
    -- Set the target group
    set_group("Cravillac")
    -- Add the required packages
    add_packages("directxmath")

    -- Add the include directories
    add_includedirs("..")

    -- Add the source files
    add_files("Core/**.cpp|Editor/**.cpp|Ext/Math/**.cpp|ImGui/**.cpp|Renderer/**.cpp")
    add_headerfiles("Core/**.h|Editor/**.h|Ext/Math/**.h|ImGui/**.h|Renderer/**.h")
    
target_end()