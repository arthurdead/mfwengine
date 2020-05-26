namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T>
	void to_string(const __MFW_VECTOR_LIKE_CONTAINER<T> &src, ucstring &dst)
	{
		if(src.empty()) {
			return;
		}

		for(const T &it : src) {
			dst += as_string<ucstring>(it);
			dst += u8',';
			dst += u8' ';
		}
		dst.erase(dst.end() - 2, dst.end());
	}

	template <typename T>
	void to_string(const __MFW_VECTOR_LIKE_CONTAINER<T> &src, uwstring &dst)
	{
		if(src.empty()) {
			return;
		}

		for(const T &it : src) {
			dst += as_string<uwstring>(it);
			dst += u',';
			dst += u' ';
		}
		dst.erase(dst.end() - 2, dst.end());
	}

	template <typename T, typename V>
	bool contains(const __MFW_VECTOR_LIKE_CONTAINER<T> &vec, const V &value)
	{
		if(vec.empty()) {
			return false;
		} else if(vec.size() == 1) {
			return (*vec.cbegin() == value);
		} else {
			for(const T &it : vec) {
				if(it == value) {
					return true;
				}
			}
			return false;
		}
	}
#else
	#error
#endif
}
