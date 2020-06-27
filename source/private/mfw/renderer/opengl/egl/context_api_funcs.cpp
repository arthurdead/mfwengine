#include <private/mfw/renderer/opengl/egl/context_api_funcs.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_context_api_egl, u8"renderer/render_api/opengl/context_api/egl"_p)

	context_api_funcs_egl::~context_api_funcs_egl()
	{
		gladLoaderUnloadEGL();

	#if MFW_CONFIGURATION_IS(DEBUG)
		gladUninstallGLDebug();
	#endif
	}

	bool context_api_funcs_egl::initialize()
	{
	#if MFW_CONFIGURATION_IS(DEBUG)
		gladInstallEGLDebug();
	#endif

		if(!gladLoaderLoadEGL(EGL_NO_DISPLAY)) {
			return false;
		}

		return true;
	}

	void context_api_funcs_egl::on_egl_loaded(const graphics_card &gpu)
	{
		render_api_funcs_opengl::instance().on_context_loaded(gpu, eglGetProcAddress);
	}

	void context_api_funcs_egl::context_data_t::close()
	{
		if(refs && *refs > 0) {
			*refs--;
		}

		if(display && surface) {
			eglDestroySurface(display, surface);
			surface = nullptr;
		}

		if(refs) {
			if(*refs == 0 && display) {
				eglTerminate(display);
				display = nullptr;
			}
		} else {
			if(display) {
				eglTerminate(display);
			}
		}
	}

	void context_api_funcs_egl::destroy_window(window &win)
	{
		window_render_data_t &win_data{win.get_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_data.get_context_data<context_data_t>()};

		ctx_data.close();

		ctx_data.refs = nullptr;
	}

	void context_api_funcs_egl::context_data_t::make_current()
	{
		eglMakeCurrent(display, surface, surface, context);
	}

	void context_api_funcs_egl::context_data_t::swap_buffers()
	{
		eglSwapBuffers(display, surface);
	}

	void context_api_funcs_egl::create_window(window &win, const graphics_card &gpu, size_t w, size_t h)
	{
		window_render_data_t &win_data{win.allocate_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_data.allocate_context_data<context_data_t>()};

		opaque_data output{};
		opaque_data input{};
		egl::input_get_display_t &input_get_display{input.allocate<egl::input_get_display_t>()};
		input_get_display.win = &win;
		interfaces::display_api_funcs::instance().query_data(u8"egl_get_display"_sv, output, &input);
		egl::output_get_display_t &output_get_display{output.get<egl::output_get_display_t>()};

		ctx_data.refs = output_get_display.refs;

		bool first{*ctx_data.refs == 1};

		if(first) {
			ctx_data.display = eglGetPlatformDisplayEXT(output_get_display.platform, output_get_display.display, nullptr);
		} else {
			ctx_data.display = eglGetPlatformDisplay(output_get_display.platform, output_get_display.display, nullptr);
		}

		if(first) {
			if(!eglInitialize(ctx_data.display, nullptr, nullptr)) {
				MFW_DEBUGBREAK();
			}

			if(!gladLoaderLoadEGL(ctx_data.display)) {
				MFW_DEBUGBREAK();
			}

			if(!eglBindAPI(EGL_OPENGL_API)) {
				MFW_DEBUGBREAK();
			}

			eglSwapInterval(ctx_data.display, 1);
		}

		constexpr EGLint config_attribs[]{
			EGL_RED_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_BLUE_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_LUMINANCE_SIZE, 0,
			EGL_COLOR_BUFFER_TYPE, EGL_RGB_BUFFER,
			EGL_CONFIG_CAVEAT, EGL_NONE,
			EGL_CONFORMANT, EGL_OPENGL_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
			EGL_NONE
		};

		eglChooseConfig(ctx_data.display, config_attribs, &ctx_data.config, 1, &ctx_data.num_config);

		input.destroy();
		output.destroy();
		egl::input_get_window_t &input_get_win{input.allocate<egl::input_get_window_t>()};
		input_get_win.win = &win;
		interfaces::display_api_funcs::instance().query_data(u8"egl_get_window"_sv, output, &input);
		egl::output_get_window_t &output_get_win{output.get<egl::output_get_window_t>()};

		//#define __MFW_USE_PLATFORM_WINDOW_SURFACE

		constexpr
	#ifdef __MFW_USE_PLATFORM_WINDOW_SURFACE
		EGLAttrib
	#else
		EGLint
	#endif
		surface_attribs[]{
			EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
			EGL_NONE,
		};

	#ifdef __MFW_USE_PLATFORM_WINDOW_SURFACE
		ctx_data.surface = eglCreatePlatformWindowSurface(ctx_data.display, ctx_data.config, output_get_win.win, surface_attribs);
	#else
		ctx_data.surface = eglCreateWindowSurface(ctx_data.display, ctx_data.config, *reinterpret_cast<EGLNativeWindowType *>(output_get_win.win), surface_attribs);
	#endif

		ctx_data.context = eglCreateContext(ctx_data.display, ctx_data.config, EGL_NO_CONTEXT, nullptr);

		ctx_data.make_current();

		if(first) {
			on_egl_loaded(*win.gpu_);
		}
	}

	void context_api_funcs_egl::make_current(window &win)
	{
		window_render_data_t &win_data{win.get_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_data.get_context_data<context_data_t>()};

		ctx_data.make_current();
	}

	void context_api_funcs_egl::swap_buffers(window &win)
	{
		window_render_data_t &win_data{win.get_render_data<window_render_data_t>()};
		context_data_t &ctx_data{win_data.get_context_data<context_data_t>()};

		ctx_data.swap_buffers();
	}
}