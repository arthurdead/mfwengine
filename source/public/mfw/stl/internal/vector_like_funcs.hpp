namespace mfw::stl
{
	MFW_VISIBILITY_LOCAL_PUSH()

	template <typename _Tp, typename _Sp>
	void to_string(const _MFW_VECTOR_LIKE_CONTAINER<_Tp> &__src, _Sp &__dst) noexcept;

	template <typename _Tp, typename _Vp>
	bool contains(const _MFW_VECTOR_LIKE_CONTAINER<_Tp> &__vec, const _Vp &__value) noexcept;

	MFW_VISIBILITY_LOCAL_POP()
}

#include <public/mfw/stl/impl/vector_like_funcs.tpp>