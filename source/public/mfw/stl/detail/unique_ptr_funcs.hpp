#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T>
	void to_string(const unique_ptr<T> &src, ucstring &dst);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/unique_ptr_funcs.ipp>