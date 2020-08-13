#ifndef MFW_PUBLIC_STL_STRING_VIEW_HPP
#define MFW_PUBLIC_STL_STRING_VIEW_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/shared/char_traits.hpp>
#include <public/mfw/stl/functional.hpp>
#include <public/mfw/stl/filesystem.hpp>

#if MFW_STDCPP_IS(DEFAULT)
	#include <string_view>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/string_view.h>
#else
	#error
#endif

namespace mfw::stl
{
	template <typename _Cp, typename _Tp = char_traits<_Cp>>
	using basic_string_view = ::MFW_STD_NAMESPACE::basic_string_view<_Cp, _Tp>;

	using string_view = basic_string_view<char, char_traits<char>>;
	using wstring_view = basic_string_view<wchar_t, char_traits<wchar_t>>;
	using u8string_view = basic_string_view<char8_t, char_traits<char8_t>>;
	using u16string_view = basic_string_view<char16_t, char_traits<char16_t>>;
	using u32string_view = basic_string_view<char32_t, char_traits<char32_t>>;

	using osstring_view = basic_string_view<oschar_t, char_traits<oschar_t>>;

	using pstring_view = type_view_t<filesystem::path>;
}

#include <public/mfw/stl/internal/string_view_funcs.hpp>

#endif