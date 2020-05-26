namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	inline void to_string(const ucstring &src, core::univalue &dst)
	{
		dst.set(src);
	}

	inline void to_string(const pstring &src, core::univalue &dst)
	{
		MFW_MESSAGE("fix this")
		ucstring str{src.u8string()};
		dst.set_string(str);
	}
#else
	#error
#endif
}