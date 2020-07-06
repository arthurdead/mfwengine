#ifndef __MFW_PRIVATE_RENDERER_OPENGL_EGL_CONTEXT_API_FUNCS_HPP
#define __MFW_PRIVATE_RENDERER_OPENGL_EGL_CONTEXT_API_FUNCS_HPP

#pragma once

#include <private/mfw/renderer/opengl/context_api_funcs.hpp>
#include <private/mfw/renderer/opengl/egl/egl.hpp>
#include <private/mfw/renderer/opengl/egl/shared.hpp>
#include <private/mfw/renderer/opengl/render_api_funcs.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>

namespace mfw::renderer
{
	class context_api_funcs_egl : public interfaces::context_api_funcs
	{
	public:
		~context_api_funcs_egl() override;

		bool initialize() override;
		void create_window(window &win, const graphics_card &gpu, size_t w, size_t h) override;
		void destroy_window(window &win) override;
		void make_current(window &win) override;
		void swap_buffers(window &win) override;

		void on_egl_loaded(const graphics_card &gpu);

		using window_render_data_t = render_api_funcs_opengl::window_render_data_t;

		struct context_data_t
		{
			~context_data_t() { close(); }

			EGLDisplay display{nullptr};
			size_t *refs{nullptr};

			EGLConfig config{nullptr};
			EGLint num_config{0};

			EGLSurface surface{nullptr};

			EGLContext context{nullptr};

			void make_current();
			void swap_buffers();

			void close();
		};
	};
}

#endif