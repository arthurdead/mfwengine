inline ::mfw::stl::string operator""_s(const char *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::string{__ptr, __len}; }
inline ::mfw::stl::wstring operator""_s(const wchar_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::wstring{__ptr, __len}; }
#ifdef MFW_CPP_CHAR8_SUPPORTED
inline ::mfw::stl::u8string operator""_s(const char8_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::u8string{__ptr, __len}; }
#endif
inline ::mfw::stl::u16string operator""_s(const char16_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::u16string{__ptr, __len}; }
inline ::mfw::stl::u32string operator""_s(const char32_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::u32string{__ptr, __len}; }
inline ::mfw::stl::pstring operator""_p(const ::mfw::stl::pchar_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::pstring{__ptr, __ptr+__len}; }