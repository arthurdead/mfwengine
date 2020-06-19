#ifndef __MFW_PUBLIC_STL_TYPEINFO_H
#define __MFW_PUBLIC_STL_TYPEINFO_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <typeinfo>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::type_info;
	
	#if MFW_COMPILER_FLAGGED(UNIX)
		MFW_PUSH_OPTIONS()
		MFW_OPTIMIZE_PRAGMA("rtti")
	#endif
	
	template <typename T>
	constexpr inline const ::MFW_STD_NAMESPACE::type_info &get_typeid(T) noexcept
		{ return typeid(T); }

	template <typename T>
	constexpr inline const ::MFW_STD_NAMESPACE::type_info &get_typeid() noexcept
		{ return typeid(T); }
		
	template <typename D, typename S>
	constexpr inline D runtime_cast(S src) noexcept(false)
		{ return dynamic_cast<D>(src); }
	
	#if MFW_COMPILER_FLAGGED(UNIX)
		MFW_POP_OPTIONS()
	#endif
#else
	#error
#endif
}

#endif