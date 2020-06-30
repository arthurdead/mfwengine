#ifndef _MFW_PUBLIC_STL_TYPEINFO_HPP
#define _MFW_PUBLIC_STL_TYPEINFO_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>

#include <typeinfo>

#if MFW_HAS_FEATURE(cxx_rtti) || \
	defined __cpp_rtti || \
	defined __GXX_RTTI || \
	defined _CPPRTTI
	#define MFW_CPP_RTTI_SUPPORTED 1
#endif

namespace mfw::stl
{
	using ::std::type_info;

#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_PUSH_OPTIONS()
	MFW_OPTIMIZE_PRAGMA("rtti")
#endif
	
	template <typename T>
	inline const type_info &get_typeid(T) noexcept
		{ return typeid(T); }

	template <typename T>
	inline const type_info &get_typeid() noexcept
		{ return typeid(T); }
		
	template <typename D, typename S>
	inline D runtime_cast(S src) noexcept(false)
		{ return dynamic_cast<D>(src); }
	
#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_POP_OPTIONS()
#endif
}

#endif