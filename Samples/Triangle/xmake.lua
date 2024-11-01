target("Triangle")
    set_default(true)
    set_kind("binary")
    add_files("**.cpp")
    add_includedirs("..", "$(projectdir)")
    set_group("Samples")
    add_deps("CravillacSource")

target_end()