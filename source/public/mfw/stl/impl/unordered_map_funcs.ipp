namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename K, typename T>
	inline void to_string(const unordered_map<K, T> &src, ucstring &dst)
	{
		if(src.empty()) {
			return;
		}

		for(const pair<K, T> &it : src) {
			dst += as_string<ucstring>(it.first);
			dst += u8'=';
			dst += as_string<ucstring>(it.second);
			dst += u8',';
			dst += u8' ';
		}
		dst.erase(dst.end() - 2, dst.end());
	}
#else
	#error
#endif
}
