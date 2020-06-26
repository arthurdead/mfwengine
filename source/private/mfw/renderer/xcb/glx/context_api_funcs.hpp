#ifndef __MFW_PRIVATE_RENDERER_XCB_GLX_CONTEXT_API_FUNCS_H
#define __MFW_PRIVATE_RENDERER_XCB_GLX_CONTEXT_API_FUNCS_H

#pragma once

#include <private/mfw/renderer/opengl/context_api_funcs.hpp>
#include <private/mfw/renderer/xcb/glx/glx.hpp>
#include <private/mfw/renderer/xcb/glx/shared.hpp>
#include <private/mfw/renderer/opengl/render_api_funcs.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>
#include <private/mfw/renderer/xcb/display_api_funcs.hpp>

namespace mfw::renderer
{
	class context_api_funcs_xcb_glx : public interfaces::context_api_funcs
	{
	public:
		static context_api_funcs_xcb_glx &instance();

		bool initialize() override { return true; }
		void create_window(window &win, const graphics_card &gpu, size_t w, size_t h) override;
		void destroy_window(window &win) override {}
		void make_current(window &win) override;
		void swap_buffers(window &win) override;

		xcb_visualid_t get_visual(window &win);

		using window_display_data_t = display_api_funcs_xcb::window_display_data_t;
		using display_info_t = display_api_funcs_xcb::display_info_t;
		using screen_info_t = display_api_funcs_xcb::screen_info_t;
		using monitor_display_data_t = display_api_funcs_xcb::monitor_display_data_t;
		using window_render_data_t = render_api_funcs_opengl::window_render_data_t;

		struct prop_list_t
		{
			uint32_t name{0};
			uint32_t value{0};
			static uint32_t *get_prop_value(prop_list_t *props, uint32_t num, uint32_t name) {
				for(uint32_t j{0}; j < num; j++) {
					if(props[j].name == name) {
						return &props[j].value;
					}
				}
				return nullptr;
			}
		};

		struct config_t
		{
			xcb_glx_fbconfig_t config{0};
			prop_list_t *props{nullptr};
			size_t num_props{0};
			int32_t idx{0};
			uint32_t *get_value(uint32_t name) {
				return prop_list_t::get_prop_value(props, num_props, name);
			}
		};

		static void choose_config(xcb_connection_t *xcb_disp, int32_t scrn, const prop_list_t *attribs, size_t len, config_t &config);

		struct context_data_t
		{
			~context_data_t() { close(); }

			xcb_glx_context_t context{0};
			config_t config{};
			xcb_visualid_t visual{0};
			xcb_glx_window_t window{0};
			xcb_glx_context_tag_t tag{0};

			void make_current(xcb_connection_t *xcb_disp);
			void swap_buffers(xcb_connection_t *xcb_disp);

			void setup_config(xcb_connection_t *xcb_disp, size_t scrn);

			void close();
		};
	};
}

#endif