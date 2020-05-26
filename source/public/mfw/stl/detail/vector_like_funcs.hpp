#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T>
	void to_string(const __MFW_VECTOR_LIKE_CONTAINER<T> &src, ucstring &dst);

	template <typename T>
	void to_string(const __MFW_VECTOR_LIKE_CONTAINER<T> &src, uwstring &dst);

	template <typename T, typename V>
	bool contains(const __MFW_VECTOR_LIKE_CONTAINER<T> &vec, const V &value);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/vector_like_funcs.ipp>
