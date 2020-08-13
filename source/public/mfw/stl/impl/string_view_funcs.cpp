#include <public/mfw/stl/string_view.hpp>

namespace mfw::stl
{
	namespace __public_impl_string_view_funcs_cpp_internal
	{
		template <typename _Sp>
		static MFW_VISIBILITY_LOCAL bool _to_bool_impl(_Sp __src, size_t __len, bool &__dst) noexcept
		{
			using __C = typename _Sp::value_type;

			if(__len == 0) {
				__dst = false;
				return false;
			} else if(__len == 4) {
				if(__src[0] == static_cast<__C>('t') &&
					__src[1] == static_cast<__C>('r') &&
					__src[2] == static_cast<__C>('u') &&
					__src[3] == static_cast<__C>('e')) {
					__dst = true;
					return true;
				}
			} else if(__len == 5) {
				if(__src[0] == static_cast<__C>('f') &&
					__src[1] == static_cast<__C>('a') &&
					__src[2] == static_cast<__C>('l') &&
					__src[3] == static_cast<__C>('s') &&
					__src[4] == static_cast<__C>('e')) {
					__dst = false;
					return true;
				}
			}

			uint8_t __i{0};
			if(!to_int(__src, __i)) {
				__dst = false;
				return false;
			}
			__dst = (__i > 0);
			return true;
		}
	}

	MFW_VISIBILITY_LOCAL_PUSH()

	bool to_bool(string_view __src, bool &__dst) noexcept
	{ return __public_impl_string_view_funcs_ipp_internal::_to_bool_impl(__src, __src.length(), __dst); }
	bool to_bool(wstring_view __src, bool &__dst) noexcept
	{ return  __public_impl_string_view_funcs_ipp_internal::_to_bool_impl(__src, __src.length(), __dst); }
	bool to_bool(u8string_view __src, bool &__dst) noexcept
	{ return  __public_impl_string_view_funcs_ipp_internal::_to_bool_impl(__src, __src.length(), __dst); }
	/*
	bool to_bool(u16string_view __src, bool &__dst) noexcept
	{ return  __public_impl_string_view_funcs_ipp_internal::_to_bool_impl(__src, __src.length(), __dst); }
	bool to_bool(u32string_view __src, bool &__dst) noexcept
	{ return  __public_impl_string_view_funcs_ipp_internal::_to_bool_impl(__src, __src.length(), __dst); }
	*/

	MFW_VISIBILITY_LOCAL_POP()
}

MFW_VISIBILITY_LOCAL_PUSH()

constexpr ::mfw::stl::string_view operator""_sv(const char *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::string_view{__ptr, __len}; }
constexpr ::mfw::stl::wstring_view operator""_sv(const wchar_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::wstring_view{__ptr, __len}; }
#ifdef MFW_CPP_CHAR8_SUPPORTED
constexpr ::mfw::stl::u8string_view operator""_sv(const char8_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::u8string_view{__ptr, __len}; }
#endif
constexpr ::mfw::stl::u16string_view operator""_sv(const char16_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::u16string_view{__ptr, __len}; }
constexpr ::mfw::stl::u32string_view operator""_sv(const char32_t *__ptr, ::mfw::stl::size_t __len) noexcept
{ return ::mfw::stl::u32string_view{__ptr, __len}; }

MFW_VISIBILITY_LOCAL_POP()