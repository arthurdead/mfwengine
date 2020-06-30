#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
	namespace __private_string_funcs_cpp MFW_VISIBILITY_LOCAL
	{
		template <typename _Sp, typename _Cp>
		static size_t _replace_all_impl_c_c(_Sp &__str, _Cp __from, _Cp __to) noexcept
		{
			size_t __count{0};

			replace_if(__str.begin(), __str.end(), [__from, &__count](_Cp __old) -> bool {
				if(__old == __from) {
					__count++;
					return true;
				}
				return false;
			}, __to);

			return __count;
		}

		template <typename _Sp, typename _Vp>
		static size_t _replace_all_impl_v_v(_Sp &__str, _Vp __from, _Vp __to) noexcept
		{
			size_t __count{0};

			size_t __pos{0};
			while(true) {
				__pos = __str.find(__from, __pos);
				if(__pos == _Sp::npos) {
					break;
				}

				__str.replace(__pos, __from.length(), __to);
				__count++;

				__pos += __to.length();
			}

			return __count;
		}
	}

	MFW_STL_API size_t MFW_STL_CALL replace_all(string &__str, char __from, char __to) noexcept
	{ return __private_string_funcs_cpp::_replace_all_impl_c_c(__str, __from, __to); }
	MFW_STL_API size_t MFW_STL_CALL replace_all(string &__str, string_view __from, string_view __to) noexcept
	{ return __private_string_funcs_cpp::_replace_all_impl_v_v(__str, __from, __to); }

	MFW_STL_API size_t MFW_STL_CALL replace_all(wstring &__str, wchar_t __from, wchar_t __to) noexcept
	{ return __private_string_funcs_cpp::_replace_all_impl_c_c(__str, __from, __to); }
	MFW_STL_API size_t MFW_STL_CALL replace_all(wstring &__str, wstring_view __from, wstring_view __to) noexcept
	{ return __private_string_funcs_cpp::_replace_all_impl_v_v(__str, __from, __to); }
}