#include <private/mfw/scripting/scripting.hpp>
#include <public/mfw/core/logging_interface.hpp>

#if MFW_COMPILER == MFW_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4464)
#endif
#include <datetime/datetime.h>
#include <scriptany/scriptany.h>
#include <scriptarray/scriptarray.h>
#include <scriptdictionary/scriptdictionary.h>
#include <scriptfile/scriptfile.h>
#include <scriptfile/scriptfilesystem.h>
#include <scriptgrid/scriptgrid.h>
#include <scripthandle/scripthandle.h>
#include <scripthelper/scripthelper.h>
#include <scriptstdstring/scriptstdstring.h>
#include <weakref/weakref.h>
#if MFW_COMPILER == MFW_COMPILER_MSVC
#pragma warning(pop)
#endif

namespace mfw::scripting
{
	MFW_DECLARE_LOG_CONTEXT(log_scripting, u"scripting"_p);

	MFW_DECLARE_GLOBAL_ALLOCATOR(scripting, ::mfw::scripting::scripting);

	::mfw::scripting::scripting &::mfw::scripting::scripting::instance() {
		return __scripting_global_allocator.instance();
	}
	MFW_SCRIPTING_API interfaces::scripting & MFW_SCRIPTING_CALL interfaces::scripting::instance()
		{ return ::mfw::scripting::scripting::instance(); }

	void mfw::scripting::scripting::script_message(const asSMessageInfo *info, void *userptr)
	{
		u16string message{};
		convert(rcast<const char8_t *>(info->message), message);

		u16string section{};
		convert(rcast<const char8_t *>(info->section), section);

		if(info->type == asMSGTYPE_ERROR) {
			log_scripting.error(u"{} ({}, {}) : {}"_sv, section, info->row, info->col, message);
		} else if(info->type == asMSGTYPE_WARNING) {
			log_scripting.warning(u"{} ({}, {}) : {}"_sv, section, info->row, info->col, message);
		} else if(info->type == asMSGTYPE_INFORMATION) {
			log_scripting.info(u"{} ({}, {}) : {}"_sv, section, info->row, info->col, message);
		}
	}

	core::exit_code mfw::scripting::scripting::initialize()
	{
		engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		engine->SetMessageCallback(asFUNCTION(script_message), this, asCALL_CDECL);

		jit_compiler.reset(new asCJITCompiler(
		#if MFW_CONFIGURATION != MFW_CONFIGURATION_DEBUG
			JIT_NO_SUSPEND|
			JIT_SYSCALL_FPU_NORESET|
			JIT_SYSCALL_NO_ERRORS|
			JIT_FAST_REFCOUNT|
			JIT_ALLOC_SIMPLE
		#else
			0
		#endif
		));

		engine->SetEngineProperty(asEP_INCLUDE_JIT_INSTRUCTIONS, 1);
		engine->SetEngineProperty(asEP_USE_CHARACTER_LITERALS, 1);
		engine->SetEngineProperty(asEP_ALLOW_MULTILINE_STRINGS, 1);
		engine->SetEngineProperty(asEP_REQUIRE_ENUM_SCOPE, 1);
		engine->SetEngineProperty(asEP_SCRIPT_SCANNER, 1);
		engine->SetEngineProperty(asEP_STRING_ENCODING, 1);
		engine->SetEngineProperty(asEP_ALLOW_UNICODE_IDENTIFIERS, 1);
		engine->SetEngineProperty(asEP_AUTO_GARBAGE_COLLECT, 1);
		engine->SetEngineProperty(asEP_ALWAYS_IMPL_DEFAULT_CONSTRUCT, 1);
		engine->SetEngineProperty(asEP_INIT_GLOBAL_VARS_AFTER_BUILD, 1);
		engine->SetEngineProperty(asEP_DISABLE_INTEGER_DIVISION, 1);
		engine->SetEngineProperty(asEP_DISALLOW_VALUE_ASSIGN_FOR_REF_TYPE, 1);
		engine->SetEngineProperty(asEP_ALLOW_IMPLICIT_HANDLE_TYPES, 1);
		#if MFW_CONFIGURATION != MFW_CONFIGURATION_DEBUG
		engine->SetEngineProperty(asEP_OPTIMIZE_BYTECODE, 1);
		engine->SetEngineProperty(asEP_COMPILER_WARNINGS, 0);
		#else
		engine->SetEngineProperty(asEP_OPTIMIZE_BYTECODE, 0);
		engine->SetEngineProperty(asEP_COMPILER_WARNINGS, 2);
		#endif
		engine->SetJITCompiler(jit_compiler.get());

		RegisterStdString(engine);
		RegisterScriptArray(engine, true);
		RegisterStdStringUtils(engine);
		RegisterScriptDateTime(engine);
		RegisterScriptAny(engine);
		RegisterScriptGrid(engine);
		RegisterScriptHandle(engine);
		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		RegisterExceptionRoutines(engine);
		#endif
		RegisterScriptWeakRef(engine);
		RegisterScriptDictionary(engine);
		RegisterScriptFile(engine);
		RegisterScriptFileSystem(engine);

		return core::exit_code::success;
	}

	univalue mfw::scripting::scripting::execute_string(const u16string_view &str) const
	{
		jit_compiler->finalizePages();

		u8string nstr{};
		convert(str, nstr);

		float64_t ret{0.0};
		ExecuteString(engine, rcast<const char *>(nstr.c_str()), &ret, asTYPEID_DOUBLE);

		univalue val{};
		val.set(scast<float32_t>(ret));
		return val;
	}

	core::exit_code mfw::scripting::scripting::shutdown()
	{
		if(engine) {
			engine->Release();
		}

		return core::exit_code::success;
	}
};