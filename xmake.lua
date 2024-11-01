set_xmakever("2.9.4")
add_rules("mode.debug", "mode.release")
set_allowedmodes("debug", "release")
set_allowedplats("windows")

-- Set C/C++ language version
set_languages("c17", "cxx20")

-- Build debug mode by default
set_defaultmode("debug")

add_links("d3d11","user32.lib", "dxgi", "dxguid", "comctl32.lib", "d3dcompiler.lib", "gdi32.lib", "kernel32.lib", "shell32.lib")

add_defines("UNICODE")

-- Project name and version
set_project("CravillacEngine")
set_version("0.0.1")

-- Enable all warnings and handle them as compilation errors
set_policy("build.warning", true)
set_warnings("all", "extra")

-- Enable link time optimization
if(is_mode("release")) then
    set_policy("build.optimization.lto", true)
end
-- Allow auto build before running
set_policy("run.autobuild", true)

-- Generate compile_commands.json on build_should_pas
add_rules("plugin.compile_commands.autoupdate")

-- Include xmake build scripts
includes("Scripts/Packages.lua")
includes("Scripts/Plugins/xmake.lua")

--Add build config specific macros (and set runtimes)
if is_mode("debug") then
    add_defines("CRAVILLAC_BUILD_DEBUG", "_DEBUG")
    set_runtimes("MDd")
elseif is_mode("release") then
    add_defines("CRAVILLAC_BUILD_RELEASE", "NDEBUG")
    set_runtimes("MD")
end

includes("**/xmake.lua")
