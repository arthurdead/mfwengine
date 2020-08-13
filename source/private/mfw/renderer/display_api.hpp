#ifndef MFW_PRIVATE_RENDERER_DISPLAY_API_HPP
#define MFW_PRIVATE_RENDERER_DISPLAY_API_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class display_api : stl::uchar_t
	{
		unknown,
		xlib,
		xcb,
	#if MFW_OS_IS(LINUX)
		wayland,
	#elif MFW_OS_IS(WINDOWS)
		winapi,
	#elif MFW_OS_IS(MACOS)
		quartz,
	#endif
	};

	display_api get_display_api();
	bool detect_display_api();
}

#endif