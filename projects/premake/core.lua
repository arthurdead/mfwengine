--kind "SharedLib"
kind "StaticLib"

defines{
	"__MFW_DISABLE_ASMJIT"
}

dependson "core_implib"

filter "kind:SharedLib"
	core_implibfolder = path.join(sourcefolder,"private/mfw/core_implib")

	postbuildcommands{
		"{RMDIR} \"" .. core_implibfolder .. "\"",
		"{MKDIR} \"" .. core_implibfolder .. "\"",
		"{CHDIR} \"" .. core_implibfolder .. "\"",
		"python \"" .. path.join(thirdpartyfolder,"Implib.so/implib-gen.py") .. "\" \"%{cfg.buildtarget.abspath}\" --no-dlopen --no-lazy-load -o \"" .. core_implibfolder .. "\"",
		--"g++ \"" .. path.join(core_implibfolder,"core.so.init.c") .. "\" \"" .. path.join(core_implibfolder,"core.so.tramp.S") .. "\" -D_GNU_SOURCE -ldl -fPIC -c",
	}

filter "configurations:Debug"
	links "iberty"
