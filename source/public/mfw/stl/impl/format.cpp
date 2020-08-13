#include <public/mfw/stl/format.hpp>

MFW_VISIBILITY_LOCAL_PUSH()

::mfw::stl::lit_fmt_c operator""_fmt(const char *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_c{__str, __len}; }
::mfw::stl::lit_fmt_w operator""_fmt(const wchar_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_w{__str, __len}; }
#ifdef MFW_CPP_CHAR8_SUPPORTED
::mfw::stl::lit_fmt_u8 operator""_fmt(const char8_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_u8{__str, __len}; }
#endif
::mfw::stl::lit_fmt_u16 operator""_fmt(const char16_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_u16{__str, __len}; }
::mfw::stl::lit_fmt_u32 operator""_fmt(const char32_t *__str, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::lit_fmt_u32{__str, __len}; }

MFW_VISIBILITY_LOCAL_POP()