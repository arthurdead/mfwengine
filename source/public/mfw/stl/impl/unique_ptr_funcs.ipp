namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	template <typename T>
	inline void to_string(const unique_ptr<T> &src, ucstring &dst)
	{
		if(src.empty()) {
			dst.insert(0, u8"(nullptr)", 9);
		} else {
			dst += as_string<ucstring>(*src.get());
		}
	}
#else
	#error
#endif
}