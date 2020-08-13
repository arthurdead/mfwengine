#ifndef MFW_PUBLIC_PCH_HPP
#define MFW_PUBLIC_PCH_HPP

#pragma once

//#define _MFW_ENABLE_CUSTOM_ALLOCATORS

#include <public/mfw/stl/version.hpp>

#ifdef MFW_CPP
	#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
		#include <public/mfw/stl/new.hpp>
	#endif

	#ifdef MFW_DEPENDS_ON_STL
		#include <public/mfw/stl/pch.hpp>
	#endif

	#ifdef MFW_DEPENDS_ON_CORE
		#include <public/mfw/core/pch.hpp>
	#endif
#endif

#include <public/mfw/pch_warnings.hpp>

#endif
