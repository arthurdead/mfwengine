#ifndef __MFW_PUBLIC_PCH_H
#define __MFW_PUBLIC_PCH_H

#pragma once

//#define __MFW_ENABLE_CUSTOM_ALLOCATORS

#include <public/mfw/stl/version.hpp>

#ifdef MFW_CPP
	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
		#include <public/mfw/stl/new.hpp>
	#endif

	#ifdef MFW_DEPENDS_ON_STL
		#include <public/mfw/stl/pch.hpp>
	#endif

	#ifdef MFW_DEPENDS_ON_CORE
		//#include <public/mfw/core/pch.hpp>
	#endif
#endif

#include <public/mfw/pch_warnings.hpp>

#endif
