#include <private/mfw/core/rttr.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/debugging.hpp>
#include <public/mfw/stl/memory.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/pch_literals.hpp>

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_rttr, u8"core/rttr"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(rttr, core::rttr)

	core::rttr &core::rttr::instance() {
		return __rttr_global_allocator.instance();
	}
	MFW_CORE_API interfaces::rttr & MFW_CORE_CALL interfaces::rttr::instance()
		{ return core::rttr::instance(); }

	bool core::rttr::register_class_info(const class_info &info)
	{
	#ifdef __MFW_USE_ASMJIT
		class_info_vec_t::value_type &jitinfo{class_infos.emplace_back()};
		jitinfo = info;
		if(!jitinfo.build_signatures()) {
			MFW_DEBUGBREAK();
			return false;
		}
		return true;
	#else
		class_infos.emplace_back(info);
		return true;
	#endif
	}

	bool core::rttr::register_func_info(const func_info &info)
	{
	#ifdef __MFW_USE_ASMJIT
		func_info_vec_t::value_type &jitinfo{function_infos.emplace_back()};
		jitinfo = info;
		if(!jitinfo.build_signature()) {
			MFW_DEBUGBREAK();
			return false;
		}
		return true;
	#else
		function_infos.emplace_back(info);
		return true;
	#endif
	}

	const class_info *core::rttr::find_class_info(const ucstring_view &name)
	{
		for(const class_info_vec_t::value_type &it : class_infos) {
			const class_info_vec_t::value_type &info{it};
			if(info.get_name() == name) {
				return &info;
			}
		}
		return nullptr;
	}

	const func_info *core::rttr::find_function_info(const ucstring_view &name) const
	{
		for(const func_info_vec_t::value_type &it : function_infos) {
			const func_info_vec_t::value_type &info{it};
			if(info.get_name() == name) {
				return &info;
			}
		}
		return nullptr;
	}

	const ucstring &core::rttr::clean_name(const ::std::type_info &info) const
	{
		static ucstring name{};
		name = uc_str(info.name());

		static ucstring str{};
		str.clear();
		undecorate(name, str, undecorate_flags::name_only);

		replace_all(str, u8"class "_sv, {});
		replace_all(str, u8"struct "_sv, {});
	#if MFW_PROCESSOR & MFW_PROCESSOR_64BITS_FLAG
		replace_all(str, u8" * __ptr64"_sv, {});
	#elif MFW_PROCESSOR & MFW_PROCESSOR_32BITS_FLAG
		replace_all(str, u8" * __ptr32"_sv, {});
	#endif

		return str;
	}

	bool core::rttr::call_function(const func_info &info, const vector<type_holder> &args, type_holder &result) const
	{
	#ifdef __MFW_USE_ASMJIT
		const asmjit::FuncSignature &signature{info.get_extrainfo<asmjit::FuncSignature>()};
		if(!asm_builder::instance().call_function(info, signature, args, result)) {
			MFW_DEBUGBREAK();
			return false;
		}
		return true;
	#else
		return false;
	#endif
	}

	const func_info *core::rttr::find_internal_function(internal_func which, const class_info &info) const
	{
		for(const func_info &it : info.get_functions()) {
			const ucstring &name{it.get_name()};
			const vector<type_info> &args{it.get_args()};

			if(which == internal_func::default_ctor || which == internal_func::copy_ctor ||
				which == internal_func::move_ctor) {
				if(name != u8"counstructor"_sv) {
					continue;
				}
			} else if(which == internal_func::copy_assign || which == internal_func::move_assign) {
				if(name != u8"operator="_sv) {
					continue;
				}
			} else if(which == internal_func::dtor) {
				if(name != u8"destructor"_sv) {
					continue;
				}
			}

			if(which == internal_func::default_ctor || which == internal_func::dtor) {
				if(args.size() != 0) {
					continue;
				}
			} else if(which == internal_func::copy_ctor || which == internal_func::move_ctor ||
						which == internal_func::copy_assign || which == internal_func::move_assign) {
				if(args.size() != 1) {
					continue;
				}
			}

			if(which == internal_func::copy_ctor || which == internal_func::move_ctor ||
				which == internal_func::copy_assign || which == internal_func::move_assign) {
				const type_info &arg0{args[0]};
				if(!arg0.is_class()) {
					continue;
				}

				if(arg0.name() != info.get_name()) {
					continue;
				}

				if(which == internal_func::move_ctor || which == internal_func::move_assign) {
					if(!arg0.is_rvalue_ref()) {
						continue;
					}
				} else if(which == internal_func::copy_ctor || which == internal_func::copy_assign) {
					if(!arg0.is_const() || !arg0.is_lvalue_ref()) {
						continue;
					}
				}
			}

			return &it;
		}

		return nullptr;
	}

	namespace __rttr_internal
	{
		static void class_as_pointer(const type_info &cls_info, type_holder &holder)
		{
			type_info info{};
			info = cls_info;
			info.make_pointer();
			holder.deduce(info);
		}
	}

	bool core::rttr::call_internal_function(internal_func which, void *dst, const void *src, const class_info &info) const
	{
		const func_info *func{find_internal_function(which, info)};
		if(!func) {
			MFW_DEBUGBREAK();
			return false;
		}

		vector<type_holder> args{};

		type_holder &dstholder{args.emplace_back()};
		__rttr_internal::class_as_pointer(info, dstholder);
		dstholder.set_var(dst);

		if(which != internal_func::dtor && which != internal_func::default_ctor) {
			if(src) {
				const type_info &arg0{func->get_arg(0)};
				type_holder &srcholder{args.emplace_back()};
				srcholder.deduce(arg0);
				srcholder.set_var_raw(src);
			} else {
				MFW_DEBUGBREAK();
				return false;
			}
		}

		type_holder result{};
		if(!call_function(*func, args, result)) {
			MFW_DEBUGBREAK();
			return false;
		}

		return true;
	}

	void *core::rttr::allocate(const class_info &info) const
	{
		void *ptr{::mfw::stl::allocate(info.size(), info.align(), 0, false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS == MFW_OS_WINDOWS
		,MFW_MEMORY_DEFAULT_BLOCK
		#endif
		,__FILE__, __LINE__, &info.std_info()
	#endif
		)};
		if(!call_internal_function(internal_func::default_ctor, ptr, nullptr, info)) {
			MFW_DEBUGBREAK();
		}
		return ptr;
	}

	bool core::rttr::deallocate(void *ptr, const class_info &info) const
	{
		bool called{call_internal_function(internal_func::dtor, ptr, nullptr, info)};
		::mfw::stl::deallocate(ptr, info.size(), info.align(), 0, false
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS == MFW_OS_WINDOWS
		,MFW_MEMORY_DEFAULT_BLOCK
		#endif
		,__FILE__, __LINE__, &info.std_info()
	#endif
		);
		if(!called) {
			MFW_DEBUGBREAK();
			return false;
		}
		return true;
	}

#ifdef __MFW_USE_ASMJIT
	bool core::rttr::func_info_jit::build_signature()
	{
		signature.setCallConv(asmjit::CallConv::Id::kIdHost);
		asmjit::Type::Id id{asm_builder::to_asmjit_type(get_return_info())};
		signature.setRet(id);
		const vector<type_info> &args{get_args()};
		for(const type_info &arg : args) {
			id = asm_builder::to_asmjit_type(arg);
			signature.addArg(id);
		}
		set_extrainfo(signature);
		return true;
	}

	bool core::rttr::class_info_jit::build_signatures()
	{
		vector<func_info> &funcs{get_functions()};

		signatures.resize(funcs.size());

		for(size_t i{0}; i < funcs.size(); i++) {
			func_info &func{funcs[i]};
			asmjit::FuncSignatureBuilder &signature{signatures[i]};

			signature.setCallConv(asmjit::CallConv::Id::kIdHost);
			asmjit::Type::Id id{asm_builder::to_asmjit_type(func.get_return_info())};
			signature.setRet(id);
			const vector<type_info> &args{func.get_args()};
			for(const type_info &arg : args) {
				id = asm_builder::to_asmjit_type(arg);
				signature.addArg(id);
			}

			func.set_extrainfo(signature);
		}

		return true;
	}
#endif

	MFW_CORE_API void MFW_CORE_CALL type_holder::call_dtor()
	{
		if(info_.is_class() && !info_.is_ptr_like() && !memory_ && ctor_called) {
			const class_info *cls_info{info_.find_class_info()};
			if(!cls_info) {
				MFW_DEBUGBREAK();
			}

			core::rttr::instance().call_internal_function(core::rttr::internal_func::dtor, memory_.get(), nullptr, *cls_info);
			ctor_called = false;
		}
	}

	MFW_CORE_API bool MFW_CORE_CALL type_holder::set_var_raw(const void *var)
	{
		unset_var();

		if(var) {
			copy(memory_.get(), info_.size(), var, info_.size());
		} else {
			MFW_DEBUGBREAK();
			return false;
		}

		return true;
	}

	bool type_holder::call_default_ctor()
	{
		const class_info *cls_info{info_.find_class_info()};
		if(!cls_info) {
			MFW_DEBUGBREAK();
			return false;
		}
		if(!core::rttr::instance().call_internal_function(core::rttr::internal_func::default_ctor, memory_.get(), nullptr, *cls_info)) {
			MFW_DEBUGBREAK();
			return false;
		}
		ctor_called = true;
		return true;
	}

	MFW_CORE_API type_holder & MFW_CORE_CALL type_holder::operator=(const type_holder &other)
	{
		info_ = other.info_;
		if(other.memory_) {
			allocate();
			copy(memory_.get(), info_.size(), other.memory_.get(), other.info_.size());
		} else {
			memory_.reset(nullptr);
		}
		ctor_called = other.ctor_called;
		return *this;
	}

	bool type_holder::call_copy_ctor(const void *var)
	{
		const class_info *cls_info{info_.find_class_info()};
		if(!cls_info) {
			copy(memory_.get(), info_.size(), var, info_.size());
			return true;
		}
		if(!core::rttr::instance().call_internal_function(core::rttr::internal_func::copy_ctor, memory_.get(), var, *cls_info)) {
			MFW_DEBUGBREAK();
			return false;
		}
		ctor_called = true;
		return true;
	}

	MFW_CORE_API bool MFW_CORE_CALL type_holder::set_var(const void *var)
	{
		unset_var();
		allocate();

		if(info_.is_primitive() || info_.is_ptr_like()) {
			if(var) {
				copy(memory_.get(), info_.size(), var, info_.size());
			} else {
				MFW_DEBUGBREAK();
				return false;
			}
		} else {
			if(!var) {
				if(!call_default_ctor()) {
					return false;
				}
			} else {
				if(!call_copy_ctor(var)) {
					return false;
				}
			}
		}

		return true;
	}

	MFW_CORE_API void MFW_CORE_CALL type_holder::unset_var()
	{
		call_dtor();
		memory_.reset(nullptr);
	}

	MFW_CORE_API void MFW_CORE_CALL type_holder::deduce(const type_info &_info)
	{
		call_dtor();
		clear();
		info_ = _info;
		allocate();
	}

	MFW_CORE_API void MFW_CORE_CALL type_info::set_name()
	{
		name_ = core::rttr::instance().clean_name(*std_info_);
	}

	MFW_CORE_API void MFW_CORE_CALL type_info::make_pointer()
	{
		flags_ |= flags::pointer_;
		size_ = sizeof(void *);
		align_ = alignof(void *);
		pointers += 1;
	}

	MFW_CORE_API void MFW_CORE_CALL type_info::clear()
	{
		size_ = 0;
		align_ = 0;
		std_info_ = nullptr;
		name_.clear();
		pointers = 0;
		extent_ = 0;
		rank_ = 0;
	}

	MFW_CORE_API void MFW_CORE_CALL type_holder::clear()
	{
		info_.clear();
		memory_.reset(nullptr);
		ctor_called = false;
	}

	MFW_CORE_API const class_info * MFW_CORE_CALL type_info::find_class_info() const
	{
		return core::rttr::instance().find_class_info(name());
	}

	class __empty_class {};

	template <typename T>
	using __set_func_t = void (*)(__empty_class *ptr, T __empty_class::*var, const T &val);

	template <typename T>
	using __get_func_t = void (*)(const __empty_class *ptr, T __empty_class::*var, T &val);

	template <typename T>
	static void __set_func(const class_info::member_variable_info &, void *func, __empty_class *ptr, void *var, const type_holder &val)
	{
		T value{val.convert<T>()};
		reinterpret_cast<__set_func_t<T>>(func)(ptr, force_cast<T __empty_class::*>(var), value);
	}

	template <typename T>
	static void __get_func(const class_info::member_variable_info &, void *func, const __empty_class *ptr, void *var, type_holder &val)
	{
		T value{};
		reinterpret_cast<__get_func_t<T>>(func)(ptr, force_cast<T __empty_class::*>(var), value);
		val.set_var(value);
	}

	template <typename T>
	static void __get_func_ptr(const class_info::member_variable_info &, void *, const __empty_class *ptr, void *var, type_holder &val)
	{
		const T *value{&(ptr->*force_cast<T __empty_class::*>(var))};
		val.set_var(value);
	}

	MFW_CORE_API void MFW_CORE_CALL class_info::member_variable_info::set(type_holder &obj, const type_holder &val) const
	{
		__empty_class *ptr{obj.get_var_ptr<__empty_class>()};

		if(is_pointer()) {
			__set_func<uintptr_t>(*this, set_func, ptr, var, val);
		} else if(is_int8()) {
			__set_func<int8_t>(*this, set_func, ptr, var, val);
		} else if(is_int16()) {
			__set_func<int16_t>(*this, set_func, ptr, var, val);
		} else if(is_int32()) {
			__set_func<int32_t>(*this, set_func, ptr, var, val);
		} else if(is_int64()) {
			__set_func<int64_t>(*this, set_func, ptr, var, val);
		} else if(is_float32()) {
			__set_func<float32_t>(*this, set_func, ptr, var, val);
		} else if(is_float64()) {
			__set_func<float64_t>(*this, set_func, ptr, var, val);
		} else {
			MFW_DEBUGBREAK();
		}
	}

	MFW_CORE_API void MFW_CORE_CALL class_info::member_variable_info::get_ptr(const type_holder &obj, type_holder &val) const
	{
		const __empty_class *ptr{obj.get_var_ptr<__empty_class>()};

		__rttr_internal::class_as_pointer(*this, val);

		if(is_pointer()) {
			__get_func_ptr<uintptr_t>(*this, get_func, ptr, var, val);
		} else if(is_int8()) {
			__get_func_ptr<int8_t>(*this, get_func, ptr, var, val);
		} else if(is_int16()) {
			__get_func_ptr<int16_t>(*this, get_func, ptr, var, val);
		} else if(is_int32()) {
			__get_func_ptr<int32_t>(*this, get_func, ptr, var, val);
		} else if(is_int64()) {
			__get_func_ptr<int64_t>(*this, get_func, ptr, var, val);
		} else if(is_float32()) {
			__get_func_ptr<float32_t>(*this, get_func, ptr, var, val);
		} else if(is_float64()) {
			__get_func_ptr<float64_t>(*this, get_func, ptr, var, val);
		} else {
			MFW_DEBUGBREAK();
		}
	}

	MFW_CORE_API void MFW_CORE_CALL class_info::member_variable_info::get(const type_holder &obj, type_holder &val) const
	{
		const __empty_class *ptr{obj.get_var_ptr<__empty_class>()};

		val.deduce(*this);

		if(is_pointer()) {
			__get_func<uintptr_t>(*this, get_func, ptr, var, val);
		} else if(is_int8()) {
			__get_func<int8_t>(*this, get_func, ptr, var, val);
		} else if(is_int16()) {
			__get_func<int16_t>(*this, get_func, ptr, var, val);
		} else if(is_int32()) {
			__get_func<int32_t>(*this, get_func, ptr, var, val);
		} else if(is_int64()) {
			__get_func<int64_t>(*this, get_func, ptr, var, val);
		} else if(is_float32()) {
			__get_func<float32_t>(*this, get_func, ptr, var, val);
		} else if(is_float64()) {
			__get_func<float64_t>(*this, get_func, ptr, var, val);
		} else {
			MFW_DEBUGBREAK();
		}
	}
}