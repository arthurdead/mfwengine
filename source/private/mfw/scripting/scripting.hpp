#ifndef __MFW_SCRIPTING_SCRIPTING_H
#define __MFW_SCRIPTING_SCRIPTING_H

#pragma once

#include <public/mfw/scripting/scripting_interface.hpp>
#include <public/mfw/core/globals.hpp>

#if MFW_COMPILER == MFW_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4371 4191)
#endif
#define module module_
#include <angelscript.h>
#include <as_jit.h>
#undef module
#if MFW_COMPILER == MFW_COMPILER_MSVC
#pragma warning(pop)
#endif

namespace mfw::scripting
{
	class scripting final : public interfaces::scripting, core::global_initializer
	{
	public:
		static scripting &instance();

	private:
		core::exit_code initialize() override;
		core::exit_code update() override { return core::exit_code::success; }
		core::exit_code shutdown() override;

		univalue execute_string(const u16string_view &str) const override;

		static void script_message(const asSMessageInfo *info, void *userptr);

		asIScriptEngine *engine{nullptr};
		unique_ptr<asCJITCompiler> jit_compiler{};
	};
};

#endif