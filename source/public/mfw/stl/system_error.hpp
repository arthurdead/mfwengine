#ifndef MFW_PUBLIC_STL_SYSTEM_ERROR_HPP
#define MFW_PUBLIC_STL_SYSTEM_ERROR_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#include <system_error>

#if MFW_HAS_FEATURE(cxx_exceptions) || \
	defined __cpp_exceptions || \
	defined __EXCEPTIONS || \
	defined _HAS_EXCEPTIONS
	#define MFW_CPP_EXCEPTIONS_SUPPORTED 1
#endif

namespace mfw::stl
{
	using ::std::error_code;
	using ::std::errc;
	using ::std::is_error_code_enum;
	using ::std::error_category;
	using ::std::logic_error;
	using ::std::system_error;
	using ::std::error_condition;
	using ::std::runtime_error;
}

#endif