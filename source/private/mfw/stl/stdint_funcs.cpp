#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/system_error.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <charconv>
	#include <cinttypes>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_stdint_funcs_cpp MFW_VISIBILITY_LOCAL
	{
		template <typename _Rp, typename _T2p, typename _Tp, typename _Sp>
		static void _to_string_impl_tc(_Tp __src, _Sp &__dst, radix_t __base) noexcept
		{
			__dst.clear();
			
			using ::MFW_STD_NAMESPACE::to_chars_result;
			using ::MFW_STD_NAMESPACE::to_chars;

			using __C = typename _Sp::value_type;

			while(true) {
				_Rp *__begin{reinterpret_cast<_Rp *>(&(*__dst.begin()))};
				_Rp *__end{reinterpret_cast<_Rp *>(&(*__dst.end()))};
				to_chars_result __res{to_chars(__begin, __end, static_cast<_T2p>(__src), static_cast<int32_t>(__base))};

				if(__res.ec == errc::value_too_large) {
					__dst.resize(__dst.size()+1);
				} else {
					break;
				}
			}

			if(__base == radix_t::hexadecimal) {
				__dst.insert(0, 1, static_cast<__C>('0'));
				__dst.insert(1, 1, static_cast<__C>('x'));
				__dst.erase(__dst.end()-2);
			}
		}

		template <typename _Rp, typename _Tp, typename _Sp, typename _Cp, typename _Fp>
		static void _to_string_impl_printf(_Tp __src, _Sp &__dst, const _Cp *__fmt, _Fp __func) noexcept
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

	MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, schar_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uchar_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, int16_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uint16_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, int32_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uint32_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, int64_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uint64_t>(__src, __dst, __base); }

	MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, schar_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uchar_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, int16_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uint16_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, int32_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uint32_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, int64_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, u8string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl_tc<char, uint64_t>(__src, __dst, __base); }

	#define _MFW_STDINT_TO_STRING_W_HELPER(d, s) \
		const wchar_t *__fmt{nullptr}; \
		switch(__base) { \
			case radix_t::decimal: { __fmt = L"%" MFW_MACRO_CONCATENATE(L, PRI##d##s); break; } \
			case radix_t::octal: { __fmt = L"%" MFW_MACRO_CONCATENATE(L, PRIo##s); break; } \
			case radix_t::hexadecimal: { __fmt = L"%" MFW_MACRO_CONCATENATE(L, PRIx##s); break; } \
		} \
		__private_stdint_funcs_cpp::_to_string_impl_printf<wchar_t>(__src, __dst, __fmt, ::MFW_STD_NAMESPACE::swprintf);

	MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 8) }
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 8) }
	MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 16) }
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 16) }
	MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 32) }
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 32) }
	MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 64) }
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, wstring &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 64) }

#if MFW_WCHAR_SIZE == 16
	MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 8) }
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 8) }
	MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 16) }
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 16) }
	MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 32) }
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 32) }
	MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 64) }
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, u16string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 64) }
#elif MFW_WCHAR_SIZE == 32
	MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 8) }
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 8) }
	MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 16) }
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 16) }
	MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 32) }
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 32) }
	MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(d, 64) }
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, u32string &__dst, radix_t __base) noexcept
	{ _MFW_STDINT_TO_STRING_W_HELPER(u, 64) }
#else
	#error
#endif
}