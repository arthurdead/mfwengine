#ifndef __MFW_PUBLIC_STL_STRING_H
#define __MFW_PUBLIC_STL_STRING_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/detail/allocator.hpp>
#include <public/mfw/stl/functional.hpp>
#include <public/mfw/stl/shared/char_traits.hpp>
#include <public/mfw/stl/type_traits.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <string>
	#include <filesystem>
	#pragma pop_macro("new")
#else
	#error
#endif

#define MFW_PATH_FROM_CHARARRAY(name, len) reinterpret_cast<const upchar_t *>(name), reinterpret_cast<const upchar_t *>(name+len)

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename C, typename T = char_traits<C>, typename A = allocator<C>>
	using basic_string = ::MFW_STD_NAMESPACE::basic_string<C, T, A>;

	using string = basic_string<char, char_traits<char>, allocator<char>>;
	using wstring = basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>>;
	using u8string = basic_string<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u16string = basic_string<char16_t, char_traits<char16_t>, allocator<char16_t>>;
	using u32string = basic_string<char32_t, char_traits<char32_t>, allocator<char32_t>>;
	using ucstring = basic_string<ucchar_t, char_traits<ucchar_t>, allocator<ucchar_t>>;
	using uwstring = basic_string<uwchar_t, char_traits<uwchar_t>, allocator<uwchar_t>>;
	using usstring = basic_string<uschar_t, char_traits<uschar_t>, allocator<uschar_t>>;

	using pstring = ::MFW_STD_NAMESPACE::filesystem::path;
	using npstring = pstring::string_type;
	#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	using unpstring = uwstring;
	#else
	using unpstring = ucstring;
	#endif

	namespace literals
	{
		using namespace ::MFW_STD_NAMESPACE::string_literals;
	}
#else
	#error
#endif
}

namespace MFW_STD_NAMESPACE
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <>
	class hash<::mfw::stl::pstring>
	{
	public:
		::MFW_STD_NAMESPACE::size_t operator()(const ::mfw::stl::pstring &str) const
		{
			MFW_MESSAGE("remove this later")
		#if MFW_COMPILER_FLAGGED(UNIX)
			return ::MFW_STD_NAMESPACE::_Hash_impl::hash(str.c_str(), str.native().length() * sizeof(::mfw::stl::upchar_t));
		#else
			#error
		#endif
		}
	};

	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	template <>
	class hash<::mfw::stl::ucstring>
	{
	public:
		::MFW_STD_NAMESPACE::size_t operator()(const ::mfw::stl::ucstring &str) const
		{
			MFW_MESSAGE("remove this later")
		#if MFW_COMPILER_FLAGGED(UNIX)
			return ::MFW_STD_NAMESPACE::_Hash_impl::hash(str.c_str(), str.length() * sizeof(::mfw::stl::ucchar_t));
		#else
			#error
		#endif
		}
	};

	template <>
	class hash<::mfw::stl::uwstring>
	{
	public:
		::MFW_STD_NAMESPACE::size_t operator()(const ::mfw::stl::uwstring &str) const
		{
			MFW_MESSAGE("remove this later")
		#if MFW_COMPILER_FLAGGED(UNIX)
			return ::MFW_STD_NAMESPACE::_Hash_impl::hash(str.c_str(), str.length() * sizeof(::mfw::stl::uwchar_t));
		#else
			#error
		#endif
		}
	};
	#endif
#endif
}

#include <public/mfw/stl/detail/string_funcs.hpp>
#include <public/mfw/stl/impl/string.ipp>

#endif