#ifndef MFW_PRIVATE_RENDERER_OPENGL_CONTEXT_API_HPP
#define MFW_PRIVATE_RENDERER_OPENGL_CONTEXT_API_HPP

#pragma once

#include <public/mfw/stl/version.hpp>

namespace mfw::renderer
{
	enum class context_api : stl::uchar_t
	{
		unknown,
		egl,
	#if MFW_OS_IS(WINDOWS)
		wgl,
	#elif MFW_OS_IS(LINUX)
		glx,
		xcb_glx,
	#elif MFW_OS_IS(MACOS)
		cgl,
	#endif
	};

	context_api get_context_api();
	bool detect_context_api();
}

#endif