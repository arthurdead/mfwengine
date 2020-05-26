#include <public/mfw/stl/vector.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T, typename ...Args>
	bool to_vector(const tuple<Args...> &tup, vector<T> &vec);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/tuple_funcs.ipp>