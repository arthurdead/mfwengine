#ifndef MFW_PRIVATE_RENDERER_OPENGL_EGL_SHARED_HPP
#define MFW_PRIVATE_RENDERER_OPENGL_EGL_SHARED_HPP

#pragma once

#include <private/mfw/renderer/opengl/egl/egl.hpp>

namespace mfw::renderer
{
	class window;

	namespace egl
	{
		struct input_get_display_t
		{
			window *win{nullptr};
		};

		struct output_get_display_t
		{
			EGLenum platform{0};
			void *display{0};
			size_t *refs{nullptr};
		};

		struct input_get_window_t
		{
			window *win{nullptr};
		};

		struct output_get_window_t
		{
			EGLNativeWindowType *win{nullptr};
		};
	}
}

#endif