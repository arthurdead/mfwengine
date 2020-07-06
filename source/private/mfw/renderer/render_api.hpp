#ifndef __MFW_PRIVATE_RENDERER_RENDER_API_HPP
#define __MFW_PRIVATE_RENDERER_RENDER_API_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class render_api
	{
		unknown,
	#if MFW_OS_IS(WINDOWS)
		directx,
	#endif
		opengl,
		vulkan,
	};

	render_api get_render_api();
	bool detect_render_api();
}

#endif