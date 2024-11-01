target("CravillacSource")

    set_default(false)
    -- Set the target kind
    set_kind("static")
    -- Set the target group
    set_group("Cravillac")
    -- Add the required packages

    -- Add the include directories
    add_includedirs("..")

    -- Add the source files
    add_files("Core/**.cpp", "Ext/Math/SimpleMath.cpp")
    add_headerfiles("Core/**.h", "Ext/Math/SimpleMath.h")
    
target_end()