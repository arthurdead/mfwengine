#ifndef MFW_PUBLIC_STL_TYPEINFO_HPP
#define MFW_PUBLIC_STL_TYPEINFO_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>

#include <typeinfo>
#include <typeindex>

#if MFW_HAS_FEATURE(cxx_rtti) || \
	defined __cpp_rtti || \
	defined __GXX_RTTI || \
	defined _CPPRTTI
	#define MFW_CPP_RTTI_SUPPORTED 1
#endif

namespace mfw::stl
{
	using ::std::type_info;
	using ::std::type_index;

#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_PUSH_OPTIONS()
	MFW_OPTIMIZE_PRAGMA("rtti")
#endif
	
	#pragma push_macro("typeid")
	#undef typeid

	template <typename T>
	constexpr const type_info &get_typeid(const T &) noexcept
	{ return typeid(const T &); }
	template <typename T>
	constexpr const type_info &get_typeid(type_identity<T>) noexcept
	{ return typeid(T); }
	template <typename T>
	constexpr const type_info &get_typeid() noexcept
	{ return typeid(T); }

	#pragma pop_macro("typeid")

	#pragma push_macro("dynamic_cast")
	#undef dynamic_cast
		
	template <typename D, typename S>
	MFW_VISIBILITY_LOCAL inline D runtime_cast(S src) noexcept(false)
	{ return dynamic_cast<D>(src); }

	#pragma pop_macro("typeid")
	
#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_POP_OPTIONS()
#endif
}

#define dynamic_cast ::mfw::stl::runtime_cast
#define typeid ::mfw::stl::get_typeid

#endif