#ifndef __MFW_PRIVATE_RENDERER_MONITOR_H
#define __MFW_PRIVATE_RENDERER_MONITOR_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <private/mfw/renderer/helpers.hpp>

#if MFW_OS == MFW_OS_WINDOWS
	MFW_MESSAGE("get rid of this")
	#include <Windows.h>
#endif

namespace mfw::renderer
{
	namespace agnostic
	{
		class gpu;
	}

	class monitor
	{
	public:
		friend class agnostic::gpu;

		const RECT &work_rect() const { return wrect; }
		const RECT &mon_rect() const { return mrect; }
		const RECT &rect(bool work) const { return (work ? work_rect() : mon_rect()); }

		void mon_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
			{ bounds(false, x, y, w, h); }
		void work_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
			{ bounds(true, x, y, w, h); }

		void bounds(bool work, int32_t *x, int32_t *y, int32_t *w, int32_t *h) const;
		
		const agnostic::gpu &gpu() const { return *gpu_; }
		bool primary() const { return primary_; }

	private:
	#if MFW_OS == MFW_OS_WINDOWS
		HMONITOR monitor_{nullptr};
	#endif
	
		bool primary_{false};
		agnostic::gpu *gpu_{nullptr};

		RECT mrect{};
		RECT wrect{};

		uint32_t dpix{0};
		uint32_t dpiy{0};

		ucstring name{};
	};
}

#endif