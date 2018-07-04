project "bass"
	language "C"
	kind "StaticLib"
	targetname "bass"
	
	disablewarnings {
		"4244", -- warning C4244: '=': conversion from '?' to '?', possible loss of data
		"4305", -- warning C4305: 'initializing': truncation from '?' to '?'
		"4018" -- warning C4018: '<': signed/unsigned mismatch
	}

	includedirs {
		"."
	}
	
	vpaths { 
		["Headers/*"] = "**.h",
		["Sources/*"] = "**.c",
		["*"] = "premake5.lua"
	}

	files {
		"premake5.lua",
		"**.h",
		"**.c"
	}

	filter "architecture:x64"
		flags { "ExcludeFromBuild" }
	filter "system:not windows"
		flags { "ExcludeFromBuild" }