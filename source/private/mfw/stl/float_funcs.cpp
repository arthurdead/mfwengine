#include <public/mfw/stl/float.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cstdio>
	#include <charconv>
	#if MFW_LIBC_IS(MS)
		#define _MFW_CHARCONV_FLOAT_SUPPORTED
	#endif
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_float_funcs_cpp
	{
		template <typename _Rp, typename _Tp, typename _Sp, typename _Cp, typename _Fp>
		static MFW_VISIBILITY_LOCAL void _to_string_impl_printf(_Tp __src, _Sp &__dst, const _Cp *__fmt, _Fp __func) noexcept
		{
			__dst.clear();
		#if MFW_COMPILER_FLAGGED(MSVC)
			MFW_WARNING_SUPPRESS(4774)
		#endif
			int32_t __size{__func(nullptr, 0, __fmt, __src)};
			__dst.resize(static_cast<size_t>(__size));
		#if MFW_COMPILER_FLAGGED(MSVC)
			MFW_WARNING_SUPPRESS(4774)
		#endif
			__func(reinterpret_cast<_Rp *>(__dst.data()), static_cast<size_t>(__size+1), __fmt, __src);
		}
	}

#ifndef _MFW_CHARCONV_FLOAT_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<char>(__src, __dst, "%f", ::MFW_STD_NAMESPACE::snprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<char>(__src, __dst, "%lf", ::MFW_STD_NAMESPACE::snprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<char>(__src, __dst, "%Lf", ::MFW_STD_NAMESPACE::snprintf); }

	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, u8string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<char>(__src, __dst, "%f", ::MFW_STD_NAMESPACE::snprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, u8string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<char>(__src, __dst, "%lf", ::MFW_STD_NAMESPACE::snprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, u8string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<char>(__src, __dst, "%Lf", ::MFW_STD_NAMESPACE::snprintf); }
#else
	#error
#endif

	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, wstring &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%f", ::MFW_STD_NAMESPACE::swprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, wstring &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%lf", ::MFW_STD_NAMESPACE::swprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, wstring &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%Lf", ::MFW_STD_NAMESPACE::swprintf); }

#if MFW_WCHAR_SIZE == 16
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, u16string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%f", ::MFW_STD_NAMESPACE::swprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, u16string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%lf", ::MFW_STD_NAMESPACE::swprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, u16string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%Lf", ::MFW_STD_NAMESPACE::swprintf); }
#elif MFW_WCHAR_SIZE == 32
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, u32string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%f", ::MFW_STD_NAMESPACE::swprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, u32string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%lf", ::MFW_STD_NAMESPACE::swprintf); }
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, u32string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, L"%Lf", ::MFW_STD_NAMESPACE::swprintf); }
#else
	#error
#endif
}