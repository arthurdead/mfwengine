#ifndef __MFW_PRIVATE_RENDERER_RENDERWINDOW_H
#define __MFW_PRIVATE_RENDERER_RENDERWINDOW_H

#pragma once

#include <public/mfw/stl/version.hpp>

#include <private/mfw/renderer/window.hpp>
#include <private/mfw/renderer/monitor.hpp>

#define MFW_RENDERWINDOW_MIN_WIDTH 320
#define MFW_RENDERWINDOW_MIN_HEIGHT 280

namespace mfw::renderer::agnostic
{
	class renderwindow : public window
	{
	public:
		renderwindow(const monitor *mon);

		void monitor_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const;

	protected:
	#if MFW_OS == MFW_OS_WINDOWS
		int64_t window_proc(uint32_t msg, uint64_t param1, int64_t param2) override;
	#endif
		void max_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const override;
		void min_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const override;

		bool fullscreen{false};
		const monitor *monitor_{nullptr};
	};
};

#endif