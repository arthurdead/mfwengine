#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/limits.hpp>
#include <public/mfw/stl/system_error.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <charconv>
	#include <cctype>
	#if MFW_LIBC_IS(MS)
		#define _MFW_CHARCONV_FLOAT_SUPPORTED
	#endif
#else
	#error
#endif

#if MFW_LIBC_FLAGGED(UNIX)
	#include <fnmatch.h>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_string_view_funcs_cpp
	{
		MFW_VISIBILITY_LOCAL_PUSH()

		template <typename _Rp, typename _T2p, typename _SVp, typename _Tp>
		static bool _to_int_impl_fc(_SVp __src, _Tp &__dst, radix_t __base) noexcept
		{
			using __C = typename _SVp::value_type;

			size_t __len{__src.length()};
			if(__len == 4) {
				if(__src[0] == static_cast<__C>('t') &&
					__src[1] == static_cast<__C>('r') &&
					__src[2] == static_cast<__C>('u') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = static_cast<_Tp>(1);
					return true;
				}
			} else if(__len == 5) {
				if(__src[0] == static_cast<__C>('f') &&
					__src[1] == static_cast<__C>('a') &&
					__src[2] == static_cast<__C>('l') &&
					__src[3] == static_cast<__C>('s') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = static_cast<_Tp>(0);
					return true;
				}
			} else if(__len == 0) {
				__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
				return false;
			}
			
			using ::MFW_STD_NAMESPACE::from_chars_result;
			using ::MFW_STD_NAMESPACE::from_chars;

			const _Rp *__begin{reinterpret_cast<const _Rp *>(&(*__src.cbegin()))};
			const _Rp *__end{reinterpret_cast<const _Rp *>(&(*__src.cend()))};
			_T2p __tmp{static_cast<_T2p>(0)};
			from_chars_result __res{from_chars(__begin, __end, __tmp, static_cast<int32_t>(__base))};
			__dst = static_cast<_Tp>(__tmp);

			if(__res.ec == errc::invalid_argument || __res.ptr == __begin) {
				__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
				return false;
			} else {
				if(__res.ptr != __end) {
					MFW_MESSAGE("TODO check for f/u/b/i8/16/32/64")
					__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
					return false;
				}
				return true;
			}
		}

		template <typename _Rp, typename _T2p, typename _SVp, typename _Tp, typename _Fp>
		static bool _to_int_impl_sto(_SVp __src, _Tp &__dst, _Fp __func, radix_t __base) noexcept
		{
			using __C = typename _SVp::value_type;

			size_t __len{__src.length()};
			if(__len == 4) {
				if(__src[0] == static_cast<__C>('t') &&
					__src[1] == static_cast<__C>('r') &&
					__src[2] == static_cast<__C>('u') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = static_cast<_Tp>(1);
					return true;
				}
			} else if(__len == 5) {
				if(__src[0] == static_cast<__C>('f') &&
					__src[1] == static_cast<__C>('a') &&
					__src[2] == static_cast<__C>('l') &&
					__src[3] == static_cast<__C>('s') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = static_cast<_Tp>(0);
					return true;
				}
			} else if(__len == 0) {
				__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
				return false;
			}

			__C *__end{nullptr};
			const __C *__begin{&(*__src.cbegin())};
			_T2p __tmp{__func(reinterpret_cast<const _Rp *>(__begin), reinterpret_cast<_Rp **>(&__end), static_cast<int32_t>(__base))};
			if(__end == __begin) {
				__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
				return false;
			} else {
				if(__end != &(*__src.cend())) {
					MFW_MESSAGE("TODO check for f/u/b/i8/16/32/64")
					__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
					return false;
				}
				__dst = static_cast<_Tp>(__tmp);
				return true;
			}
		}

		template <typename _Rp, typename _T2p, typename _SVp, typename _Tp, typename _Fp>
		static bool _to_float_impl_sto(_SVp __src, _Tp &__dst, _Fp __func) noexcept
		{
			using __C = typename _SVp::value_type;

			size_t __len{__src.length()};
			if(__len == 4) {
				if(__src[0] == static_cast<__C>('t') &&
					__src[1] == static_cast<__C>('r') &&
					__src[2] == static_cast<__C>('u') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = static_cast<_Tp>(1.0f);
					return true;
				}
			} else if(__len == 5) {
				if(__src[0] == static_cast<__C>('f') &&
					__src[1] == static_cast<__C>('a') &&
					__src[2] == static_cast<__C>('l') &&
					__src[3] == static_cast<__C>('s') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = static_cast<_Tp>(0.0f);
					return true;
				}
			} else if(__len == 0) {
				__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
				return false;
			}
			
			__C *__end{nullptr};
			const __C *__start{&(*__src.cbegin())};
			_T2p __tmp{__func(reinterpret_cast<const _Rp *>(__start), reinterpret_cast<_Rp **>(&__end))};
			if(__end == __start) {
				__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
				return false;
			} else {
				if(__end != &(*__src.cend())) {
					MFW_MESSAGE("TODO check for f/b/u/i8/16/32/64")
					__dst = static_cast<_Tp>(numeric_limits<_T2p>::max());
					return false;
				}
				__dst = static_cast<_Tp>(__tmp);
				return true;
			}
		}

		template <typename _SVp, typename _Sp>
		static void _to_upper_impl(_SVp __src, _Sp &__dst) noexcept
		{
			using __C = _SVp::value_type;

			__dst = __src;
			transform(__dst.begin(), __dst.end(), __dst.begin(), [](__C __c) noexcept -> __C {
				return static_cast<__C>(::MFW_STD_NAMESPACE::toupper(static_cast<int32_t>(__c)));
			});
		}

		template <typename _Rp, typename _SVp>
		static bool _matches_pattern_impl(_SVp __str, _SVp __pattern) noexcept
		{
			if(__str == __pattern) {
				return true;
			}
			
		#if MFW_LIBC_FLAGGED(UNIX)
			return (fnmatch(reinterpret_cast<const _Rp *>(__pattern.data()), reinterpret_cast<const _Rp *>(__str.data()), FNM_CASEFOLD
			#if !MFW_LIBC_IS(MUSL)
			|FNM_EXTMATCH
			#endif
			) != FNM_NOMATCH);
		#else
			#error
		#endif
		}

		MFW_VISIBILITY_LOCAL_POP()
	}

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, schar_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uchar_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, int16_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uint16_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, int32_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uint32_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, int64_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uint64_t>(__src, __dst, __base); }
	
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, schar_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uchar_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, int16_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uint16_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, int32_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uint32_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, int64_t>(__src, __dst, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_fc<char, uint64_t>(__src, __dst, __base); }

	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoll, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoull, __base); }

#if MFW_WCHAR_SIZE == 16
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoll, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoull, __base); }
#elif MFW_WCHAR_SIZE == 32
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint8_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint16_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstol, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint32_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoul, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, long long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoll, __base); }
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint64_t &__dst, radix_t __base) noexcept
	{ return __private_string_view_funcs_cpp::_to_int_impl_sto<wchar_t, unsigned long long>(__src, __dst, ::MFW_STD_NAMESPACE::wcstoull, __base); }
#else
	#error
#endif

#ifndef _MFW_CHARCONV_FLOAT_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_float(string_view __src, float32_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<char, float>(__src, __dst, ::MFW_STD_NAMESPACE::strtof); }
	MFW_STL_API bool MFW_STL_CALL to_float(string_view __src, float64_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<char, double>(__src, __dst, ::MFW_STD_NAMESPACE::strtod); }
	MFW_STL_API bool MFW_STL_CALL to_float(string_view __src, float80_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<char, long double>(__src, __dst, ::MFW_STD_NAMESPACE::strtold); }

	MFW_STL_API bool MFW_STL_CALL to_float(u8string_view __src, float32_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<char, float>(__src, __dst, ::MFW_STD_NAMESPACE::strtof); }
	MFW_STL_API bool MFW_STL_CALL to_float(u8string_view __src, float64_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<char, double>(__src, __dst, ::MFW_STD_NAMESPACE::strtod); }
	MFW_STL_API bool MFW_STL_CALL to_float(u8string_view __src, float80_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<char, long double>(__src, __dst, ::MFW_STD_NAMESPACE::strtold); }
#else
	#error
#endif

	MFW_STL_API bool MFW_STL_CALL to_float(wstring_view __src, float32_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, float>(__src, __dst, ::MFW_STD_NAMESPACE::wcstof); }
	MFW_STL_API bool MFW_STL_CALL to_float(wstring_view __src, float64_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, double>(__src, __dst, ::MFW_STD_NAMESPACE::wcstod); }
	MFW_STL_API bool MFW_STL_CALL to_float(wstring_view __src, float80_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, long double>(__src, __dst, ::MFW_STD_NAMESPACE::wcstold); }

#if MFW_WCHAR_SIZE == 16
	MFW_STL_API bool MFW_STL_CALL to_float(u16string_view __src, float32_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, float>(__src, __dst, ::MFW_STD_NAMESPACE::wcstof); }
	MFW_STL_API bool MFW_STL_CALL to_float(u16string_view __src, float64_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, double>(__src, __dst, ::MFW_STD_NAMESPACE::wcstod); }
	MFW_STL_API bool MFW_STL_CALL to_float(u16string_view __src, float80_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, long double>(__src, __dst, ::MFW_STD_NAMESPACE::wcstold); }
#elif MFW_WCHAR_SIZE == 32
	MFW_STL_API bool MFW_STL_CALL to_float(u32string_view __src, float32_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, float>(__src, __dst, ::MFW_STD_NAMESPACE::wcstof); }
	MFW_STL_API bool MFW_STL_CALL to_float(u32string_view __src, float64_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, double>(__src, __dst, ::MFW_STD_NAMESPACE::wcstod); }
	MFW_STL_API bool MFW_STL_CALL to_float(u32string_view __src, float80_t &__dst) noexcept
	{ return __private_string_view_funcs_cpp::_to_float_impl_sto<wchar_t, long double>(__src, __dst, ::MFW_STD_NAMESPACE::wcstold); }
#else
	#error
#endif

	MFW_STL_API void MFW_STL_CALL to_upper(string_view __src, string &__dst) noexcept
	{ __private_string_view_funcs_cpp::_to_upper_impl(__src, __dst); }
	MFW_STL_API void MFW_STL_CALL to_upper(wstring_view __src, wstring &__dst) noexcept
	{ __private_string_view_funcs_cpp::_to_upper_impl(__src, __dst); }
	MFW_STL_API void MFW_STL_CALL to_upper(u8string_view __src, u8string &__dst) noexcept
	{ __private_string_view_funcs_cpp::_to_upper_impl(__src, __dst); }
	MFW_STL_API void MFW_STL_CALL to_upper(u16string_view __src, u16string &__dst) noexcept
	{ __private_string_view_funcs_cpp::_to_upper_impl(__src, __dst); }
	MFW_STL_API void MFW_STL_CALL to_upper(u32string_view __src, u32string &__dst) noexcept
	{ __private_string_view_funcs_cpp::_to_upper_impl(__src, __dst); }
	
	MFW_STL_API bool MFW_STL_CALL matches_pattern(string_view __str, string_view __pattern) noexcept
	{ return __private_string_view_funcs_cpp::_matches_pattern_impl<char>(__str, __pattern); }
	MFW_STL_API bool MFW_STL_CALL matches_pattern(u8string_view __str, u8string_view __pattern) noexcept
	{ return __private_string_view_funcs_cpp::_matches_pattern_impl<char>(__str, __pattern); }
}