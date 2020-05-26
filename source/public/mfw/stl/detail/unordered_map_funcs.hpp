namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename K, typename T>
	void to_string(const unordered_map<K, T> &src, ucstring &dst);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/unordered_map_funcs.ipp>