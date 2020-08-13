#ifndef MFW_PRIVATE_RENDERER_RENDER_API_HPP
#define MFW_PRIVATE_RENDERER_RENDER_API_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class render_api : stl::uchar_t
	{
		unknown,
	#if MFW_OS_IS(WINDOWS)
		directx,
	#endif
		vulkan,
	#if MFW_OS_IS(MACOS)
		metal,
	#else
		opengl,
	#endif
	};

	render_api get_render_api();
	bool detect_render_api();
}

#endif