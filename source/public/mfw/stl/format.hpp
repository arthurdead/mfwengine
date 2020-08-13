#ifndef MFW_PUBLIC_STL_FORMAT_HPP
#define MFW_PUBLIC_STL_FORMAT_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cstdarg>
#else
	#error
#endif

namespace mfw::stl
{
	extern MFW_STL_API void MFW_STL_CALL format(string &__string, string_view __format, va_list __args) noexcept;
	extern MFW_STL_API void MFW_STL_CALL format(wstring &__string, wstring_view __format, va_list __args) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL format(u8string &__string, u8string_view __format, va_list __args) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL format(u16string &__string, u16string_view __format, va_list __args) noexcept;
	extern MFW_STL_API void MFW_STL_CALL format(u32string &__string, u32string_view __format, va_list __args) noexcept;

	extern MFW_STL_API bool MFW_STL_CALL format(string &__string, string_view __format, vector_view __args) noexcept;
	extern MFW_STL_API bool MFW_STL_CALL format(wstring &__string, wstring_view __format, vector_view __args) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API bool MFW_STL_CALL format(u8string &__string, u8string_view __format, vector_view __args) noexcept;
#endif
	extern MFW_STL_API bool MFW_STL_CALL format(u16string &__string, u16string_view __format, vector_view __args) noexcept;
	extern MFW_STL_API bool MFW_STL_CALL format(u32string &__string, u32string_view __format, vector_view __args) noexcept;

	template <typename _Sp, typename... _Args>
	MFW_VISIBILITY_LOCAL bool format(_Sp &__str, type_view_t<_Sp> __fmt, _Args &&... __args) noexcept;

	namespace __public_format_hpp
	{
		template <typename _Sp>
		struct MFW_VISIBILITY_LOCAL lit_fmt_base final
		{
			using string_type = _Sp;
			using string_view_type = type_view_t<_Sp>;
			using char_type = typename _Sp::value_type;

			lit_fmt_base(const char_type *__str, size_t __len);

			template <typename... _Args>
			string_type operator()(_Args &&... __args) const noexcept;

		private:
			string_view_type _M_fmt{};
		};
	}

	using lit_fmt_c = __public_format_hpp::lit_fmt_base<string>;
	using lit_fmt_w = __public_format_hpp::lit_fmt_base<wstring>;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	using lit_fmt_u8 = __public_format_hpp::lit_fmt_base<u8string>;
#endif
	using lit_fmt_u16 = __public_format_hpp::lit_fmt_base<u16string>;
	using lit_fmt_u32 = __public_format_hpp::lit_fmt_base<u32string>;

	MFW_DECLARE_AS_TO_FUNC_GLOBAL(string)
	MFW_DECLARE_AS_TO_CHECK_FUNCS(string)
}

MFW_VISIBILITY_LOCAL_PUSH()

extern ::mfw::stl::lit_fmt_c operator""_fmt(const char *str, ::mfw::stl::size_t len) noexcept;
extern ::mfw::stl::lit_fmt_w operator""_fmt(const wchar_t *str, ::mfw::stl::size_t len) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
extern ::mfw::stl::lit_fmt_u8 operator""_fmt(const char8_t *str, ::mfw::stl::size_t len) noexcept;
#endif
extern ::mfw::stl::lit_fmt_u16 operator""_fmt(const char16_t *str, ::mfw::stl::size_t len) noexcept;
extern ::mfw::stl::lit_fmt_u32 operator""_fmt(const char32_t *str, ::mfw::stl::size_t len) noexcept;

MFW_VISIBILITY_LOCAL_POP()

#include <public/mfw/stl/impl/format.ipp>

#endif