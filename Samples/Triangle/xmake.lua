target("Triangle")
    set_default(true)
    set_kind("binary")
    add_files("**.cpp")
    add_includedirs("..", "$(projectdir)")
    set_group("Samples")
    add_deps("CravillacSource")
    add_links("d3d11","user32.lib", "dxgi", "dxguid", "comctl32.lib", "d3dcompiler.lib", "gdi32.lib", "kernel32.lib", "shell32.lib")

target_end()