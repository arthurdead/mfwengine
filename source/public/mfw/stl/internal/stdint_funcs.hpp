#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
	MFW_VISIBILITY_LOCAL_PUSH()

	extern void to_string(bool __src, string &dst) noexcept;
	extern void to_string(bool __src, wstring &dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern void to_string(bool __src, u8string &dst) noexcept;
#endif
	extern void to_string(bool __src, u16string &dst) noexcept;
	extern void to_string(bool __src, u32string &dst) noexcept;

	MFW_VISIBILITY_LOCAL_POP()

	extern MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int8_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;

	extern MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint8_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;
	
	extern MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int16_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;

	extern MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint16_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;

	extern MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int32_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;

	extern MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint32_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;

	extern MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(int64_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;

	extern MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, wstring &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	extern MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, u8string &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	extern MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, u16string &__dst, radix_t __base = radix_t::decimal) noexcept;
	extern MFW_STL_API void MFW_STL_CALL to_string(uint64_t __src, u32string &__dst, radix_t __base = radix_t::decimal) noexcept;
}

#include <public/mfw/stl/impl/stdint_funcs.tpp>