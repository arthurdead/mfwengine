#include <public/mfw/stl/format.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cstdio>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_format_cpp MFW_VISIBILITY_LOCAL
	{
		template <typename _Sp, typename _SVp, typename _Fp>
		static void _format_impl_3(_Sp &__buffer, _SVp __fmtstr, _Fp __func, va_list __args) noexcept
		{
			int32_t __size{__func(nullptr, __fmtstr.data(), __args)};
			__buffer.resize(static_cast<size_t>(__size));
			__func(__buffer.data(), __fmtstr.data(), __args);
		}

		template <typename _Sp, typename _SVp, typename _Fp>
		static void _format_impl_4(_Sp &__buffer, _SVp __fmtstr, _Fp __func, va_list __args) noexcept
		{
			int32_t __size{__func(nullptr, 0, __fmtstr.data(), __args)};
			__buffer.resize(static_cast<size_t>(__size));
			__func(__buffer.data(), __size+1, __fmtstr.data(), __args);
		}

		template <typename _Sp, typename _SVp>
		static bool _format_impl(_Sp &__str, _SVp __fmtstr, const vector<_Sp> &__args) noexcept
		{
			using __C = typename _Sp::value_type;

			typename _SVp::const_iterator __it{__fmtstr.cbegin()};
			size_t __arg{0};
			while(__it != __fmtstr.cend()) {
				bool __isfmt{false};
				if(*__it == static_cast<__C>('{')) {
					__isfmt = true;
					if(__it != __fmtstr.cbegin()) {
						if(*(__it - 1) == static_cast<__C>('\\')) {
							__isfmt = false;
						}
					}
					if(*(__it + 1) != static_cast<__C>('}')) {
						__isfmt = false;
					} else {
						__it++;
					}
				}
				if(__isfmt) {
					if(__arg >= __args.size()) {
						return false;
					}
					__str += __args[__arg];
					__arg++;
				} else {
					__str += *__it;
				}
				__it++;
			}
			
			if(__arg != __args.size()) {
				return false;
			}

			return true;
		}
	}

	MFW_STL_API void MFW_STL_CALL format(string &__buffer, string_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_FLAGGED(UNIX)
		__private_format_cpp::_format_impl_3(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vsprintf, __args);
	#else
		//__private_format_cpp::_format_impl_4(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vsprintf_s, __args);
		MFW_DEBUGBREAK();
	#endif
	}

	MFW_STL_API void MFW_STL_CALL format(wstring &__buffer, wstring_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_FLAGGED(UNIX)
		__private_format_cpp::_format_impl_4(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf, __args);
	#else
		//__private_format_cpp::_format_impl_4(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf_s, __args);
		MFW_DEBUGBREAK();
	#endif
	}

	MFW_STL_API bool MFW_STL_CALL format(string &__str, string_view __fmtstr, const vector<string> &__args) noexcept
	{ return __private_format_cpp::_format_impl(__str, __fmtstr, __args); }
}