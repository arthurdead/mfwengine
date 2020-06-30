#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/system_error.hpp>

#if MFW_STDC_IS(DEFAULT)
	#include <charconv>
#else
	#error
#endif

namespace mfw::stl
{
	namespace __private_stdint_funcs_cpp MFW_VISIBILITY_LOCAL
	{
		template <typename _T2p, typename _Tp, typename _Sp>
		static void _to_string_impl(_Tp __src, _Sp &__dst, radix_t __base = radix_t::decimal) noexcept
		{
			__dst.clear();
			
			using ::MFW_STD_NAMESPACE::to_chars_result;
			using ::MFW_STD_NAMESPACE::to_chars;

			using __C = typename _Sp::value_type;

			while(true) {
				__C *__begin{reinterpret_cast<__C *>(&(*__dst.begin()))};
				__C *__end{reinterpret_cast<__C *>(&(*__dst.end()))};
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
	}

	MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<int16_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<uint16_t>(__src, __dst, __base); }

	MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<int16_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<uint16_t>(__src, __dst, __base); }

	MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<int32_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<uint32_t>(__src, __dst, __base); }

	MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<int64_t>(__src, __dst, __base); }
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, string &__dst, radix_t __base) noexcept
	{ __private_stdint_funcs_cpp::_to_string_impl<uint64_t>(__src, __dst, __base); }
}