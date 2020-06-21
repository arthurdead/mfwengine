#ifndef __MFW_PRIVATE_RENDERER_DISPLAY_API_H
#define __MFW_PRIVATE_RENDERER_DISPLAY_API_H

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class display_api
	{
		unknown,
		xlib,
		xcb,
	#if MFW_OS_IS(LINUX)
		wayland,
	#elif MFW_OS_IS(WINDOWS)
		windows,
	#endif
	};

	display_api get_display_api();
	bool detect_display_api();
}

#endif