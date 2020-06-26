#ifndef __MFW_PRIVATE_RENDERER_OPENGL_CONTEXT_API_H
#define __MFW_PRIVATE_RENDERER_OPENGL_CONTEXT_API_H

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class context_api
	{
		unknown,
		egl,
	#if MFW_OS_IS(WINDOWS)
		wgl,
	#elif MFW_OS_IS(LINUX)
		glx,
		xcb_glx,
	#endif
	};

	context_api get_context_api();
	bool detect_context_api();
}

#endif