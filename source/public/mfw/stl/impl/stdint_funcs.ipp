namespace mfw::stl
{
	namespace __public_impl_stdint_funcs_ipp_internal MFW_VISIBILITY_LOCAL
	{
		template <typename _Sp>
		static void _to_string_impl(bool __src, _Sp &__dst) noexcept
		{
			using __C = typename _Sp::value_type;

			if(__src) {
				__dst.append(1, static_cast<__C>('t'));
				__dst.append(1, static_cast<__C>('r'));
				__dst.append(1, static_cast<__C>('u'));
				__dst.append(1, static_cast<__C>('e'));
			} else {
				__dst.append(1, static_cast<__C>('f'));
				__dst.append(1, static_cast<__C>('a'));
				__dst.append(1, static_cast<__C>('l'));
				__dst.append(1, static_cast<__C>('s'));
				__dst.append(1, static_cast<__C>('e'));
			}
		}
	}

	inline void to_string(bool __src, string &__dst) noexcept
	{ __public_impl_stdint_funcs_ipp_internal::_to_string_impl(__src, __dst); }
	inline void to_string(bool __src, wstring &__dst) noexcept
	{ __public_impl_stdint_funcs_ipp_internal::_to_string_impl(__src, __dst); }
	inline void to_string(bool __src, u8string &__dst) noexcept
	{ __public_impl_stdint_funcs_ipp_internal::_to_string_impl(__src, __dst); }
	inline void to_string(bool __src, u16string &__dst) noexcept
	{ __public_impl_stdint_funcs_ipp_internal::_to_string_impl(__src, __dst); }
	inline void to_string(bool __src, u32string &__dst) noexcept
	{ __public_impl_stdint_funcs_ipp_internal::_to_string_impl(__src, __dst); }
}