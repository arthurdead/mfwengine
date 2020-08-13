#ifndef MFW_PRIVATE_RENDERER_VULKAN_SPIRV_SHADER_H
#define MFW_PRIVATE_RENDERER_VULKAN_SPIRV_SHADER_H

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::renderer
{
	enum /*class*/ spirv_shader_type : uchar_t
	{
		vertex,
		fragment,
		count,
		unknown = count,
	};

	spirv_shader_type get_spirv_type(ucstring_view ext);
	spirv_shader_type get_spirv_type(const pstring &ext);

	enum class spirv_shader_target : uchar_t
	{
		vulkan,
		opengl,
	};

	enum class spirv_shader_source : uchar_t
	{
		glsl,
		hlsl,
	};

	void compile_spirv_shaders(spirv_shader_target target, spirv_shader_source source);
}

#endif