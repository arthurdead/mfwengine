#ifndef _MFW_PUBLIC_STL_FORMAT_HPP
#define _MFW_PUBLIC_STL_FORMAT_HPP

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
	MFW_STL_API void MFW_STL_CALL format(string &__string, string_view __format, va_list __args) noexcept;
	MFW_STL_API void MFW_STL_CALL format(wstring &__string, wstring_view __format, va_list __args) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL format(u8string &__string, u8string_view __format, va_list __args) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL format(u16string &__string, u16string_view __format, va_list __args) noexcept;
	MFW_STL_API void MFW_STL_CALL format(u32string &__string, u32string_view __format, va_list __args) noexcept;

	MFW_STL_API bool MFW_STL_CALL format(string &__string, string_view __format, const vector<string> &__args) noexcept;
	MFW_STL_API bool MFW_STL_CALL format(wstring &__string, wstring_view __format, const vector<wstring> &__args) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL format(u8string &__string, u8string_view __format, const vector<u8string> &__args) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL format(u16string &__string, u16string_view __format, const vector<u16string> &__args) noexcept;
	MFW_STL_API bool MFW_STL_CALL format(u32string &__string, u32string_view __format, const vector<u32string> &__args) noexcept;

	template <typename _Sp, typename _SVp, typename... _Args>
	bool format(_Sp &__str, _SVp __fmt, _Args &&... __args) noexcept;

	namespace __public_format_hpp MFW_VISIBILITY_LOCAL
	{
		template <typename _Sp, typename _SVp, typename _Cp>
		struct MFW_VISIBILITY_LOCAL lit_fmt_base
		{
			lit_fmt_base(const _Cp *__str, size_t __len) noexcept
				: _M_fmt{__str, __len} {}

			template <typename... _Args>
			_Sp operator()(_Args &&... __args) const noexcept {
				_Sp __str{};
				format(__str, _M_fmt, forward<_Args>(__args)...);
				return __str;
			}

		private:
			_SVp _M_fmt{};
		};
	}

	using lit_fmt_c = __public_format_hpp::lit_fmt_base<string, string_view, char>;
	using lit_fmt_w = __public_format_hpp::lit_fmt_base<wstring, wstring_view, wchar_t>;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	using lit_fmt_u8 = __public_format_hpp::lit_fmt_base<u8string, u8string_view, char8_t>;
#endif
	using lit_fmt_u16 = __public_format_hpp::lit_fmt_base<u16string, u16string_view, char16_t>;
	using lit_fmt_u32 = __public_format_hpp::lit_fmt_base<u32string, u32string_view, char32_t>;

	MFW_DECLARE_AS_TO_FUNC_GLOBAL(string)
	MFW_DECLARE_AS_TO_CHECK_FUNCS(string)
}

::mfw::stl::lit_fmt_c operator""_fmt(const char *str, ::mfw::stl::size_t len) noexcept;
::mfw::stl::lit_fmt_w operator""_fmt(const wchar_t *str, ::mfw::stl::size_t len) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
::mfw::stl::lit_fmt_u8 operator""_fmt(const char8_t *str, ::mfw::stl::size_t len) noexcept;
#endif
::mfw::stl::lit_fmt_u16 operator""_fmt(const char16_t *str, ::mfw::stl::size_t len) noexcept;
::mfw::stl::lit_fmt_u32 operator""_fmt(const char32_t *str, ::mfw::stl::size_t len) noexcept;

#include <public/mfw/stl/impl/format.ipp>

#endif