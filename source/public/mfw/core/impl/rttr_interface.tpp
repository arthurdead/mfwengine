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

	
}

#endif