namespace mfw::stl
{
	template <typename _Tp, typename _Sp>
	MFW_VISIBILITY_LOCAL void to_string(const unique_ptr<_Tp> &src, _Sp &dst) noexcept;
}

#include <public/mfw/stl/impl/unique_ptr_funcs.tpp>