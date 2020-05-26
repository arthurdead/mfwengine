#ifndef __MFW_PUBLIC_STL_FORMAT_H
#define __MFW_PUBLIC_STL_FORMAT_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <cstdarg>
#else
	#error
#endif

namespace mfw::stl
{
	MFW_STL_API void MFW_STL_CALL format(ucstring &string, ucstring_view format, va_list args);
	MFW_STL_API void MFW_STL_CALL format(uwstring &string, uwstring_view format, va_list args);

	MFW_STL_API bool MFW_STL_CALL format(ucstring &string, ucstring_view format, const vector<ucstring> &args);
	MFW_STL_API bool MFW_STL_CALL format(uwstring &string, uwstring_view format, const vector<uwstring> &args);

	template <typename ...Args>
	bool format(ucstring &str, ucstring_view fmtstr, Args &&... args);

	template <typename ...Args>
	bool format(uwstring &str, uwstring_view fmtstr, Args &&... args);

	namespace __format_internal
	{
		template <typename S, typename SV, typename C>
		struct lit_fmt_base
		{
			lit_fmt_base(const C *str, size_t len);

			template <typename ...Args>
			S operator()(Args &&... args) const;

		private:
			SV fmtstr{};
		};

		using lit_fmt_c = lit_fmt_base<ucstring, ucstring_view, ucchar_t>;
		using lit_fmt_w = lit_fmt_base<uwstring, uwstring_view, uwchar_t>;
	}

	MFW_DECLARE_AS_TO_FUNC_GLOBAL(string)
	MFW_DECLARE_AS_TO_CHECK_FUNCS(string)

	namespace literals
	{
		__format_internal::lit_fmt_c operator""_fmt(const ucchar_t *str, size_t len);
		__format_internal::lit_fmt_w operator""_fmt(const uwchar_t *str, size_t len);
	}
}

#include <public/mfw/stl/impl/format.ipp>

#endif