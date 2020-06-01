#include <private/mfw/renderer/renderwindow.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#endif

namespace mfw::renderer::agnostic
{
	renderwindow::renderwindow(const monitor *mon)
		: 
	#if MFW_OS == MFW_OS_WINDOWS
		window{nullptr, WS_OVERLAPPEDWINDOW, WS_EX_OVERLAPPEDWINDOW},
	#elif MFW_OS == MFW_OS_LINUX
		window{nullptr, mon->gpu()},
	#endif
		monitor_{mon}
	{
		set_bounds(-1, -1, MFW_RENDERWINDOW_MIN_WIDTH, MFW_RENDERWINDOW_MIN_HEIGHT);
	}

	void renderwindow::monitor_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		monitor_->bounds(!fullscreen, x, y, w, h);
	}

	void renderwindow::max_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		monitor_bounds(x, y, w, h);
	}

	void renderwindow::min_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		monitor_bounds(x, y, nullptr, nullptr);

		if(w) {
			*w = MFW_RENDERWINDOW_MIN_WIDTH;
		}
		if(h) {
			*h = MFW_RENDERWINDOW_MIN_HEIGHT;
		}
	}

#if MFW_OS == MFW_OS_WINDOWS
	int64_t renderwindow::window_proc(uint32_t msg, uint64_t param1, int64_t param2)
	{
		return __super::window_proc(msg, param1, param2);
	}
#endif
}