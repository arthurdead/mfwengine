#include <public/mfw/stl/detail/default_delete.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename D = default_delete<T>>
	using unique_ptr = ::MFW_STD_NAMESPACE::unique_ptr<T, D>;
#else
	#error
#endif
}

#include <public/mfw/stl/detail/unique_ptr_funcs.hpp>