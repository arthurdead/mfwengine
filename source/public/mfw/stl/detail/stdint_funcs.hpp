#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	void to_string(bool src, ucstring &dst);
	void to_string(bool src, uwstring &dst);

	MFW_STL_API void MFW_STL_CALL to_string(int8_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(int8_t src, uwstring &dst, int32_t base = 10);

	MFW_STL_API void MFW_STL_CALL to_string(uint8_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(uint8_t src, uwstring &dst, int32_t base = 10);
	
	MFW_STL_API void MFW_STL_CALL to_string(int16_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(int16_t src, uwstring &dst, int32_t base = 10);

	MFW_STL_API void MFW_STL_CALL to_string(uint16_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(uint16_t src, uwstring &dst, int32_t base = 10);

	MFW_STL_API void MFW_STL_CALL to_string(int32_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(int32_t src, uwstring &dst, int32_t base = 10);

	MFW_STL_API void MFW_STL_CALL to_string(uint32_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(uint32_t src, uwstring &dst, int32_t base = 10);

	MFW_STL_API void MFW_STL_CALL to_string(int64_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(int64_t src, uwstring &dst, int32_t base = 10);

	MFW_STL_API void MFW_STL_CALL to_string(uint64_t src, ucstring &dst, int32_t base = 10);
	MFW_STL_API void MFW_STL_CALL to_string(uint64_t src, uwstring &dst, int32_t base = 10);
#else
	#error
#endif
}

#include <public/mfw/stl/impl/stdint_funcs.ipp>