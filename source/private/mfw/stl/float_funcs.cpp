#include <public/mfw/stl/float.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <cstdio>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_float_funcs_cpp MFW_VISIBILITY_LOCAL
	{
		MFW_MESSAGE("replace both with tochars on msvc")
		template <typename _Tp, typename _Sp, typename _Cp, typename _Fp>
		static void _to_string_impl(_Tp __src, _Sp &__dst, const _Cp *__fmt, _Fp __func) noexcept
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
			__func(__dst.data(), static_cast<size_t>(__size+1), __fmt, __src);
		}
	}

	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl(__src, __dst, "%f", ::MFW_STD_NAMESPACE::snprintf); }

	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl(__src, __dst, "%lf", ::MFW_STD_NAMESPACE::snprintf); }

	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, string &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl(__src, __dst, "%Lf", ::MFW_STD_NAMESPACE::snprintf); }

	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, wstring &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl(__src, __dst, L"%f", ::MFW_STD_NAMESPACE::swprintf); }

	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, wstring &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl(__src, __dst, L"%lf", ::MFW_STD_NAMESPACE::swprintf); }

	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, wstring &__dst) noexcept
	{ __private_float_funcs_cpp::_to_string_impl(__src, __dst, L"%Lf", ::MFW_STD_NAMESPACE::swprintf); }
}