#include <private/mfw/renderer/renderer.hpp>
#include <private/mfw/renderer/window.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/renderer/display_api.hpp>
#include <private/mfw/renderer/render_api.hpp>
#include <private/mfw/renderer/graphics_card.hpp>
#include <private/mfw/renderer/monitor.hpp>
#include <private/mfw/renderer/render_api_funcs.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_renderer, u8"renderer"_p)
	MFW_DECLARE_GLOBAL_ALLOCATOR(renderer, ::mfw::renderer::renderer)

	mfw::renderer::renderer &mfw::renderer::renderer::instance() {
		return __renderer_global_allocator.instance();
	}

	MFW_RENDERER_API interfaces::renderer & MFW_RENDERER_CALL interfaces::renderer::instance()
		{ return mfw::renderer::renderer::instance(); }

	core::exit_status mfw::renderer::renderer::initialize()
	{
		if(!detect_display_api()) {
			return core::exit_status::fatal;
		}

		if(!detect_render_api()) {
			return core::exit_status::fatal;
		}

		if(!graphics_card::initialize()) {
			return core::exit_status::fatal;
		}

		if(!monitor::initialize()) {
			return core::exit_status::fatal;
		}

		if(!window::initialize()) {
			return core::exit_status::fatal;
		}

		if(!interfaces::render_api_funcs::instance().pre_initialize()) {
			return core::exit_status::fatal;
		}

		return {};
	}

	core::exit_status mfw::renderer::renderer::initialize_render_api()
	{
		if(!interfaces::render_api_funcs::instance().initialize()) {
			return core::exit_status::fatal;
		}

		return {};
	}

	core::exit_status mfw::renderer::renderer::shutdown()
	{
		graphics_card::shutdown();
		window::shutdown();

		return {};
	}

	core::exit_status mfw::renderer::renderer::update()
	{
		window::update();

		return {};
	}

	/*gpu *mfw::renderer::renderer::create_gpu() const
	{
		return new gpu{};
	}*/

	/*
	renderwindow *mfw::renderer::renderer::create_window() const
	{
		const gpu &gpu_{gpu::maingpu()};
		return new renderwindow{&gpu_.main_monitor()};
	}
	*/
	
	void mfw::renderer::renderer::do_stuff()
	{
		/*
		renderwindow *win = create_window();
		win->enable(true);
		*/
		monitor &mon{monitor::main_monitor()};
		graphics_card &gpu{graphics_card::main_gpu()};
		window *test{new window{mon, gpu, 1600, 900, 0, 0}};
		test->on_render([test]{
			interfaces::render_api_funcs::instance().render_window(*test);
		});
		test->show(true);
	}
}