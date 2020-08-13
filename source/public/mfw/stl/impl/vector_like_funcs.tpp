namespace mfw::stl
{
	MFW_VISIBILITY_LOCAL_PUSH()

	template <typename _Tp, typename _Sp>
	void to_string(const _MFW_VECTOR_LIKE_CONTAINER<_Tp> &__src, _Sp &__dst) noexcept;
	/*{
		if(__src.empty()) {
			return;
		}

		extern void to_string(const _Tp &, _Sp &);

		using __C = typename _Sp::value_type;

		for(const _Tp &__it : __src) {
			_MFW_TO_STRING_HELPER(__it, _Tp, __dst, _Sp)
			__dst.append(1, static_cast<__C>(','));
			__dst.append(1, static_cast<__C>(' '));
		}
		__dst.erase(__dst.end()-2, __dst.end());
	}*/

	template <typename _Tp, typename _Vp>
	bool contains(const _MFW_VECTOR_LIKE_CONTAINER<_Tp> &__vec, const _Vp &__value) noexcept
	{
		if(__vec.empty()) {
			return false;
		} else if(__vec.size() == 1) {
			return (*__vec.cbegin() == __value);
		} else {
			for(const _Tp &__it : __vec) {
				if(__it == __value) {
					return true;
				}
			}
			return false;
		}
	}

	MFW_VISIBILITY_LOCAL_POP()
}
