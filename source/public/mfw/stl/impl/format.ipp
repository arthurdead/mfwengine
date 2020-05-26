#include <public/mfw/stl/tuple.hpp>

namespace mfw::stl
{
	namespace literals
	{
		inline __format_internal::lit_fmt_c operator""_fmt(const ucchar_t *str, size_t len)
		{ return {str, len}; }
		inline __format_internal::lit_fmt_w operator""_fmt(const uwchar_t *str, size_t len)
		{ return {str, len}; }
	}

	namespace __format_internal
	{
		MFW_IMPLEMENT_TUPLE_TO_VECTOR_INTERNAL(string)
	}

	MFW_IMPLEMENT_AS_TO_FUNC_GLOBAL(string)
	MFW_IMPLEMENT_TUPLE_TO_VECTOR(string, __format_internal::)

	namespace __format_internal
	{
		#define __MFW_STL_FORMAT \
			overload_cast_static(bool, MFW_STL_CALL, stl::format, S &, SV, const vector<S> &)

		#define __MFW_STL_TO_VECTOR \
			overload_cast_static(bool, MFW_STL_CALL, stl::to_vector_string, const tuple<Args...> &, vector<S> &)

		template <typename S, typename SV, typename ...Args>
		bool format(S &str, SV fmtstr, Args &&... args)
		{
			tuple<Args...> tuple_args{forward<Args>(args)...};
			vector<S> vec_args{};
			if(!__MFW_STL_TO_VECTOR(tuple_args, vec_args)) {
				return false;
			}
			return __MFW_STL_FORMAT(str, fmtstr, static_cast<const vector<S> &>(vec_args));
		}

		template <typename S, typename SV, typename C>
		lit_fmt_base<S, SV, C>::lit_fmt_base(const C *str, size_t len)
			: fmtstr{str, len}
		{

		}

		template <typename S, typename SV, typename C> template <typename ...Args>
		S lit_fmt_base<S, SV, C>::operator()(Args &&... args) const
		{
			S str{};
			__format_internal::format(str, fmtstr, forward<Args>(args)...);
			return str;
		}
	}

	template <typename ...Args>
	bool format(ucstring &str, ucstring_view fmtstr, Args &&... args)
	{ return __format_internal::format(str, fmtstr, forward<Args>(args)...); }

	template <typename ...Args>
	bool format(uwstring &str, uwstring_view fmtstr, Args &&... args)
	{ return __format_internal::format(str, fmtstr, forward<Args>(args)...); }
}