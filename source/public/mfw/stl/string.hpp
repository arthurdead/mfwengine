#ifndef _MFW_PUBLIC_STL_STRING_HPP
#define _MFW_PUBLIC_STL_STRING_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/internal/allocator.hpp>
#include <public/mfw/stl/functional.hpp>
#include <public/mfw/stl/shared/char_traits.hpp>
#include <public/mfw/stl/type_traits.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <string>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/string.h>
#else
	#error
#endif
#if MFW_STDC_IS(DEFAULT)
	#include <cstring>
#else
	#error
#endif
#include <filesystem>
#pragma pop_macro("new")

namespace mfw::stl
{
	template <typename C, typename T = char_traits<C>, typename A = allocator<C>>
	using basic_string = ::MFW_STD_NAMESPACE::basic_string<C, T, A>;

	using string = basic_string<char, char_traits<char>, allocator<char>>;
	using wstring = basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>>;
	using u8string = basic_string<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u16string = basic_string<char16_t, char_traits<char16_t>, allocator<char16_t>>;
	using u32string = basic_string<char32_t, char_traits<char32_t>, allocator<char32_t>>;
	
	using osstring = basic_string<oschar_t, char_traits<oschar_t>, allocator<oschar_t>>;

	using pstring = ::std::filesystem::path;
	using npstring = pstring::string_type;
}

namespace MFW_STD_NAMESPACE
{
	template <>
	class hash<::mfw::stl::pstring>
	{
	public:
		::MFW_STD_NAMESPACE::size_t operator()(const ::mfw::stl::pstring &__str) const noexcept;
	};
}

#include <public/mfw/stl/impl/string.ipp>
#include <public/mfw/stl/internal/string_funcs.hpp>

#endif