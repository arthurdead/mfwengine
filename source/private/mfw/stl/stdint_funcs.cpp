#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/system_error.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <charconv>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __stdint_funcs_internal
	{
		template <typename S, typename S2>
		void __to_string_impl(S src, ucstring &dst, int32_t base = 10)
		{
			dst.clear();
			
			using ::MFW_STD_NAMESPACE::to_chars_result;
			using ::MFW_STD_NAMESPACE::to_chars;

			while(true) {
				char *begin{reinterpret_cast<char *>(&(*dst.begin()))};
				char *end{reinterpret_cast<char *>(&(*dst.end()))};
				to_chars_result res{to_chars(begin, end, static_cast<S2>(src), base)};

				if(res.ec == errc::value_too_large) {
					dst.resize(dst.size()+1);
				} else {
					break;
				}
			}

			if(base == 16) {
				dst.insert(0, u8"0x", 2);
				dst.erase(dst.end()-2);
			}
		}

		template <typename S>
		void to_string(S src, ucstring &dst, int32_t base = 10)
		{ __to_string_impl<S, S>(src, dst, base); }

		template <>
		void to_string(int8_t src, ucstring &dst, int32_t base)
		{ __to_string_impl<int8_t, int16_t>(src, dst, base); }

		template <>
		void to_string(uint8_t src, ucstring &dst, int32_t base)
		{ __to_string_impl<uint8_t, uint16_t>(src, dst, base); }
	}

	MFW_STL_API void MFW_STL_CALL to_string(int8_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(uint8_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(int16_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(uint16_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(int32_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(uint32_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(int64_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}

	MFW_STL_API void MFW_STL_CALL to_string(uint64_t src, ucstring &dst, int base)
	{
		__stdint_funcs_internal::to_string(src, dst, base);
	}
#else
	#error
#endif
}