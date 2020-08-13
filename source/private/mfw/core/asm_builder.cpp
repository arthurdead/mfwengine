#ifdef __MFW_USE_ASMJIT

#include <private/mfw/core/asm_builder.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_asm_builder, u8"core/asm_builder"_p);

	MFW_DECLARE_GLOBAL_ALLOCATOR(asmbuilder, asm_builder);

	asm_builder &asm_builder::instance() {
		return __asmbuilder_global_allocator.instance();
	}

	void asm_builder::get_int_ret_reg(int_register_info_t &reg, const type_info &info)
	{
		if(info.is_int8()) {
			reg.register_ = &asmjit::x86::al;
		} else if(info.is_int16() || info.is_int32()) {
			reg.register_ = &asmjit::x86::eax;
		} else if(info.is_int64()) {
			reg.register_ = &asmjit::x86::rax;
		} else if(info.is_ptr_like() || info.is_any_reference()) {
			reg.register_ = &asmjit::x86::rax;
		}
	}

	void asm_builder::get_int_arg_reg(int_register_info_t &reg, const type_info &info, size_t position)
	{
		if(position < 4) {
			if(info.is_ptr_like() || info.is_any_reference()) {
				if(position == 0) {
					reg.register_ = &asmjit::x86::edx;
				} else if(position == 1) {
					reg.register_ = &asmjit::x86::rdx;
				} else if(position == 2) {
					reg.register_ = &asmjit::x86::r8;
				} else if(position == 3) {
					reg.register_ = &asmjit::x86::r9;
				}
			} else if(info.is_int8()) {
				if(position == 0) {
					reg.register_ = &asmjit::x86::cl;
				} else if(position == 1) {
					reg.register_ = &asmjit::x86::dl;
				} else if(position == 2) {
					reg.register_ = &asmjit::x86::r8b;
				} else if(position == 3) {
					reg.register_ = &asmjit::x86::r9b;
				}
			} else if(info.is_int16()) {
				if(position == 0) {
					reg.register_ = &asmjit::x86::cx;
				} else if(position == 1) {
					reg.register_ = &asmjit::x86::dx;
				} else if(position == 2) {
					reg.register_ = &asmjit::x86::r8w;
				} else if(position == 3) {
					reg.register_ = &asmjit::x86::r9w;
				}
			} else if(info.is_int32() || info.is_int64()) {
				if(position == 0) {
					reg.register_ = &asmjit::x86::ecx;
				} else if(position == 1) {
					reg.register_ = &asmjit::x86::edx;
				} else if(position == 2) {
					reg.register_ = &asmjit::x86::r8d;
				} else if(position == 3) {
					reg.register_ = &asmjit::x86::r9d;
				}
			}
		} else {
			int32_t offset{static_cast<int32_t>(((position + 1) * 8) - 8)};
			if(info.is_ptr_like() || info.is_any_reference()) {
				reg.register_ = &asmjit::x86::rax;
				reg.memory = asmjit::x86::qword_ptr(asmjit::x86::rsp, offset);
				reg.use_memory = true;
			} else if(info.is_int8()) {
				reg.memory = asmjit::x86::byte_ptr(asmjit::x86::rsp, offset);
			} else if(info.is_int16()) {
				reg.memory = asmjit::x86::word_ptr(asmjit::x86::rsp, offset);
			} else if(info.is_int32()) {
				reg.memory = asmjit::x86::dword_ptr(asmjit::x86::rsp, offset);
			} else if(info.is_int64()) {
				reg.memory = asmjit::x86::qword_ptr(asmjit::x86::rsp, offset);
			}
		}
	}

	void asm_builder::get_float_ret_reg(float_register_info_t &reg, const type_info &info)
	{
		if(info.is_any_float()) {
			reg.register_ = &asmjit::x86::xmm0;
		}
	}

	void asm_builder::get_float_arg_reg(float_register_info_t &reg, const type_info &info, size_t position)
	{
		if(position < 4) {
			if(info.is_any_float()) {
				if(position == 0) {
					reg.register_ = &asmjit::x86::xmm0;
				} else if(position == 1) {
					reg.register_ = &asmjit::x86::xmm1;
				} else if(position == 2) {
					reg.register_ = &asmjit::x86::xmm2;
				} else if(position == 3) {
					reg.register_ = &asmjit::x86::xmm3;
				}
			}
		} else {
			int32_t offset{static_cast<int32_t>(((position+1) * 8)-8)};
			reg.register_ = &asmjit::x86::xmm0;
			if(info.is_float32()) {
				reg.memory = asmjit::x86::dword_ptr(asmjit::x86::rsp, offset);
			} else if(info.is_float64()) {
				reg.memory = asmjit::x86::qword_ptr(asmjit::x86::rsp, offset);
			}
			reg.use_memory = true;
		}
	}

	asm_builder::asm_builder()
	{
		emptysignature.setCallConv(asmjit::CallConv::Id::kIdHost);
		emptysignature.setRetT<void>();
	}

	void asm_builder::error_handler::handleError(asmjit::Error err, const char *, asmjit::BaseEmitter *)
	{
		asmjit::ErrorCode code{err};

		log_asm_builder().error(u8"{}"_sv, code);
		MFW_DEBUGBREAK();
	}

	bool asm_builder::call_function(const func_info &info, const asmjit::FuncSignature &signature, const vector<type_holder> &args, type_holder &result)
	{
		bool is_member{!info.get_this_info().is_void()};

		const vector<type_info> &args_info{info.get_args()};
		if(args_info.size() != (is_member ? (args.size()-1) : args.size())) {
			MFW_DEBUGBREAK();
			return false;
		}

		asmjit::CodeHolder code{};
		code.init(runtime.codeInfo());
		code.setErrorHandler(&errhandler);

		asmjit::x86::Compiler compiler{&code};
		compiler.addFunc(emptysignature);

		for(size_t i{is_member ? 1u : 0u}; i < args.size(); i++) {
			size_t infoindex{is_member ? (i-1) : i};
			size_t regindex{is_member ? (i-1) : i};

			const type_info &func_arg_info{args_info[infoindex]};
			const type_holder &arg_holder{args[i]};
			const type_info &arg_info{arg_holder.info()};

			if(func_arg_info.is_any_reference()) {
				uintptr_t intvalue{arg_holder.get_var<uintptr_t>()};
				const asmjit::Imm imm_arg{asmjit::imm(&intvalue)};

				int_register_info_t reg{};
				get_int_arg_reg(reg, func_arg_info, regindex);
				if(reg.register_) {
					compiler.mov(asmjit::x86::rdx, imm_arg);
				} else {
					compiler.mov(reg.memory, imm_arg);
				}
			} else if(func_arg_info.is_ptr_like()) {
				uintptr_t intvalue{arg_holder.get_var<uintptr_t>()};
				const asmjit::Imm imm_arg{asmjit::imm(&intvalue)};

				int_register_info_t reg{};
				get_int_arg_reg(reg, func_arg_info, regindex);
				if(reg.register_) {
					if(intvalue == 0) {
						compiler.xor_(*reg.register_, *reg.register_);
					} else {
						compiler.mov(asmjit::x86::rdx, imm_arg);
					}
				} else {
					compiler.mov(reg.memory, imm_arg);
				}
			} else if(func_arg_info.is_any_int()) {
				MFW_MESSAGE("todo convert")
				int64_t intvalue{arg_holder.convert<int64_t>()};

				int_register_info_t reg{};
				get_int_arg_reg(reg, func_arg_info, regindex);
				if(reg.register_) {
					if(intvalue == 0) {
						compiler.xor_(*reg.register_, *reg.register_);
					} else {
						compiler.mov(*reg.register_, intvalue);
					}
				} else {
					compiler.mov(reg.memory, intvalue);
				}
			} else if(func_arg_info.is_any_float()) {
				MFW_MESSAGE("todo convert")
				float64_t flvalue{arg_holder.convert<float64_t>()};

				asmjit::x86::Mem memvalue{};
				if(arg_info.is_float32()) {
					memvalue = compiler.newFloatConst(asmjit::ConstPool::Scope::kScopeGlobal, static_cast<float32_t>(flvalue));
				} else if(arg_info.is_float64()) {
					memvalue = compiler.newDoubleConst(asmjit::ConstPool::Scope::kScopeGlobal, static_cast<float64_t>(flvalue));
				}

				float_register_info_t reg{};
				get_float_arg_reg(reg, func_arg_info, regindex);
				if(flvalue == 0.0) {
					compiler.xorps(*reg.register_, *reg.register_);
				} else {
					if(func_arg_info.is_float32()) {
						compiler.movss(*reg.register_, memvalue);
					} else if(func_arg_info.is_float64()) {
						compiler.movsd(*reg.register_, memvalue);
					}
				}
				if(reg.use_memory) {
					if(func_arg_info.is_float32()) {
						compiler.movss(reg.memory, *reg.register_);
					} else if(func_arg_info.is_float64()) {
						compiler.movsd(reg.memory, *reg.register_);
					}
				}
			} else {
				MFW_DEBUGBREAK();
				return false;
			}
		}

		if(is_member) {
			const type_holder &arg_holder{args[0]};
			uintptr_t intvalue{arg_holder.get_var<uintptr_t>()};
			const asmjit::Imm imm_this{asmjit::imm(intvalue)};
			compiler.mov(asmjit::x86::rcx, imm_this);
		}

		const void *func_ptr{info.get_funcptr()};
		const asmjit::Imm imm_func{asmjit::imm(func_ptr)};
		compiler.call(imm_func, signature);

		const type_info &retinfo{info.get_return_info()};
		if(!retinfo.is_void()) {
			result.deduce(retinfo);

			union {
				int8_t i8;
				uint8_t u8;
				int16_t i16;
				uint16_t u16;
				int32_t i32;
				uint32_t u32;
				int64_t i64;
				uint64_t u64{0};
				float32_t f32;
				float64_t f64;
			} retu{};

			const asmjit::x86::Mem ret_mem{asmjit::x86::qword_ptr_abs(reinterpret_cast<uintptr_t>(&retu))};

			if(retinfo.is_any_int() || retinfo.is_ptr_like()) {
				int_register_info_t reg{};
				get_int_ret_reg(reg, retinfo);

				compiler.mov(ret_mem, *reg.register_);
			} else if(retinfo.is_any_float()) {
				float_register_info_t reg{};
				get_float_ret_reg(reg, retinfo);

				if(retinfo.is_float32()) {
					compiler.movss(ret_mem, *reg.register_);
				} else if(retinfo.is_float64()) {
					compiler.movsd(ret_mem, *reg.register_);
				}
			} else {
				MFW_DEBUGBREAK();
				return false;
			}

			if(retinfo.is_int8()) {
				if(retinfo.is_unsigned()) {
					result.set_var(retu.u8);
				} else {
					result.set_var(retu.i8);
				}
			} else if(retinfo.is_int16()) {
				if(retinfo.is_unsigned()) {
					result.set_var(retu.u16);
				} else {
					result.set_var(retu.i16);
				}
			} else if(retinfo.is_int32()) {
				if(retinfo.is_unsigned()) {
					result.set_var(retu.u32);
				} else {
					result.set_var(retu.i32);
				}
			} else if(retinfo.is_int64()) {
				if(retinfo.is_unsigned()) {
					result.set_var(retu.u64);
				} else {
					result.set_var(retu.i64);
				}
			} else if(retinfo.is_float32()) {
				result.set_var(retu.f32);
			} else if(retinfo.is_float64()) {
				result.set_var(retu.f64);
			}
		}

		compiler.endFunc();
		compiler.finalize();

		using func_t = void (*)();
		func_t func{nullptr};
		runtime.add(&func, &code);

		func();

		runtime.release(func);

		return true;
	}

	asmjit::Type::Id asm_builder::to_asmjit_type(const type_info &info)
	{
		if(info.is_ptr_like() || info.is_any_reference()) {
			return asmjit::Type::Id::kIdUIntPtr;
		} else if(info.is_any_int()) {
			if(info.is_signed()) {
				if(info.is_int8()) {
					return asmjit::Type::Id::kIdI8;
				} else if(info.is_int16()) {
					return asmjit::Type::Id::kIdI16;
				} else if(info.is_int32()) {
					return asmjit::Type::Id::kIdI32;
				} else if(info.is_int64()) {
					return asmjit::Type::Id::kIdI64;
				}
			} else if(info.is_unsigned()) {
				if(info.is_int8()) {
					return asmjit::Type::Id::kIdU8;
				} else if(info.is_int16()) {
					return asmjit::Type::Id::kIdU16;
				} else if(info.is_int32()) {
					return asmjit::Type::Id::kIdU32;
				} else if(info.is_int64()) {
					return asmjit::Type::Id::kIdU64;
				}
			}
		} else if(info.is_any_float()) {
			if(info.is_float32()) {
				return asmjit::Type::Id::kIdF32;
			} else if(info.is_float64()) {
				return asmjit::Type::Id::kIdF64;
			}
		} else if(info.is_void()) {
			return asmjit::Type::Id::kIdVoid;
		}

		MFW_DEBUGBREAK();
		return asmjit::Type::Id::kIdVoid;
	}
}

#endif