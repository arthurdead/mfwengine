#ifndef MFW_PUBLIC_STL_STRING_HPP
#define MFW_PUBLIC_STL_STRING_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/internal/allocator.hpp>
#include <public/mfw/stl/functional.hpp>
#include <public/mfw/stl/shared/char_traits.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/filesystem.hpp>

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
#pragma pop_macro("new")

namespace mfw::stl
{
	template <typename _Cp, typename _Tp = char_traits<_Cp>, typename _Alloc = allocator<_Cp>>
	using basic_string = ::MFW_STD_NAMESPACE::basic_string<_Cp, _Tp, _Alloc>;

	using string = basic_string<char, char_traits<char>, allocator<char>>;
	using wstring = basic_string<wchar_t, char_traits<wchar_t>, allocator<wchar_t>>;
	using u8string = basic_string<char8_t, char_traits<char8_t>, allocator<char8_t>>;
	using u16string = basic_string<char16_t, char_traits<char16_t>, allocator<char16_t>>;
	using u32string = basic_string<char32_t, char_traits<char32_t>, allocator<char32_t>>;
	
	using osstring = basic_string<oschar_t, char_traits<oschar_t>, allocator<oschar_t>>;

	using pstring = filesystem::path;
	using npstring = pstring::string_type;
}

namespace MFW_STD_NAMESPACE
{
	template <>
	class MFW_VISIBILITY_LOCAL hash<::mfw::stl::pstring>
	{
	public:
		::MFW_STD_NAMESPACE::size_t operator()(const ::mfw::stl::pstring &__str) const noexcept;
	};
}

#include <public/mfw/stl/impl/string.tpp>
#include <public/mfw/stl/internal/string_funcs.hpp>

#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/string_view.hpp>

namespace mfw::stl
{
	struct type_view<string> final
	{ using type = string_view; };
	struct type_view<wstring> final
	{ using type = wstring_view; };
	struct type_view<u8string> final
	{ using type = u8string_view; };
	struct type_view<u16string> final
	{ using type = u16string_view; };
	struct type_view<u32string> final
	{ using type = u32string_view; };
	struct type_view<osstring> final
	{ using type = osstring_view; };
}

#endif