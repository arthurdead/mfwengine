#include <private/mfw/renderer/xcb/display_api_funcs.hpp>

#include <dirent.h>
#include <cstring>

#include <private/mfw/renderer/opengl/context_api.hpp>

#ifdef __MFW_PRIVATE_RENDERER_VULKAN_VULKAN_H
	#error
#endif
#define VK_USE_PLATFORM_XCB_KHR
#include <private/mfw/renderer/vulkan/vulkan.hpp>
#include <private/mfw/renderer/vulkan/shared.hpp>

#ifdef __MFW_PRIVATE_RENDERER_OPENGL_EGL_EGL_H
	#error
#endif
#define USE_X11
#include <private/mfw/renderer/opengl/egl/egl.hpp>
#include <private/mfw/renderer/opengl/egl/shared.hpp>

#ifdef __MFW_PRIVATE_RENDERER_XCB_GLX_H
	#error
#endif
#include <private/mfw/renderer/xcb/glx/context_api_funcs.hpp>
#include <private/mfw/renderer/xcb/glx/glx.hpp>
#include <private/mfw/renderer/xcb/glx/shared.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_display_api_xcb, u8"renderer/display_api/xcb"_p)

	display_api_funcs_xcb::display_info_t::~display_info_t()
	{
		delete close_reply;
		delete protocols_reply;

		xcb_disconnect(xcb_disp);

		if(x11_disp) {
			XCloseDisplay(x11_disp);
		}
	}

	Display *display_api_funcs_xcb::display_info_t::get_x11_disp()
	{
		x11_ref++;

		if(x11_ref == 1) {
			x11_disp = XOpenDisplay(c_str(name));
			XSetEventQueueOwner(x11_disp, XCBOwnsEventQueue);
		}

		return x11_disp;
	}

	void display_api_funcs_xcb::display_info_t::close_x11_disp()
	{
		if(x11_ref > 0) {
			x11_ref--;
		}

		if(x11_ref == 0) {
			XCloseDisplay(x11_disp);
			x11_disp = nullptr;
		}
	}

	bool display_api_funcs_xcb::collect_monitors(ptr_vector<monitor> &monitors)
	{
		DIR *x11dir{opendir("/tmp/.X11-unix")};
		if(x11dir) {
			dirent *entry{nullptr};
			while(true) {
				entry = readdir(x11dir);
				if(!entry) {
					break;
				}

				if((entry->d_name[0] == '.') ||
					(entry->d_name[0] == '.' && entry->d_name[1] == '.')) {
					continue;
				}

				ucstring display_name{};
				display_name.assign(uc_str(entry->d_name), static_cast<size_t>(strlen(entry->d_name)));
				display_name.erase(display_name.cbegin(), display_name.cbegin()+1);
				display_name.insert(0, 1, u8':');

				int32_t disp_num{0};
				xcb_connection_t *xcb_disp{xcb_connect(c_str(display_name), &disp_num)};
				int32_t has_error{xcb_connection_has_error(xcb_disp)};
				if(!has_error) {
					const xcb_setup_t *setup{xcb_get_setup(xcb_disp)};

					display_info_t &disp_info{displays.emplace_back()};
					disp_info.xcb_disp = xcb_disp;
					disp_info.name = move(display_name);
					disp_info.num = disp_num;

					__MFW_XCB_REPLY_FUNC(xcb_intern_atom, proto, (xcb_disp, true, 12, "WM_PROTOCOLS"))
					__MFW_XCB_REPLY_FUNC(xcb_intern_atom, close, (xcb_disp, false, 16, "WM_DELETE_WINDOW"))

					disp_info.protocols_reply = proto_reply;
					disp_info.close_reply = close_reply;

					int32_t scrn_len{xcb_setup_roots_length(setup)};
					xcb_screen_iterator_t scrn_it{xcb_setup_roots_iterator(setup)};
					for(int32_t scrn_i{0}; scrn_i < scrn_len; scrn_i++) {
						xcb_screen_t &scrn{*scrn_it.data};

						screen_info_t &scrn_info{disp_info.screens.emplace_back()};
						scrn_info.display = &disp_info;
						scrn_info.root = scrn.root;
						scrn_info.visual = scrn.root_visual;
						scrn_info.white_pixel = scrn.white_pixel;

						/*__MFW_XCB_REPLY_FUNC(xcb_randr_get_providers, prov, (xcb_disp, scrn.root))
						__MFW_XCB_ARR_FUNC(xcb_randr_get_providers_providers, prov, xcb_randr_provider_t, {
							__MFW_XCB_REPLY_FUNC(xcb_randr_get_provider_info, prov_info, (xcb_disp, prov_data, 0))
							__MFW_XCB_NAME_FUNC(xcb_randr_get_provider_info, prov_info, {
								ucstring prov_name{};
								__MFW_XCB_MAKE_NAME(prov_name, prov_info)
								MFW_DEBUGBREAK();
							})
						})*/

						__MFW_XCB_REPLY_FUNC(xcb_randr_get_monitors, mon, (xcb_disp, scrn.root, true))
						__MFW_XCB_ITER_FUNC(xcb_randr_get_monitors_monitors, mon, xcb_randr_monitor_info, {
							MFW_MESSAGE("TODO get the actual monitor name")
							ucstring mon_name{};
							get_atom_string(xcb_disp, mon_data.name, mon_name);
							monitor &mon_info{monitors.emplace_back()};
							mon_info.name = mon_name;
							mon_info.width = mon_data.width;
							mon_info.height = mon_data.height;
							mon_info.x = mon_data.x;
							mon_info.y = mon_data.y;
							monitor_display_data_t &disp_data{mon_info.allocate_display_data<monitor_display_data_t>()};
							disp_data.scrn_info = &scrn_info;
							log_display_api_xcb().info(u8"found {}"_sv, mon_name);
							if(mon_data.primary && !monitor::mainmonitor) {
								monitor::mainmonitor = &mon_info;
							}
						})

						xcb_screen_next(&scrn_it);
					}
				}
			}
			closedir(x11dir);
			log_display_api_xcb().info(u8"selected {} as main monitor"_sv, monitor::mainmonitor->name);
			return !monitors.empty();
		} else {
			return false;
		}
	}

	void display_api_funcs_xcb::create_window(window &win, const monitor &mon, size_t w, size_t h, ssize_t x, ssize_t y)
	{
		monitor_display_data_t &mon_disp_data{const_cast<monitor &>(mon).get_display_data<monitor_display_data_t>()};
		screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		display_info_t &disp_info{*scrn_info.display};
		window_display_data_t &win_disp_data{win.allocate_display_data<window_display_data_t>()};

		xcb_connection_t *xcb_disp{disp_info.xcb_disp};

		xcb_visualid_t visual{0};
		if(get_context_api() == context_api::xcb_glx) {
			visual = context_api_funcs_xcb_glx::instance().get_visual(win);
		} else {
			visual = scrn_info.visual;
		}

		if(scrn_info.colormap == 0) {
			scrn_info.colormap = xcb_generate_id(xcb_disp);
			xcb_create_colormap(xcb_disp, XCB_COLORMAP_ALLOC_NONE, scrn_info.colormap, scrn_info.root, visual);
		}

		win_disp_data.id = xcb_generate_id(xcb_disp);
		uint32_t values[]{
			scrn_info.white_pixel,
			0,
			XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_STRUCTURE_NOTIFY,
			scrn_info.colormap,
			0,
		};
		constexpr uint32_t mask{
			XCB_CW_BACK_PIXEL|
			XCB_CW_OVERRIDE_REDIRECT|
			XCB_CW_EVENT_MASK|
			XCB_CW_COLORMAP
		};
		xcb_create_window(xcb_disp, XCB_COPY_FROM_PARENT, win_disp_data.id, scrn_info.root, x, y, w, h, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, visual, mask, values);

		xcb_change_property(xcb_disp, XCB_PROP_MODE_REPLACE, win_disp_data.id, disp_info.protocols_reply->atom, 4, 32, 1, &disp_info.close_reply->atom);

		window_map.emplace(window_map_t::value_type{win_disp_data.id, &win});
	}

	void display_api_funcs_xcb::show_window(window &win, bool show)
	{
		monitor &mon{*const_cast<monitor *>(win.mon_)};
		monitor_display_data_t &mon_disp_data{mon.get_display_data<monitor_display_data_t>()};
		screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};

		xcb_connection_t *xcb_disp{scrn_info.xcb_disp()};

		if(show) {
			xcb_map_window(xcb_disp, win_disp_data.id);
		} else {
			xcb_unmap_window(xcb_disp, win_disp_data.id);
		}
	}

	void display_api_funcs_xcb::update(monitor &mon)
	{
		monitor_display_data_t &mon_disp_data{mon.get_display_data<monitor_display_data_t>()};
		screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		display_info_t &disp_info{*scrn_info.display};

		xcb_connection_t *xcb_disp{scrn_info.xcb_disp()};

		xcb_flush(xcb_disp);

		xcb_generic_event_t *base_event{xcb_poll_for_event(xcb_disp)};
		if(base_event) {
			MFW_MESSAGE("why do i need this bit and")
			int32_t type{base_event->response_type & ~0x80};
			switch(type) {
				case XCB_EXPOSE: {
					xcb_expose_event_t *event{reinterpret_cast<xcb_expose_event_t *>(base_event)};
					window *win{window_map.find(event->window)->second};
					win->on_internal_event(window::internal_event_type::render, nullptr);
					break;
				}
				case XCB_DESTROY_NOTIFY: {
					xcb_destroy_notify_event_t *event{reinterpret_cast<xcb_destroy_notify_event_t *>(base_event)};
					window *win{window_map.find(event->window)->second};
					win->on_internal_event(window::internal_event_type::destroyed, nullptr);
					break;
				}
				case XCB_CLIENT_MESSAGE: {
					xcb_client_message_event_t *event{reinterpret_cast<xcb_client_message_event_t *>(base_event)};
					if(event->data.data32[0] == disp_info.close_reply->atom) {
						window *win{window_map.find(event->window)->second};
						win->on_internal_event(window::internal_event_type::closed, nullptr);
					}
					break;
				}
			}
		}
	}

	void display_api_funcs_xcb::destroy_window(window &win)
	{
		monitor &mon{*const_cast<monitor *>(win.mon_)};
		monitor_display_data_t &mon_disp_data{mon.get_display_data<monitor_display_data_t>()};
		screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};

		xcb_connection_t *xcb_disp{scrn_info.xcb_disp()};

		window_map.erase(win_disp_data.id);

		if(win.destroyed_from != window::destroy_from::event) {
			xcb_destroy_window(xcb_disp, win_disp_data.id);
		}

		scrn_info.display->close_x11_disp();
	}

	void display_api_funcs_xcb::query_data(ucstring_view name, opaque_data &outputs, const opaque_data *inputs)
	{
		if(name == u8"vulkan_surface_ext_name"_sv) {
			const ucchar_t *&ptr{outputs.allocate<const ucchar_t *>()};
			ptr = uc_str(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
		} else if(name == u8"vulkan_create_surface"_sv) {
			vulkan::input_create_window_t &input_data{const_cast<opaque_data *>(inputs)->get<vulkan::input_create_window_t>()};
			window_display_data_t &win_data{input_data.win->get_display_data<window_display_data_t>()};
			screen_info_t &scrn_data{*input_data.win->mon_->get_display_data<monitor_display_data_t>().scrn_info};
			vk::XcbSurfaceCreateInfoKHR info{};
			xcb_connection_t *xcb_disp{scrn_data.xcb_disp()};
			info.connection = xcb_disp;
			info.window = win_data.id;
			vk::UniqueSurfaceKHR surface{MFW_VKRES((*input_data.instance)->createXcbSurfaceKHRUnique(move(info), &vulkan::alloc_callbacks()))};
			outputs.allocate<vk::UniqueSurfaceKHR>() = move(surface);
		} else if(name == u8"egl_get_display"_sv) {
			egl::input_get_display_t &input_data{const_cast<opaque_data *>(inputs)->get<egl::input_get_display_t>()};
			screen_info_t &scrn_data{*input_data.win->mon_->get_display_data<monitor_display_data_t>().scrn_info};
			egl::output_get_display_t &display{outputs.allocate<egl::output_get_display_t>()};
			display.platform = EGL_PLATFORM_X11_KHR;
			display.display = scrn_data.display->get_x11_disp();
			display.refs = &scrn_data.display->x11_ref;
		} else if(name == u8"egl_get_window"_sv) {
			egl::input_get_display_t &input_data{const_cast<opaque_data *>(inputs)->get<egl::input_get_display_t>()};
			window_display_data_t &win_data{input_data.win->get_display_data<window_display_data_t>()};
			egl::output_get_window_t &display{outputs.allocate<egl::output_get_window_t>()};
			display.win = reinterpret_cast<EGLNativeWindowType *>(&win_data.id);
		}
	}
}