#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string_view.hpp>
#include <public/mfw/stl/stdint.hpp>

namespace mfw::stl
{
	MFW_STL_API size_t MFW_STL_CALL replace_all(string &__str, string_view __from, string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(string &__str, char __from, char __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(string &__str, char __from, string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(string &__str, string_view __from, char __to) noexcept;

	MFW_STL_API size_t MFW_STL_CALL replace_all(wstring &__str, wstring_view __from, wstring_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(wstring &__str, wchar_t __from, wchar_t __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(wstring &__str, wchar_t __from, wstring_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(wstring &__str, wstring_view __from, wchar_t __to) noexcept;

	MFW_STL_API size_t MFW_STL_CALL replace_all(u8string &__str, u8string_view __from, u8string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u8string &__str, char8_t __from, char8_t __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u8string &__str, char8_t __from, u8string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u8string &__str, u8string_view __from, char8_t __to) noexcept;

	MFW_STL_API size_t MFW_STL_CALL replace_all(u16string &__str, u16string_view __from, u16string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u16string &__str, char16_t __from, char16_t __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u16string &__str, char16_t __from, u16string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u16string &__str, u16string_view __from, char16_t __to) noexcept;

	MFW_STL_API size_t MFW_STL_CALL replace_all(u32string &__str, u32string_view __from, u32string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u32string &__str, char32_t __from, char32_t __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u32string &__str, char32_t __from, u32string_view __to) noexcept;
	MFW_STL_API size_t MFW_STL_CALL replace_all(u32string &__str, u32string_view __from, char32_t __to) noexcept;

#ifdef _MFW_STL_FILESYSTEM_WIDE_CHAR
	void to_string(const pstring &__src, string &__dst) noexcept;
#else
	void to_string(const pstring &__src, wstring &__dst) noexcept;
#endif
	void to_string(const pstring &__src, u8string &__dst) noexcept;
	void to_string(const pstring &__src, u16string &__dst) noexcept;
	void to_string(const pstring &__src, u32string &__dst) noexcept;
}

::mfw::stl::string operator""_s(const char *__ptr, ::mfw::stl::size_t __len) noexcept;
::mfw::stl::wstring operator""_s(const wchar_t *__ptr, ::mfw::stl::size_t __len) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
::mfw::stl::u8string operator""_s(const char8_t *__ptr, ::mfw::stl::size_t __len) noexcept;
#endif
::mfw::stl::u16string operator""_s(const char16_t *__ptr, ::mfw::stl::size_t __len) noexcept;
::mfw::stl::u32string operator""_s(const char32_t *__ptr, ::mfw::stl::size_t __len) noexcept;
::mfw::stl::pstring operator""_p(const ::mfw::stl::pchar_t *__ptr, ::mfw::stl::size_t __len) noexcept;

#include <public/mfw/stl/impl/string_funcs.ipp>

namespace mfw::stl
{
	MFW_DECLARE_AS_FUNC_GLOBAL(string)
}