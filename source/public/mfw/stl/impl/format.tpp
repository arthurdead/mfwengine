#include <public/mfw/stl/tuple.hpp>

namespace mfw::stl
{
	namespace __public_impl_format_tpp
	{
		MFW_IMPLEMENT_TUPLE_TO_VECTOR_INTERNAL(string)
	}

	MFW_IMPLEMENT_AS_TO_FUNC_GLOBAL(string)
	MFW_IMPLEMENT_TUPLE_TO_VECTOR(string, __public_impl_format_tpp::)

	namespace __public_impl_format_tpp
	{
		#define _MFW_STL_FORMAT \
			overload_cast_static(bool, MFW_STL_CALL, stl::format, _Sp &, type_view_t<_Sp>, vector_view<_Sp>)

		#define _MFW_STL_TO_VECTOR \
			overload_cast_static(bool, MFW_STL_CALL, stl::to_vector_string, tuple_view<_Args...>, vector<_Sp> &)

		template <typename _Sp, typename... _Args>
		static MFW_VISIBILITY_LOCAL bool _format_impl(_Sp &__str, type_view_t<_Sp> __fmtstr, _Args &&... __args) noexcept
		{
			tuple<_Args...> __tuple_args{forward<_Args>(__args)...};
			vector<_Sp> __vec_args{};
			if(!_MFW_STL_TO_VECTOR(__tuple_args, __vec_args)) {
				return false;
			}
			return _MFW_STL_FORMAT(__str, __fmtstr, static_cast<vector_view<_Sp>>(__vec_args));
		}
	}

	namespace __public_format_hpp
	{
		template <typename _Sp>
		lit_fmt_base<_Sp>::lit_fmt_base(const char_type *__str, size_t __len) noexcept
			: _M_fmt{__str, __len} {}

		template <typename _Sp>
		template <typename... _Args>
		_Sp lit_fmt_base<_Sp>::operator()(_Args &&... __args) const noexcept {
			string_type __str{};
			format(__str, _M_fmt, forward<_Args>(__args)...);
			return __str;
		}
	}

	template <typename _Sp, typename... _Args>
	MFW_VISIBILITY_LOCAL bool format(_Sp &__str, type_view_t<_Sp> __fmtstr, _Args &&... __args) noexcept
	{ return __public_impl_format_ipp::_format_impl(__str, __fmtstr, forward<_Args>(__args)...); }
}