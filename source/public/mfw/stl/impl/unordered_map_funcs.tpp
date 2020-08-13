namespace mfw::stl
{
	template <typename _Kp, typename _Tp, typename _Sp>
	MFW_VISIBILITY_LOCAL void to_string(const unordered_map<_Kp, _Tp> &__src, _Sp &__dst) noexcept;
	/*{
		if(__src.empty()) {
			return;
		}

		using __C = typename _Sp::value_type;

		for(const pair<_Kp, _Tp> &__it : __src) {
			_MFW_TO_STRING_HELPER(__it.first, _Kp, __dst, _Sp)
			__dst.append(1, static_cast<__C>('='));
			_MFW_TO_STRING_HELPER(__it.second, _Tp, __dst, _Sp)
			__dst.append(1, static_cast<__C>(','));
			__dst.append(1, static_cast<__C>(' '));
		}
		__dst.erase(__dst.end()-2, __dst.end());
	}*/
}
