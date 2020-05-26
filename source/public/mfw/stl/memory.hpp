#ifndef __MFW_PUBLIC_STL_MEMORY_H
#define __MFW_PUBLIC_STL_MEMORY_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <memory>
	#pragma pop_macro("new")
#else
	#error
#endif

#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/shared/allocation_defines.hpp>
#include <public/mfw/stl/detail/memory_funcs.hpp>
#include <public/mfw/stl/detail/allocator.hpp>
#include <public/mfw/stl/detail/unique_ptr.hpp>
#include <public/mfw/stl/detail/use_allocator.hpp>

#define MFW_CREATE(type, ...) ::mfw::stl::__create<type>(__FILE__, __LINE__, ##__VA_ARGS__)
#define MFW_DESTROY(ptr) ::mfw::stl::__destroy(ptr, __FILE__, __LINE__)

#endif