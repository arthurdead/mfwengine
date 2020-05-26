#ifndef __MFW_DISABLE_ASMJIT

#ifndef __MFW_PRIVATE_CORE_ASM_BUILDER_H
#define __MFW_PRIVATE_CORE_ASM_BUILDER_H

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
	MFW_WARNING_PUSH()
	MFW_WARNING_DISABLE("-Wgnu-anonymous-struct")
	MFW_WARNING_DISABLE("-Wnested-anon-types")
#endif
#include <asmjit/asmjit.h>
#if MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
	MFW_WARNING_POP()
#endif
#pragma pop_macro("new")

#include <public/mfw/core/rttr_interface.hpp>

namespace mfw::core
{
	class asm_builder
	{
	public:
		static asm_builder &instance();

		asm_builder();

		bool call_function(const func_info &info, const asmjit::FuncSignature &signature, const vector<type_holder> &args, type_holder &result);

		static asmjit::Type::Id to_asmjit_type(const type_info &info);

	private:
		struct base_register_info_t
		{
			asmjit::x86::Mem memory{};
			bool use_memory{false};
		};

		struct int_register_info_t : base_register_info_t
		{
			const asmjit::x86::Gp *register_{nullptr};
		};

		struct float_register_info_t : base_register_info_t
		{
			const asmjit::x86::Xmm *register_{nullptr};
		};

		static void get_int_arg_reg(int_register_info_t &reg, const type_info &info, size_t position);
		static void get_int_ret_reg(int_register_info_t &reg, const type_info &info);

		static void get_float_arg_reg(float_register_info_t &reg, const type_info &info, size_t position);
		static void get_float_ret_reg(float_register_info_t &reg, const type_info &info);

		asmjit::JitRuntime runtime{};
		asmjit::FuncSignatureBuilder emptysignature{};

		class error_handler : public asmjit::ErrorHandler
		{
		private:
			void handleError(asmjit::Error err, const char *message, asmjit::BaseEmitter *origin) override;
		};
		error_handler errhandler{};
	};
}

#endif

#endif