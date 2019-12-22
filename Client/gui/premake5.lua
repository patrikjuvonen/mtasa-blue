project "GUI"
	language "C++"
	kind "SharedLib"
	targetname "cgui"
	targetdir(buildpath("mta"))

	filter "system:windows"
		includedirs { "../../vendor/sparsehash/src/windows" }

	filter {}
		includedirs {
			"../sdk",
			"../../vendor/cegui/include",
			"../../vendor/sparsehash/src"
		}

	pchheader "StdInc.h"
	pchsource "StdInc.cpp"

	defines {
		"_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING"
	}

	links {
		"CEGUI",
		"d3dx9.lib",
		"dxerr.lib"
	}

	vpaths {
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.cpp",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"*.h",
		"*.cpp"
	}
	
	filter "architecture:x64"
		flags { "ExcludeFromBuild" }

	filter "system:not windows"
		flags { "ExcludeFromBuild" }
