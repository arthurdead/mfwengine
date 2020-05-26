#ifndef __MFW_PUBLIC_STL_STRING_VIEW_H
#define __MFW_PUBLIC_STL_STRING_VIEW_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/shared/char_traits.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <string_view>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename C, typename T = char_traits<C>>
	using basic_string_view = ::MFW_STD_NAMESPACE::basic_string_view<C, T>;

	using string_view = basic_string_view<char, char_traits<char>>;
	using wstring_view = basic_string_view<wchar_t, char_traits<wchar_t>>;
	using u8string_view = basic_string_view<char8_t, char_traits<char8_t>>;
	using u16string_view = basic_string_view<char16_t, char_traits<char16_t>>;
	using u32string_view = basic_string_view<char32_t, char_traits<char32_t>>;
	using ucstring_view = basic_string_view<ucchar_t, char_traits<ucchar_t>>;
	using uwstring_view = basic_string_view<uwchar_t, char_traits<uwchar_t>>;
	using usstring_view = basic_string_view<uschar_t, char_traits<uschar_t>>;

	namespace literals
	{
		using namespace ::MFW_STD_NAMESPACE::string_view_literals;
	}
#else
	#error
#endif
}

#include <public/mfw/stl/detail/string_view_funcs.hpp>

#endif