#include <public/mfw/stl/format.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cstdio>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_format_cpp
	{
		MFW_VISIBILITY_LOCAL_PUSH()

		template <typename _Rp, typename _Sp, typename _SVp, typename _Fp>
		static void _format_impl_3(_Sp &__buffer, _SVp __fmtstr, _Fp __func, va_list __args) noexcept
		{
			int32_t __size{__func(nullptr, reinterpret_cast<const _Rp *>(__fmtstr.data()), __args)};
			__buffer.resize(static_cast<size_t>(__size));
			__func(reinterpret_cast<_Rp *>(__buffer.data()), reinterpret_cast<const _Rp *>(__fmtstr.data()), __args);
		}

		template <typename _Rp, typename _Sp, typename _SVp, typename _Fp>
		static void _format_impl_4(_Sp &__buffer, _SVp __fmtstr, _Fp __func, va_list __args) noexcept
		{
			int32_t __size{__func(nullptr, 0, reinterpret_cast<const _Rp *>(__fmtstr.data()), __args)};
			__buffer.resize(static_cast<size_t>(__size));
			__func(reinterpret_cast<_Rp *>(__buffer.data()), __size+1, reinterpret_cast<const _Rp *>(__fmtstr.data()), __args);
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

		MFW_VISIBILITY_LOCAL_POP()
	}

	MFW_STL_API void MFW_STL_CALL format(string &__buffer, string_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_IS(MS)
		__private_format_cpp::_format_impl_4<char>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vsprintf_s, __args);
	#else
		__private_format_cpp::_format_impl_3<char>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vsprintf, __args);
	#endif
	}

	MFW_STL_API void MFW_STL_CALL format(u8string &__buffer, u8string_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_IS(MS)
		__private_format_cpp::_format_impl_4<char>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vsprintf_s, __args);
	#else
		__private_format_cpp::_format_impl_3<char>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vsprintf, __args);
	#endif
	}

	MFW_STL_API void MFW_STL_CALL format(wstring &__buffer, wstring_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_IS(MS)
		__private_format_cpp::_format_impl_4<wchar_t>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf_s, __args);
	#else
		__private_format_cpp::_format_impl_4<wchar_t>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf, __args);
	#endif
	}

#if MFW_WCHAR_SIZE == 16
	MFW_STL_API void MFW_STL_CALL format(u16string &__buffer, u16string_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_IS(MS)
		__private_format_cpp::_format_impl_4<wchar_t>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf_s, __args);
	#else
		__private_format_cpp::_format_impl_4<wchar_t>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf, __args);
	#endif
	}
#elif MFW_WCHAR_SIZE == 32
	MFW_STL_API void MFW_STL_CALL format(u32string &__buffer, u32string_view __fmtstr, va_list __args) noexcept
	{
	#if MFW_LIBC_IS(MS)
		__private_format_cpp::_format_impl_4<wchar_t>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf_s, __args);
	#else
		__private_format_cpp::_format_impl_4<wchar_t>(__buffer, __fmtstr, ::MFW_STD_NAMESPACE::vswprintf, __args);
	#endif
	}
#else
	#error
#endif

	MFW_STL_API bool MFW_STL_CALL format(string &__str, string_view __fmtstr, const vector<string> &__args) noexcept
	{ return __private_format_cpp::_format_impl(__str, __fmtstr, __args); }
	MFW_STL_API bool MFW_STL_CALL format(wstring &__str, wstring_view __fmtstr, const vector<wstring> &__args) noexcept
	{ return __private_format_cpp::_format_impl(__str, __fmtstr, __args); }
	MFW_STL_API bool MFW_STL_CALL format(u8string &__str, u8string_view __fmtstr, const vector<u8string> &__args) noexcept
	{ return __private_format_cpp::_format_impl(__str, __fmtstr, __args); }
	MFW_STL_API bool MFW_STL_CALL format(u16string &__str, u16string_view __fmtstr, const vector<u16string> &__args) noexcept
	{ return __private_format_cpp::_format_impl(__str, __fmtstr, __args); }
	MFW_STL_API bool MFW_STL_CALL format(u32string &__str, u32string_view __fmtstr, const vector<u32string> &__args) noexcept
	{ return __private_format_cpp::_format_impl(__str, __fmtstr, __args); }
}