#ifndef __MFW_PRIVATE_RENDERER_HELPERS_H
#define __MFW_PRIVATE_RENDERER_HELPERS_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_OS == MFW_OS_WINDOWS
	MFW_MESSAGE("get rid of this")
	#include <Windows.h>
#endif

namespace mfw::renderer
{
#if MFW_OS == MFW_OS_LINUX
	struct RECT
	{
		int32_t left{0};
		int32_t top{0};
		int32_t bottom{0};
		int32_t right{0};
	};
#endif

	void set_rect_bounds(RECT &rect, int32_t x, int32_t y, int32_t w, int32_t h);
	void get_rect_bounds(const RECT &rect, int32_t *x, int32_t *y, int32_t *w, int32_t *h);
}

#endif