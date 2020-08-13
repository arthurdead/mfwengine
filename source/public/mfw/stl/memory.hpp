#ifndef MFW_PUBLIC_STL_MEMORY_HPP
#define MFW_PUBLIC_STL_MEMORY_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <memory>
#elif MFW_STDCPP_IS(EASTL)
	#include <EASTL/memory.h>
#else
	#error
#endif
#pragma pop_macro("new")

#if defined __cpp_impl_destroying_delete || \
	defined __cpp_lib_destroying_delete
	#define MFW_CPP_DESTROYING_DELETE_SUPPORTED 1
#endif

#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/shared/allocation_defines.hpp>
#include <public/mfw/stl/internal/memory_funcs.hpp>
#include <public/mfw/stl/internal/allocator.hpp>
#include <public/mfw/stl/internal/unique_ptr.hpp>
#include <public/mfw/stl/internal/use_allocator.hpp>

#define MFW_CREATE(type, ...) ::mfw::stl::__create<type>(__FILE__, __LINE__, ##__VA_ARGS__)
#define MFW_DESTROY(ptr) ::mfw::stl::__destroy(ptr, __FILE__, __LINE__)

#endif