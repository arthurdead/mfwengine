#ifndef __MFW_PUBLIC_CORE_RTTR_INTERFACE_INL
#define __MFW_PUBLIC_CORE_RTTR_INTERFACE_INL

#pragma once

#include <public/mfw/core/rttr_interface.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/type_traits.hpp>

using namespace ::mfw::stl::literals;

namespace mfw::core
{
	template <typename T>
	void func_info::deduce(T func)
	{
		clear();

		set_funcptr(func);
		virtualindex = static_cast<size_t>(-1);

		using traits_t = function_traits<T>;
		using ret_t = typename traits_t::ret_t;
		using this_t = typename traits_t::this_t;
		using args_t = typename traits_t::args_t;

		return_info.deduce(type_identity<ret_t>{});
		this_info.deduce(type_identity<this_t>{});

		constexpr size_t size{traits_t::args_num};
		if constexpr(size > 0) {
			add_arg_info_helper<0, size, args_t>();
		}
	}

	template <size_t i, size_t s, typename T>
	void func_info::add_arg_info_helper()
	{
		if constexpr(i < s) {
			using type_t = tuple_element_t<i, T>;

			add_arg_info(type_identity<type_t>{});

			if constexpr(i+1 < s) {
				add_arg_info_helper<i+1, s, T>();
			}
		}
	}

	template <typename T>
	void func_info::add_arg_info(type_identity<T>)
	{
		type_info info{};
		info.deduce(type_identity<T>{});
		arg_infos.emplace_back(move(info));
	}

	template <typename T>
	void type_holder::deduce(type_identity<T>)
	{
		clear();
		info_.deduce(type_identity<T>{});
		allocate();
	}

	template <typename T>
	void type_holder::deduce(const T &var)
	{
		deduce(type_identity<T>{});
		set_var(var);
	}

	template <typename T>
	void type_holder::deduce(const class_info &_info, const T &var)
	{
		deduce(_info);
		set_var(var);
	}

	template <typename T>
	void type_holder::deduce(const type_info &_info, const T &var)
	{
		deduce(_info);
		set_var(var);
	}

	template <typename T>
	bool type_holder::set_var(const T &var)
	{
		const T *ptr{&var};
		return set_var(static_cast<const void *>(ptr));
	}

	template <typename T>
	T type_holder::convert() const
	{
		if(info_.is_ptr_like()) {
			return static_cast<T>(get_var<uintptr_t>());
		} else if(info_.is_int8()) {
			return static_cast<T>(get_var<int8_t>());
		} else if(info_.is_int16()) {
			return static_cast<T>(get_var<int16_t>());
		} else if(info_.is_int32()) {
			return static_cast<T>(get_var<int32_t>());
		} else if(info_.is_int64()) {
			return static_cast<T>(get_var<int64_t>());
		} else if(info_.is_float32()) {
			return static_cast<T>(get_var<float32_t>());
		} else if(info_.is_float64()) {
			return static_cast<T>(get_var<float64_t>());
		} else {
			MFW_DEBUGBREAK();
			return static_cast<T>(get_var<T>());
		}
	}

	template <typename T, type_info::modify_type_flags F>
	constexpr decltype(auto) type_info::modify_type::__modify_type_helper()
	{
		if constexpr(bool_cast(F & modify_type_flags::remove_const)) {
			using type_t = remove_const_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::remove_const>();
		} else if constexpr(bool_cast(F & modify_type_flags::remove_reference)) {
			using type_t = remove_reference_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::remove_reference>();
		} else if constexpr(bool_cast(F & modify_type_flags::remove_pointer)) {
			using type_t = remove_pointer_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::remove_pointer>();
		} else if constexpr(bool_cast(F & modify_type_flags::remove_all_extents)) {
			using type_t = remove_all_extents_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::remove_all_extents>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_lvalue_reference)) {
			using type_t = add_lvalue_reference_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::add_lvalue_reference>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_rvalue_reference)) {
			using type_t = add_rvalue_reference_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::add_rvalue_reference>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_pointer)) {
			using type_t = add_pointer_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::add_pointer>();
		} else if constexpr(bool_cast(F & modify_type_flags::add_const)) {
			using type_t = add_const_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::add_const>();
		} else if constexpr(bool_cast(F & modify_type_flags::make_signed)) {
			using type_t = make_signed_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::make_signed>();
		} else if constexpr(bool_cast(F & modify_type_flags::make_unsigned)) {
			using type_t = make_unsigned_t<T>;
			return __modify_type_helper<type_t, F & ~modify_type_flags::make_unsigned>();
		} else {
		#if MFW_COMPILER == MFW_COMPILER_MSVC
			return declval<T>();
		#elif MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
			if constexpr(!is_void_v<T>) {
				return T{};
			} else {
				return;
			}
		#else
			#error
		#endif
		}
	}

	template <typename T>
	void type_info::deduce(type_identity<T>)
	{
		using base_type_t = modify_type_t<T, modify_type_flags::remove_all>;
		using only_const_t = modify_type_t<T, modify_type_flags::remove_all_but_const>;
		using only_reference_t = modify_type_t<T, modify_type_flags::remove_all_but_reference>;
		using only_pointer_t = modify_type_t<T, modify_type_flags::remove_all_but_pointer>;
		using only_extents_t = modify_type_t<T, modify_type_flags::remove_all_but_extents>;

		clear();
		if constexpr(!is_void_v<only_pointer_t>) {
			size_ = sizeof(only_pointer_t);
			align_ = alignof(only_pointer_t);
		} else {
			size_ = 0;
			align_ = 0;
		}
		std_info_ = &get_typeid<base_type_t>();
		set_name();

		rank_ = rank_v<only_extents_t>;
		extent_ = extent_v<only_extents_t>;

		if constexpr(is_class_v<base_type_t>) {
			flags_ |= flags::class_;
		}
		if constexpr(is_pointer_v<only_pointer_t>) {
			flags_ |= flags::pointer_;
			pointers += 1;
		}
		if constexpr(is_array_v<only_extents_t>) {
			flags_ |= flags::array_;
		}
		if constexpr(is_signed_v<base_type_t>) {
			flags_ |= flags::signed_;
		} else if constexpr(is_unsigned_v<base_type_t>) {
			flags_ |= flags::unsigned_;
		}
		if constexpr(is_const_v<only_const_t>) {
			flags_ |= flags::const_;
		}
		if constexpr(is_lvalue_reference_v<only_reference_t> || is_lvalue_reference_v<T>) {
			flags_ |= flags::lvalue_ref;
		} else if constexpr(is_rvalue_reference_v<only_reference_t> || is_rvalue_reference_v<T>) {
			flags_ |= flags::rvalue_ref;
		}
	}

	template <typename T>
	bool type_info::is_exact() const
	{
		using only_pointer_t = modify_type_t<T, modify_type_flags::remove_all_but_pointer>;

		return is_exact(get_typeid<only_pointer_t>());
	}

	template <typename T>
	bool type_info::is_relaxed() const
	{
		using only_pointer_t = modify_type_t<T, modify_type_flags::remove_all_but_pointer>;

		constexpr bool is_int_v{
			!is_same_v<only_pointer_t, bool> &&
			!is_floating_point_v<only_pointer_t> &&
			!is_void_v<only_pointer_t> &&
			is_arithmetic_v<only_pointer_t> &&
			is_fundamental_v<only_pointer_t>
		};

		if constexpr(is_int_v) {
			if(is_signed()) {
				using signed_t = modify_type_t<only_pointer_t, modify_type_flags::make_signed>;
				return is_exact(get_typeid<signed_t>());
			} else if(is_unsigned()) {
				using unsigned_t = modify_type_t<only_pointer_t, modify_type_flags::make_unsigned>;
				return is_exact(get_typeid<unsigned_t>());
			}
		}

		return is_exact(get_typeid<only_pointer_t>());
	}

	template <typename T>
	void class_info::deduce(type_identity<T>)
	{
		clear();
		super::deduce(type_identity<T>{});
		counstructor(type_identity<T>{}, type_identity_multiple<>{});
		counstructor(type_identity<T>{}, type_identity_multiple<const T &>{});
		counstructor(type_identity<T>{}, type_identity_multiple<T &&>{});
		function(overload_cast_member(T &, MFW_NOTHING, T, operator=, MFW_NOTHING, const T &), u8"operator="_sv);
		function(overload_cast_member(T &, MFW_NOTHING, T, operator=, MFW_NOTHING, T &&), u8"operator="_sv);
		destructor(type_identity<T>{});
		set_name(interfaces::rttr::instance().clean_name(get_typeid<T>()));
	}

	template <typename T, typename V>
	void class_info::member_variable_info::deduce(V T:: *var_)
	{
		clear();
		super::deduce(type_identity<V>{});
		offset = var_offset(var_);
		var = force_cast<void *>(var_);
		set_func = force_cast<void *>(__set_helper<T, V>);
		get_func = force_cast<void *>(__get_helper<T, V>);
	}

	template <typename T, typename ...Args>
	bool class_info::counstructor(type_identity<T>, type_identity_multiple<Args...>)
	{
		func_info info{};
		info.deduce(overload_cast_member(T *, MFW_NOTHING, __internal_funcs_helper<T>, counstructor, MFW_NOTHING, Args...));
		info.set_name(u8"counstructor"_sv);
		info.set_this_info(type_identity<T>{});
		functions.emplace_back(move(info));
		return false;
	}

	template <typename T>
	bool class_info::destructor(type_identity<T>)
	{
		func_info info{};
		info.deduce(overload_cast_member(void, MFW_NOTHING, __internal_funcs_helper<T>, destructor, MFW_NOTHING, void));
		info.set_name(u8"destructor"_sv);
		info.set_this_info(type_identity<T>{});
		functions.emplace_back(move(info));
		return true;
	}

	template <typename V, typename T>
	bool class_info::variable(V T::*var, const ucstring_view &_name_)
	{
		member_variable_info info{};
		info.deduce(var);
		info.set_name(_name_);
		variables.emplace_back(move(info));
		return true;
	}

	template <typename R, typename T, typename ...Args>
	bool class_info::function(R (T::*func)(Args...), const ucstring_view &_name_)
	{
		func_info info{};
		info.deduce(func);
		info.set_name(_name_);
		functions.emplace_back(move(info));
		return false;
	}
}

#endif