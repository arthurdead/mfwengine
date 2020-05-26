#ifndef __MFW_PUBLIC_STL_FILESYSTEM_H
#define __MFW_PUBLIC_STL_FILESYSTEM_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <filesystem>
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	namespace filesystem = ::MFW_STD_NAMESPACE::filesystem;
#else
	#error
#endif
}

#endif