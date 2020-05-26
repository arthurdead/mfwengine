function setup_files(name)
	files{
		path.join(sourcefolder, "public/mfw/" .. name .. "/**"),
		path.join(sourcefolder, "private/mfw/" .. name .. "/**"),
	}
	vpaths{
		["private/*"] = path.join(sourcefolder, "private/mfw/" .. name .. "/**"),
		["public/" .. name .. "/*"] = path.join(sourcefolder, "public/mfw/" .. name .. "/**"),
	}
end

function depends(name)
	dependson(name)
	
	if(type(name) ~= "table") then
		name = {name}
	end
	
	for k,value in pairs(name) do
		defines("MFW_DEPENDS_ON_" .. value:upper())
		files{
			path.join(sourcefolder, "public/mfw/" .. value .. "/**"),
		}
		vpaths{
			["public/" .. value .. "/*"] = path.join(sourcefolder, "public/mfw/" .. value .. "/**"),
		}
	end
end

function linksanddepends(name)
	depends(name)
	links(name)
end

function setup_project(name)
	project(name)
		location "%{wks.location}/%{prj.name}"
	
		setup_files(name)
		
		defines{
			"MFW_BUILDING_" .. name:upper(),
			"MFW_DEPENDS_ON_" .. name:upper()
		}
	
		include(name .. ".lua")
		
		if name ~= "stl" and
			name ~= "core_implib"
			then
			linksanddepends "stl"
		end
		
		if name ~= "core" and
			name ~= "core_implib" and
			name ~= "stl" and
			name ~= "launcher" and
			name ~= "codetest"
			then
			linksanddepends "core"
		end
		
		removedependson(name)
		removelinks(name)
		
		filter "kind:ConsoleApp"
			defines("MFW_" .. name:upper() .. "_BUILD=MFW_BUILD_EXECUTABLE")
		filter "kind:WindowedApp"
			defines("MFW_" .. name:upper() .. "_BUILD=MFW_BUILD_EXECUTABLE_WINDOWS")
		filter "kind:SharedLib"
			defines("MFW_" .. name:upper() .. "_BUILD=MFW_BUILD_SHARED")
		filter "kind:StaticLib"
			defines("MFW_" .. name:upper() .. "_BUILD=MFW_BUILD_STATIC")
		filter{}
	project "*"
end

workspace "mfwengine"
	location("generated/" .. _ACTION)
	
	rootfolder = path.getabsolute("../..")
	sourcefolder = path.join(rootfolder,"source")
	packagefolder = path.join(rootfolder,"package")
	thirdpartyfolder = path.join(rootfolder,"thirdparty")
	
	objdir "%{wks.location}/%{prj.name}/obj/%{cfg.system}_%{cfg.toolset}_%{cfg.platform}_%{cfg.buildcfg}"
	implibdir "%{wks.location}/%{prj.name}/obj/%{cfg.system}_%{cfg.toolset}_%{cfg.platform}_%{cfg.buildcfg}"
	targetdir "%{wks.location}/%{prj.name}/obj/%{cfg.system}_%{cfg.toolset}_%{cfg.platform}_%{cfg.buildcfg}"
	debugdir(packagefolder)
	
	filter "kind:ConsoleApp or WindowedApp"
		debugcommand(path.join(packagefolder,"%{cfg.linktarget.name}"))
		targetsuffix "_%{cfg.system}_%{cfg.platform}_%{cfg.buildcfg}"
		postbuildcommands{
			string.format("{COPY} \"%%{wks.location}/%%{prj.name}/%%{cfg.linktarget.abspath}\" \"%s/%%{cfg.linktarget.name}\"",packagefolder),
		}
		postbuildmessage "Copying files..."
	filter "kind:SharedLib"
		postbuildcommands{
			string.format("{MKDIR} \"%s/core/bin/%%{cfg.system}_%%{cfg.platform}_%%{cfg.buildcfg}\"",packagefolder),
			string.format("{COPY} \"%%{wks.location}/%%{prj.name}/%%{cfg.linktarget.abspath}\" \"%s/core/bin/%%{cfg.system}_%%{cfg.platform}_%%{cfg.buildcfg}/%%{cfg.linktarget.name}\"",packagefolder),
		}
		postbuildmessage "Copying files..."
	filter{}

	configurations{"debug", "release"}
	
	filter "system:Windows or Linux"
		platforms{"x86_64","x86"}
		-- platforms "x86_64"
	filter "system:Android"
		platforms "ARM"
	
	filter "platforms:x86_64"
		architecture "x86_64"
	filter "platforms:x86"
		architecture "x86"
	filter "platforms:ARM"
		architecture "ARM"
	filter{}
	
	preferredtoolarchitecture "x86_64"
	defaultplatform "x86_64"
	
	editorintegration "On"
	
	language "C++"
	cdialect "C11"
	cppdialect "c++17"
	
	filter "system:Windows"
		callingconvention "StdCall"
		toolset "msc"
	filter "system:not Windows"
		callingconvention "Cdecl"
		toolset "gcc"
		-- toolset "clang"
	
	filter "platforms:x86_64 or x86"
		vectorextensions "SSE4.1"
		endian "Little"
	filter "platforms:ARM"
		vectorextensions "NEON"
		endian "Big"
	
	filter "platforms:x86_64"
		structmemberalign(8)
	filter "platforms:x86 or ARM"
		structmemberalign(4)
		
	filter "toolset:gcc"
		debugger "GDB"
	filter "toolset:clang"
		debugger "LLDB"
	filter "toolset:msc"
		debugger "Default"
		
	filter "toolset:gcc or clang"
		--buildoptions{"-fchar8_t"}
	filter{}
	
	largeaddressaware "On"
	nativewchar "On"
	clr "Off"
	atl "Off"
	linkgroups "On"
	resourcegenerator "internal"
	locale "0x0409"
	debugformat "Default"
	debuggertype "NativeOnly"
	compileas "Default"
	debugextendedprotocol "On"
	unsignedchar "Off"
	pic "On"
	targetprefix ""
	runpathdirs ""
	
	flags{
		"MultiProcessorCompile",
	}
	functionlevellinking "On"
	--[[isaextensions{
		"MOVBE",
		"POPCNT",
		"PCLMUL",
		"LZCNT",
		"BMI",
		"BMI2",
		"F16C",
		"AES",
		"FMA",
		"FMA4",
		"RDRND",
	}]]
	
	characterset "Unicode"
	defines "_UNICODE"

	filter{"configurations:Debug","toolset:gcc"}
		strictaliasing "Level3"
	filter{"configurations:Debug","toolset:clang"}
		strictaliasing "Level2"
	filter "configurations:Debug"
		symbols "Full"
		optimize "Off"
		defines "_DEBUG"
		warnings "Extra"
		omitframepointer "Off"
		rtti "On"
		exceptionhandling "SEH"
		editandcontinue "On"
		floatingpoint "Strict"
		floatingpointexceptions "On"
		fpu "Software"
		intrinsics "Off"
		inlining "Disabled"
		runtime "Debug"
		stringpooling "Off"
		visibility "Default"
		inlinesvisibility "Default"
		assemblydebug "On"
		staticruntime "Off"
		flags{
			"FatalCompileWarnings",
			"FatalLinkWarnings",
			"FatalWarnings",
			"Maps",
			"ShadowedVariables",
			"UndefinedIdentifiers",
		}

	filter "configurations:Release"
		symbols "Off"
		optimize "Speed"
		defines "_NDEBUG"
		warnings "Off"
		omitframepointer "On"
		rtti "Off"
		exceptionhandling "On"
		editandcontinue "Off"
		floatingpoint "Fast"
		floatingpointexceptions "Off"
		fpu "Hardware"
		intrinsics "On"
		inlining "Auto"
		runtime "Release"
		stringpooling "On"
		strictaliasing "Off"
		visibility "Hidden"
		inlinesvisibility "Hidden"
		assemblydebug "Off"
		staticruntime "On"
		flags{
			"LinkTimeOptimization",
			"No64BitChecks",
			"NoBufferSecurityCheck",
			"NoManifest",
			"NoRuntimeChecks",
		}
	
	filter "kind:ConsoleApp"
		defines "_CONSOLE"
	filter "kind:WindowedApp"
		defines "_WINDOWS"
	filter "kind:SharedLib"
		defines "_SHARED"
	filter "kind:StaticLib"
		defines "_LIB"
	filter{}
	
	includedirs{
		sourcefolder,
		thirdpartyfolder,
	}
	
	defines{
		"MFW_LAUNCHER_BUILD=MFW_BUILD_EXECUTABLE",
		"MFW_BUILDER_BUILD=MFW_BUILD_EXECUTABLE",
		"MFW_ENGINE_BUILD=MFW_BUILD_SHARED",
		"MFW_RENDERER_BUILD=MFW_BUILD_SHARED",
		"MFW_SCRIPTING_BUILD=MFW_BUILD_SHARED",
		--"MFW_CORE_BUILD=MFW_BUILD_SHARED",
		"MFW_CORE_BUILD=MFW_BUILD_STATIC",
		"MFW_STL_BUILD=MFW_BUILD_STATIC",
		"_GNU_SOURCE",
		--"_NEW",
	}
	
	files{
		path.join(sourcefolder,"public/mfw/pch.cpp"),
		path.join(sourcefolder,"public/mfw/pch*.hpp"),
	}
	
	vpaths{
		["public"] = path.join(sourcefolder,"public/mfw/pch*.*"),
	}
	
	pchheader "public/mfw/pch.hpp"
	pchsource(path.join(sourcefolder,"public/mfw/pch.cpp"))
	forceincludes(path.join(sourcefolder,"public/mfw/pch.hpp"))
	
	warnings "Off"
	
	setup_project "stl"
	setup_project "core"
	--setup_project "core_implib"
	--setup_project "builder"
	--setup_project "renderer"
	--setup_project "engine"
	--setup_project "launcher"
	
	--setup_project "scripting"
	setup_project "codetest"
	
	startproject "codetest"
	--startproject "launcher"
	--startproject "builder"
