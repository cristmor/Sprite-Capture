workspace("SpriteCapture")
configurations({ "Debug", "Release" })

project("SpriteCapture")
kind("ConsoleApp")
language("C++")
cppdialect("C++17")

targetdir("bin/%{cfg.buildcfg}")
objdir("bin/")

files({
	"src/**.h",
	"src/**.cpp",
	"deps/imgui/**.cpp",
	"deps/imgui/**.h",
	"deps/imgui-sfml/**.cpp",
	"deps/imgui-sfml/**.h",
})
includedirs({ "src/", "deps/", "deps/imgui/", "deps/imgui-sfml" })

links({ "GL" })
links({ "GLU", "sfml-graphics", "sfml-window", "sfml-system" })

filter("configurations:Debug")
defines({ "DEBUG" })
symbols("On")

filter("configurations:Release")
defines({ "NDEBUG" })
optimize("On")
