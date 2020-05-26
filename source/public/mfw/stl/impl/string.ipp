#if MFW_STD_FLAGGED(API_CONFORMING)
namespace MFW_STD_NAMESPACE
{
	/*template <>
	::MFW_STD_NAMESPACE::size_t hash<::mfw::stl::pstring>::operator()(const ::mfw::stl::pstring &str) const
	{
	#if MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
		return ::MFW_STD_NAMESPACE::_Hash_impl::hash(str.c_str(), str.native().length() * sizeof(::mfw::stl::upchar_t));
	#else
		#error
	#endif
	}*/
	
	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	/*template <>
	::MFW_STD_NAMESPACE::size_t hash<::mfw::stl::ucstring>::operator()(const ::mfw::stl::ucstring &str) const
	{
	#if MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
		return std::_Hash_impl::hash(str.c_str(), str.length() * sizeof(ucchar_t));
	#else
		#error
	#endif
	}

	template <>
	::MFW_STD_NAMESPACE::size_t hash<::mfw::stl::uwstring>::operator()(const ::mfw::stl::uwstring &str) const
	{
	#if MFW_COMPILER & MFW_COMPILER_UNIX_FLAG
		return std::_Hash_impl::hash(str.c_str(), str.length() * sizeof(uwchar_t));
	#else
		#error
	#endif
	}*/
	#endif
}
#else
	#error
#endif