project "CEGUI"
	language "C++"
	kind "StaticLib"
	targetname "CEGUI"
	
	cppdialect "C++14"
	
	includedirs {
		"include",
		"dependencies/include"
	}
	
	defines {
		"CEGUIBASE_EXPORTS",
		"XERCES_TMPLSINC"
	}
	
	vpaths {
		["Headers/*"] = {"**.h", "**.hpp"},
		["Sources/*"] = "src/**.cpp",
		["*"] = "premake5.lua"
	}
	
	files {
		"premake5.lua",
		"src/**.cpp",
		"include/**.h",
		"dependencies/include/**.hpp",
		"dependencies/include/**.h"
	}
	
	excludes {
		"src/IconvStringTranscoder.cpp",
		"src/minibidi.cpp",
		"src/MinizipResourceProvider.cpp",
		"src/PCRERegexMatcher.cpp",
		"src/implementations/**"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }
