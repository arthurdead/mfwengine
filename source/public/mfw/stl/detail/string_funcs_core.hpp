#include <public/mfw/core/univalue.hpp>

namespace mfw
{
	namespace core
	{
		class univalue;
	}
	
	namespace stl
	{
	#if MFW_STD_FLAGGED(API_CONFORMING)
		void to_string(const ucstring &src, core::univalue &dst);
		void to_string(const pstring &src, core::univalue &dst);
	#else
		#error
	#endif
	}
}

//#include <public/mfw/stl/impl/string_funcs_core.ipp>