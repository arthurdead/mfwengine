namespace mfw::stl
{
	template <typename _Tp, typename _Sp>
	MFW_VISIBILITY_LOCAL void to_string(const unique_ptr<_Tp> &__src, _Sp &__dst) noexcept;
	/*{
		using __C = typename _Sp::value_type;

		if(__src.empty()) {
			__dst.append(1, static_cast<__C>('('));
			__dst.append(1, static_cast<__C>('n'));
			__dst.append(1, static_cast<__C>('u'));
			__dst.append(1, static_cast<__C>('l'));
			__dst.append(1, static_cast<__C>('l'));
			__dst.append(1, static_cast<__C>('p'));
			__dst.append(1, static_cast<__C>('t'));
			__dst.append(1, static_cast<__C>('r'));
			__dst.append(1, static_cast<__C>(')'));
		} else {
			const _Tp &__tmp{*__src.get()};
			_MFW_TO_STRING_HELPER(__tmp, _Tp, __dst, _Sp)
		}
	}*/
}