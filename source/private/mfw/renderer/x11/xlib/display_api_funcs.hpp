#ifndef __MFW_PRIVATE_RENDERER_X11_XLIB_XLIB_DISPLAY_API_FUNCS_H
#define __MFW_PRIVATE_RENDERER_X11_XLIB_XLIB_DISPLAY_API_FUNCS_H

#pragma once

#include <private/mfw/renderer/x11/display_api_funcs.hpp>
#include <private/mfw/renderer/x11/xlib/xlib.hpp>

namespace mfw::renderer
{
	class display_api_funcs_xlib : public display_api_funcs_x11
	{
	public:
		bool collect_monitors(ptr_vector<monitor> &monitors) override;
		bool initialize() override { return true; }
		void create_window(window &win, const monitor &monitor, size_t w, size_t h, ssize_t x, ssize_t y) override;
		void show_window(window &win, bool show) override {}
		void destroy_window(window &win) override {}
		void update(monitor &mon) override {}
		void query_data(ucstring_view name, opaque_data &outputs, const opaque_data *inputs = nullptr) override {}

		struct window_display_data_t
		{
			Window id{0};
		};
	};
}

#endif