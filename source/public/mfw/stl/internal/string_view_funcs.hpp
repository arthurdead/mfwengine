#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/float.hpp>

namespace mfw::stl
{
	MFW_STL_API void MFW_STL_CALL to_string(string_view __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(string_view __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(string_view __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(string_view __src, u32string &__dst) noexcept;

#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(u8string_view __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(u8string_view __src, wstring &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(u8string_view __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(u8string_view __src, u32string &__dst) noexcept;
#endif

	MFW_STL_API void MFW_STL_CALL to_string(wstring_view __src, string &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(wstring_view __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(wstring_view __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(wstring_view __src, u32string &__dst) noexcept;
	
	MFW_STL_API void MFW_STL_CALL to_string(u16string_view __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(u16string_view __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(u16string_view __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(u16string_view __src, u32string &__dst) noexcept;

	MFW_STL_API void MFW_STL_CALL to_string(u32string_view __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(u32string_view __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(u32string_view __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(u32string_view __src, u16string &__dst) noexcept;

#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
	void to_string(string_view __src, pstring &__dst) noexcept;
#else
	void to_string(wstring_view __src, pstring &__dst) noexcept;
#endif
#ifdef MFW_CPP_CHAR8_SUPPORTED
	void to_string(u8string_view __src, pstring &__dst) noexcept;
#endif
	void to_string(u16string_view __src, pstring &__dst) noexcept;
	void to_string(u32string_view __src, pstring &__dst) noexcept;

	bool to_bool(string_view __src, bool &__dst) noexcept;
	bool to_bool(wstring_view __src, bool &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	bool to_bool(u8string_view __src, bool &__dst) noexcept;
#endif
	bool to_bool(u16string_view __src, bool &__dst) noexcept;
	bool to_bool(u32string_view __src, bool &__dst) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int8_t &__dst, radix_t __base = radix_t::decimal) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint8_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint8_t &__dst, radix_t __base = radix_t::decimal) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int32_t &__dst, radix_t __base = radix_t::decimal) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint32_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint32_t &__dst, radix_t __base = radix_t::decimal) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, int64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, int64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, int64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, int64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, int64_t &__dst, radix_t __base = radix_t::decimal) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_int(string_view __src, uint64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(wstring_view __src, uint64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_int(u8string_view __src, uint64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_int(u16string_view __src, uint64_t &__dst, radix_t __base = radix_t::decimal) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_int(u32string_view __src, uint64_t &__dst, radix_t __base = radix_t::decimal) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_float(string_view __src, float32_t &__dst) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_float(wstring_view __src, float32_t &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_float(u8string_view __src, float32_t &__dst) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_float(u16string_view __src, float32_t &__dst) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_float(u32string_view __src, float32_t &__dst) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_float(string_view __src, float64_t &__dst) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_float(wstring_view __src, float64_t &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_float(u8string_view __src, float64_t &__dst) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_float(u16string_view __src, float64_t &__dst) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_float(u32string_view __src, float64_t &__dst) noexcept;

	MFW_STL_API bool MFW_STL_CALL to_float(string_view __src, float80_t &__dst) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_float(wstring_view __src, float80_t &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL to_float(u8string_view __src, float80_t &__dst) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL to_float(u16string_view __src, float80_t &__dst) noexcept;
	MFW_STL_API bool MFW_STL_CALL to_float(u32string_view __src, float80_t &__dst) noexcept;

	MFW_STL_API void MFW_STL_CALL to_lower(string_view __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_lower(wstring_view __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_lower(u8string_view __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_lower(u16string_view __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_lower(u32string_view __src, u32string &__dst) noexcept;

	MFW_STL_API void MFW_STL_CALL to_upper(string_view __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_upper(wstring_view __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_upper(u8string_view __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_upper(u16string_view __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_upper(u32string_view __src, u32string &__dst) noexcept;
	
	MFW_STL_API bool MFW_STL_CALL matches_pattern(string_view __str, string_view __pattern) noexcept;
	MFW_STL_API bool MFW_STL_CALL matches_pattern(wstring_view __str, wstring_view __pattern) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API bool MFW_STL_CALL matches_pattern(u8string_view __str, u8string_view __pattern) noexcept;
#endif
	MFW_STL_API bool MFW_STL_CALL matches_pattern(u16string_view __str, u16string_view __pattern) noexcept;
	MFW_STL_API bool MFW_STL_CALL matches_pattern(u32string_view __str, u32string_view __pattern) noexcept;
}

constexpr ::mfw::stl::string_view operator""_sv(const char *__ptr, ::mfw::stl::size_t __len) noexcept;
constexpr ::mfw::stl::wstring_view operator""_sv(const wchar_t *__ptr, ::mfw::stl::size_t __len) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
constexpr ::mfw::stl::u8string_view operator""_sv(const char8_t *__ptr, ::mfw::stl::size_t __len) noexcept;
#endif
constexpr ::mfw::stl::u16string_view operator""_sv(const char16_t *__ptr, ::mfw::stl::size_t __len) noexcept;
constexpr ::mfw::stl::u32string_view operator""_sv(const char32_t *__ptr, ::mfw::stl::size_t __len) noexcept;

#include <public/mfw/stl/impl/string_view_funcs.ipp>