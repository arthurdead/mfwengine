#ifndef __MFW_PRIVATE_RENDERER_XCB_DISPLAY_API_FUNCS_H
#define __MFW_PRIVATE_RENDERER_XCB_DISPLAY_API_FUNCS_H

#pragma once

#include <private/mfw/renderer/display_api_funcs.hpp>
#include <xcb/xcb.h>

namespace mfw::renderer
{
	class display_api_funcs_xcb : public interfaces::display_api_funcs
	{
	public:
		bool collect_gpus(ptr_vector<graphics_card> &gpus) override;
		bool collect_monitors(ptr_vector<monitor> &monitors) override;
		bool init_windows() override;
		void create_window(window &win, const monitor &monitor, size_t w, size_t h, ssize_t x, ssize_t y) override;
		void show_window(window &win, bool show) override;
		void flush(monitor &mon) override;
		void destroy_window(window &win) override;

		struct gpu_display_data_t
		{

		};

		struct screen_info_t
		{
			xcb_connection_t *xdisp{nullptr};
			xcb_window_t root{0};
			xcb_visualid_t visual{0};
			uint32_t white_pixel{0};
		};

		struct monitor_display_data_t
		{
			screen_info_t *scrn_info{nullptr};
		};

		struct window_display_data_t
		{
			xcb_window_t id{0};
		};
	};
}

#endif