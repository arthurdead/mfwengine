#ifndef __MFW_PRIVATE_RENDERER_OPENGL_OPENGL_RENDER_API_FUNCS_HPP
#define __MFW_PRIVATE_RENDERER_OPENGL_OPENGL_RENDER_API_FUNCS_HPP

#pragma once

#include <private/mfw/renderer/render_api_funcs.hpp>
#include <private/mfw/renderer/opengl/opengl.hpp>
#include <private/mfw/renderer/opengl/shader.hpp>

namespace mfw::renderer
{
	class render_api_funcs_opengl : public interfaces::render_api_funcs
	{
	public:
		~render_api_funcs_opengl();

		static render_api_funcs_opengl &instance();

		bool pre_initialize() override;
		bool initialize() override;
		void create_window(window &win, const graphics_card &gpu, size_t w, size_t h) override;
		void destroy_window(window &win) override;
		void render_window(window &win) override;

		uint32_t VBO{0};
		uint32_t VAO{0};
		shader_opengl shader{u8"gl_test"_sv};

		bool use_spirv{false};

		void on_context_loaded(const graphics_card &gpu, GLADloadfunc func);

		struct window_render_data_t
		{
			__MFW_RENDERER_OPAQUE_DATA(context)
		};
	};
}

#endif