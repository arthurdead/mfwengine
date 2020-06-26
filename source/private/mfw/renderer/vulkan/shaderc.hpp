#ifndef __MFW_PRIVATE_RENDERER_VULKAN_SHADERC_H
#define __MFW_PRIVATE_RENDERER_VULKAN_SHADERC_H

#pragma once

#ifdef SHADERC_SHADERC_HPP_
	#error
#endif

#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>

namespace __shaderc_std_namespace
{
	using namespace ::mfw::stl;
}
#define std __shaderc_std_namespace
#include <shaderc/shaderc.hpp>
#undef std

#endif