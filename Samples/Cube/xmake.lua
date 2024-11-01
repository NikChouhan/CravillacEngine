target("Cube")
    set_default(true)
    set_kind("binary")
    add_files("**.cpp")
    set_group("Samples")
    add_includedirs("..", "$(projectdir)")

    add_deps("CravillacSource")

target_end()