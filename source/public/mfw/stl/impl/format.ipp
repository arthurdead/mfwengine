#include <public/mfw/stl/tuple.hpp>

namespace mfw::stl
{
	namespace __public_impl_format_ipp MFW_VISIBILITY_LOCAL
	{
		MFW_IMPLEMENT_TUPLE_TO_VECTOR_INTERNAL(string)
	}

	MFW_IMPLEMENT_AS_TO_FUNC_GLOBAL(string)
	MFW_IMPLEMENT_TUPLE_TO_VECTOR(string, __public_impl_format_ipp::)

	namespace __public_impl_format_ipp MFW_VISIBILITY_LOCAL
	{
		#define _MFW_STL_FORMAT \
			overload_cast_static(bool, MFW_STL_CALL, stl::format, _Sp &, _SVp, const vector<_Sp> &)

		#define _MFW_STL_TO_VECTOR \
			overload_cast_static(bool, MFW_STL_CALL, stl::to_vector_string, const tuple<_Args...> &, vector<_Sp> &)

		template <typename _Sp, typename _SVp, typename... _Args>
		MFW_VISIBILITY_LOCAL bool _format_impl(_Sp &__str, _Sp __fmtstr, _Args &&... __args) noexcept
		{
			tuple<_Args...> __tuple_args{forward<_Args>(__args)...};
			vector<_Sp> __vec_args{};
			if(!_MFW_STL_TO_VECTOR(__tuple_args, __vec_args)) {
				return false;
			}
			return _MFW_STL_FORMAT(__str, __fmtstr, static_cast<const vector<_Sp> &>(__vec_args));
		}
	}

	template <typename _Sp, typename _SVp, typename... _Args>
	bool format(_Sp &__str, _SVp __fmtstr, _Args &&... __args) noexcept
	{ return __public_impl_format_ipp::_format_impl(__str, __fmtstr, forward<_Args>(__args)...); }
}

inline ::mfw::stl::lit_fmt_c operator""_fmt(const char *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_c{__str, __len}; }
inline ::mfw::stl::lit_fmt_w operator""_fmt(const wchar_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_w{__str, __len}; }
#ifdef MFW_CPP_CHAR8_SUPPORTED
inline ::mfw::stl::lit_fmt_u8 operator""_fmt(const char8_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_u8{__str, __len}; }
#endif
inline ::mfw::stl::lit_fmt_u16 operator""_fmt(const char16_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_u16{__str, __len}; }
inline ::mfw::stl::lit_fmt_u32 operator""_fmt(const char32_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_u32{__str, __len}; }