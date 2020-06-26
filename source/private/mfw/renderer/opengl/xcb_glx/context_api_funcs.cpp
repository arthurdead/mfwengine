#include <private/mfw/renderer/opengl/xcb_glx/context_api_funcs.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_context_api_xcb_glx, u8"renderer/render_api/opengl/context_api/xcb_glx"_p)

	context_api_funcs_xcb_glx &context_api_funcs_xcb_glx::instance() {
		return reinterpret_cast<context_api_funcs_xcb_glx &>(interfaces::context_api_funcs::instance());
	}

	void context_api_funcs_xcb_glx::context_data_t::close()
	{

	}

	void context_api_funcs_xcb_glx::context_data_t::setup_config(xcb_connection_t *xcb_disp, size_t scrn)
	{
		if(config.config == 0) {
			constexpr prop_list_t attribs[]{
				{GLX_DOUBLEBUFFER, 1},
				{GLX_RED_SIZE, 8},
				{GLX_GREEN_SIZE, 8},
				{GLX_BLUE_SIZE, 8},
				{GLX_ALPHA_SIZE, 8},
				{GLX_STENCIL_SIZE, 8},
				{GLX_DEPTH_SIZE, 24},
				{GLX_BUFFER_SIZE, 32},
				{GLX_RENDER_TYPE, GLX_RGBA_BIT},
				{GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT|GLX_PIXMAP_BIT|GLX_PBUFFER_BIT},
				{GLX_X_RENDERABLE, true},
			};

			choose_config(xcb_disp, scrn, attribs, size(attribs), config);

			visual = *config.get_value(GLX_VISUAL_ID);
		}
	}

	xcb_visualid_t context_api_funcs_xcb_glx::get_visual(window &win)
	{
		display_info_t &disp_data{*const_cast<monitor *>(win.mon_)->get_display_data<monitor_display_data_t>().scrn_info->display};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};
		window_render_data_t &win_rendr_data{win.get_or_allocate_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_rendr_data.get_or_allocate_context_data<context_data_t>()};

		xcb_connection_t *xcb_disp{disp_data.xcb_disp};

		ctx_data.setup_config(xcb_disp, disp_data.num);

		return ctx_data.visual;
	}

	void context_api_funcs_xcb_glx::create_window(window &win, const graphics_card &gpu, size_t w, size_t h)
	{
		display_info_t &disp_data{*const_cast<monitor *>(win.mon_)->get_display_data<monitor_display_data_t>().scrn_info->display};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};
		window_render_data_t &win_rendr_data{win.get_or_allocate_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_rendr_data.get_or_allocate_context_data<context_data_t>()};

		xcb_connection_t *xcb_disp{disp_data.xcb_disp};

		ctx_data.setup_config(xcb_disp, disp_data.num);

		ctx_data.context = xcb_generate_id(xcb_disp);
		xcb_glx_create_new_context(xcb_disp, ctx_data.context, ctx_data.config.config, disp_data.num, GLX_RGBA_TYPE, 0, true);

		ctx_data.window = xcb_generate_id(xcb_disp);
		xcb_glx_create_window(xcb_disp, disp_data.num, ctx_data.config.config, win_disp_data.id, ctx_data.window, 0, nullptr);

		ctx_data.make_current(xcb_disp);

		MFW_MESSAGE("TODO")
		render_api_funcs_opengl::instance().on_context_loaded(*win.gpu_, nullptr);
	}

	void context_api_funcs_xcb_glx::choose_config(xcb_connection_t *xcb_disp, int32_t scrn, const prop_list_t *attribs, size_t len, config_t &config)
	{
		__MFW_XCB_REPLY_FUNC(xcb_glx_get_fb_configs, configs, (xcb_disp, scrn))
		prop_list_t *list{reinterpret_cast<prop_list_t *>(xcb_glx_get_fb_configs_property_list(configs_reply))};
		for(int32_t i{0}; i < configs_reply->num_FB_configs; i++) {
			bool config_valid{true};
			for(int32_t j{0}; j < len; j++) {
				const prop_list_t &attrib{attribs[j]};
				uint32_t *value{prop_list_t::get_prop_value(list, configs_reply->num_properties, attrib.name)};
				if(!value || *value != attrib.value) {
					config_valid = false;
					break;
				}
			}
			if(config_valid) {
				config.config = *prop_list_t::get_prop_value(list, configs_reply->num_properties, GLX_FBCONFIG_ID);
				config.num_props = configs_reply->num_properties;
				config.props = list;
				config.idx = i;
				break;
			}
			list += configs_reply->num_properties;
		}
	}

	void context_api_funcs_xcb_glx::context_data_t::make_current(xcb_connection_t *xcb_disp)
	{
		__MFW_XCB_REPLY_FUNC(xcb_glx_make_context_current, make_curr, (xcb_disp, 0, window, window, context));
		tag = make_curr_reply->context_tag;
	}

	void context_api_funcs_xcb_glx::context_data_t::swap_buffers(xcb_connection_t *xcb_disp)
	{
		xcb_glx_swap_buffers(xcb_disp, tag, window);
	}

	void context_api_funcs_xcb_glx::make_current(window &win)
	{
		display_info_t &disp_data{*const_cast<monitor *>(win.mon_)->get_display_data<monitor_display_data_t>().scrn_info->display};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};
		window_render_data_t &win_rendr_data{win.get_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_rendr_data.get_context_data<context_data_t>()};

		xcb_connection_t *xcb_disp{disp_data.xcb_disp};

		ctx_data.make_current(xcb_disp);
	}

	void context_api_funcs_xcb_glx::swap_buffers(window &win)
	{
		display_info_t &disp_data{*const_cast<monitor *>(win.mon_)->get_display_data<monitor_display_data_t>().scrn_info->display};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};
		window_render_data_t &win_rendr_data{win.get_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_rendr_data.get_context_data<context_data_t>()};

		xcb_connection_t *xcb_disp{disp_data.xcb_disp};

		ctx_data.swap_buffers(xcb_disp);
	}
}