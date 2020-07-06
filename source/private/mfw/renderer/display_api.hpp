#ifndef __MFW_PRIVATE_RENDERER_DISPLAY_API_HPP
#define __MFW_PRIVATE_RENDERER_DISPLAY_API_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class display_api
	{
		unknown,
	#if MFW_OS_IS(LINUX)
		xlib,
		xcb,
		wayland,
	#elif MFW_OS_IS(WINDOWS)
		winapi,
	#endif
	};

	display_api get_display_api();
	bool detect_display_api();
}

#endif