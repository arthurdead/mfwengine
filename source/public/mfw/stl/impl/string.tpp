inline MFW_VISIBILITY_LOCAL ::MFW_STD_NAMESPACE::size_t ::MFW_STD_NAMESPACE::hash<::mfw::stl::pstring>::operator()(const ::mfw::stl::pstring &__str) const noexcept
{
#if MFW_STDCPP_IS(DEFAULT)
	size_t length{__str.native().length() * sizeof(::mfw::stl::pchar_t)};
	#if MFW_LIBCPP_IS(LLVM)
	return ::MFW_STD_NAMESPACE::__do_string_hash(__str.c_str(), __str.c_str() + length);
	#elif MFW_LIBCPP_IS(GNU)
	return ::MFW_STD_NAMESPACE::_Hash_impl::hash(__str.c_str(), length);
	#elif MFW_LIBCPP_IS(MS)
	return ::MFW_STD_NAMESPACE::_Hash_array_representation(__str.c_str(), length);
	#else
		#error
	#endif
#elif MFW_STDCPP_IS(EA)
	return ::MFW_STD_NAMESPACE::hash<const ::mfw::stl::pchar_t *>{}(__str.c_str());
#else
	#error
#endif
}