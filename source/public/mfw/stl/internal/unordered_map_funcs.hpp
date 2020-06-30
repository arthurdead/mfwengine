namespace mfw::stl
{
	template <typename _Kp, typename _Tp, typename _Sp>
	void to_string(const unordered_map<_Kp, _Tp> &__src, _Sp &__dst) noexcept;
}

#include <public/mfw/stl/impl/unordered_map_funcs.ipp>