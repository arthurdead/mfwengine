#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	MFW_STL_API void MFW_STL_CALL to_string(float32_t src, ucstring &dst);
	MFW_STL_API void MFW_STL_CALL to_string(float32_t src, uwstring &dst);

	MFW_STL_API void MFW_STL_CALL to_string(float64_t src, ucstring &dst);
	MFW_STL_API void MFW_STL_CALL to_string(float64_t src, uwstring &dst);

	MFW_STL_API void MFW_STL_CALL to_string(float80_t src, ucstring &dst);
	MFW_STL_API void MFW_STL_CALL to_string(float80_t src, uwstring &dst);
#else
	#error
#endif
}