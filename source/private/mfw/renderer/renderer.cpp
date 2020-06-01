#include <private/mfw/renderer/renderer.hpp>
#include <private/mfw/renderer/window.hpp>

namespace mfw::renderer
{
	MFW_RENDERER_API interfaces::renderer & MFW_RENDERER_CALL interfaces::renderer::instance()
		{ return agnostic::renderer::instance(); }

	namespace agnostic
	{
		core::exit_status renderer::initialize()
		{
			gpu::initialize();
			window::initialize();

			return {};
		}

		core::exit_status renderer::shutdown()
		{
			gpu::shutdown();
			window::shutdown();

			return {};
		}

		core::exit_status renderer::update()
		{
			window::update();

			return {};
		}

		gpu *renderer::create_gpu() const
		{
			return new gpu{};
		}

		renderwindow *renderer::create_window() const
		{
			const gpu &gpu_{gpu::maingpu()};
			return new renderwindow{&gpu_.main_monitor()};
		}
		
		void renderer::do_stuff()
		{
			renderwindow *win = create_window();
			win->enable(true);
		}
	}
}