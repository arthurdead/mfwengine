namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace __stdint_funcs_internal
	{
		template <typename S, typename C>
		void to_string(bool src, S &dst, C strtrue[4], C strfalse[5])
		{
			if(src) {
				dst.insert(0, strtrue, 4);
			} else {
				dst.insert(0, strfalse, 5);
			}
		}
	}

	inline void to_string(bool src, ucstring &dst)
	{
		__stdint_funcs_internal::to_string(src, dst, STR_C("true"), STR_C("false"));
	}

	inline void to_string(bool src, uwstring &dst)
	{
		__stdint_funcs_internal::to_string(src, dst, STR_W("true"), STR_W("false"));
	}
#else
	#error
#endif
}