#ifndef _MFW_PUBLIC_STL_HPP
#define _MFW_PUBLIC_STL_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STL_BUILD & MFW_BUILD_SHARED_FLAG
	#ifdef MFW_BUILDING_STL
		#define MFW_STL_API MFW_SHARED_EXPORT
	#else
		#define MFW_STL_API MFW_SHARED_IMPORT
	#endif
	#define MFW_STL_CALL MFW_CALL_SHARED
#else
	#define MFW_STL_API 
	#define MFW_STL_CALL 
#endif

#endif