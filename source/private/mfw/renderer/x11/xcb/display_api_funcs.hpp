#ifndef __MFW_PRIVATE_RENDERER_X11_XCB_XCB_DISPLAY_API_FUNCS_HPP
#define __MFW_PRIVATE_RENDERER_X11_XCB_XCB_DISPLAY_API_FUNCS_HPP

#pragma once

#include <private/mfw/renderer/x11/display_api_funcs.hpp>
#include <private/mfw/renderer/x11/xcb/xcb.hpp>

namespace mfw::renderer
{
	class display_api_funcs_xcb : public display_api_funcs_x11
	{
	public:
		bool collect_monitors(ptr_vector<monitor> &monitors) override;
		bool initialize() override { return true; }
		void create_window(window &win, const monitor &monitor, size_t w, size_t h, ssize_t x, ssize_t y) override;
		void show_window(window &win, bool show) override;
		void update(monitor &mon) override;
		void destroy_window(window &win) override;
		void query_data(ucstring_view name, opaque_data &outputs, const opaque_data *inputs = nullptr) override;

		struct screen_info_t;

		struct display_info_t
		{
			~display_info_t();

			ptr_vector<screen_info_t> screens{};
			ucstring name{};

			xcb_connection_t *xcb_disp{nullptr};
			int32_t num{0};

			Display *x11_disp{nullptr};
			size_t x11_ref{0};

			Display *get_x11_disp();
			void close_x11_disp();

			xcb_intern_atom_reply_t *protocols_reply{nullptr};
			xcb_intern_atom_reply_t *close_reply{nullptr};
		};

		struct screen_info_t
		{
			~screen_info_t() { close(); }

			display_info_t *display{nullptr};
			xcb_window_t root{0};
			xcb_visualid_t visual{0};
			uint32_t white_pixel{0};

			xcb_connection_t *xcb_disp() { return display->xcb_disp; }
			const xcb_connection_t *xcb_disp() const { return display->xcb_disp; }

			xcb_colormap_t colormap{0};

			void close() {}
		};

		struct monitor_display_data_t
		{
			screen_info_t *scrn_info{nullptr};
		};

		struct window_display_data_t
		{
			xcb_window_t id{0};
		};

		using window_map_t = unordered_map<xcb_window_t, window *>;
		window_map_t window_map{};

		ptr_vector<display_info_t> displays{};
	};
}

#endif