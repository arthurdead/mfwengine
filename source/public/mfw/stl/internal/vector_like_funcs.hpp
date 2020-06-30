namespace mfw::stl
{
	template <typename _Tp, typename _Sp>
	void to_string(const _MFW_VECTOR_LIKE_CONTAINER<_Tp> &__src, _Sp &__dst) noexcept;

	template <typename _Tp, typename _Vp>
	bool contains(const _MFW_VECTOR_LIKE_CONTAINER<_Tp> &__vec, const _Vp &__value) noexcept;
}

#include <public/mfw/stl/impl/vector_like_funcs.ipp>