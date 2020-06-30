#include <public/mfw/stl/internal/default_delete.hpp>

namespace mfw::stl
{
	template <typename _Tp, typename _Dp = default_delete<_Tp>>
	using unique_ptr = ::MFW_STD_NAMESPACE::unique_ptr<_Tp, _Dp>;
}

#include <public/mfw/stl/internal/unique_ptr_funcs.hpp>