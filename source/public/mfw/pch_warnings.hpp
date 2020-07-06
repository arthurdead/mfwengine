#ifndef __MFW_PUBLIC_PCH_WARNINGS_HPP
#define __MFW_PUBLIC_PCH_WARNINGS_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>

#if MFW_COMPILER_FLAGGED(MSVC)
	MFW_WARNING_DISABLE(4275) //non - DLL-interface class 'class_1' used as base for DLL-interface class 'class_2'
	MFW_WARNING_DISABLE(4625) //'derived class' : copy constructor was implicitly defined as deleted because a base class copy constructor is inaccessible or deleted
	MFW_WARNING_DISABLE(4626) //'derived class' : assignment operator was implicitly defined as deleted because a base class assignment operator is inaccessible or deleted
	MFW_WARNING_DISABLE(4251) //'identifier' : class 'type' needs to have dll-interface to be used by clients of class 'type2'
	MFW_WARNING_DISABLE(4820) //'bytes' bytes padding added after construct 'member_name'
	MFW_WARNING_DISABLE(5026) //'type': move constructor was implicitly defined as deleted
	MFW_WARNING_DISABLE(5027) //'type': move assignment operator was implicitly defined as deleted
	MFW_WARNING_DISABLE(5045) //Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
	MFW_WARNING_DISABLE(4121) //'symbol' : alignment of a member was sensitive to packing
	MFW_WARNING_DISABLE(4315) //'classname': 'this' pointer for member 'member' may not be aligned 'alignment' as expected by the constructor
	MFW_WARNING_DISABLE(5050) //Possible incompatible environment while importing module 'module_name': issue
	MFW_WARNING_DISABLE(4623) //'derived class': default constructor was implicitly defined as deleted because a base class default constructor is inaccessible or deleted
	MFW_WARNING_DISABLE(5052) //Keyword 'char8_t' was introduced in C++20 and requires use of the '/std:c++latest' command-line option
	#if MFW_CONFIGURATION_IS(DEBUG)
		MFW_WARNING_DISABLE(4189) //'identifier' : local variable is initialized but not referenced
		MFW_WARNING_DISABLE(4100) //'identifier' : unreferenced formal parameter
		MFW_WARNING_DISABLE(4514) //'function' : unreferenced inline function has been removed
		MFW_WARNING_DISABLE(4101) //'identifier' : unreferenced local variable
		MFW_WARNING_DISABLE(4505) //'function' : unreferenced local function has been removed
		MFW_WARNING_DISABLE(6031) //return value ignored: <function> could return unexpected value
		MFW_WARNING_DISABLE(28193) //The variable holds a value that must be examined
	#endif
#endif

#if MFW_COMPILER_FLAGGED(UNIX)
	#if MFW_COMPILER_FLAGGED(CLANG)
		MFW_WARNING_DISABLE_UNIX("-Wc++98-compat")
		MFW_WARNING_DISABLE_UNIX("-Wc++98-compat-pedantic")
		MFW_WARNING_DISABLE_UNIX("-Wnewline-eof")
		MFW_WARNING_DISABLE_UNIX("-Wreserved-id-macro")
		MFW_WARNING_DISABLE_UNIX("-Wc++98-c++11-c++14-compat")
		#ifdef __MFW_DISABLE_COMPILE_MESSAGES
			MFW_WARNING_DISABLE_UNIX("-W#pragma-messages")
		#endif
	#endif
	#if MFW_CONFIGURATION_IS(DEBUG)
		MFW_WARNING_DISABLE_UNIX("-Wunused-function")
		MFW_WARNING_DISABLE_UNIX("-Wunused-variable")
		MFW_WARNING_DISABLE_UNIX("-Wunused-parameter")
		MFW_WARNING_DISABLE_UNIX("-Wattributes")
		#if MFW_COMPILER_FLAGGED(CLANG)
			MFW_WARNING_DISABLE_UNIX("-Wunused-local-typedef")
		#endif
	#endif
#endif

#endif