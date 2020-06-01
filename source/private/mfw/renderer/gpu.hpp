#ifndef __MFW_PRIVATE_RENDERER_GPU_H
#define __MFW_PRIVATE_RENDERER_GPU_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/forward_list.hpp>
#include <private/mfw/renderer/monitor.hpp>

#if MFW_OS == MFW_OS_WINDOWS
	MFW_MESSAGE("get rid of this")
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <xcb/xcb.h>
#endif

#define MFW_GPU_VENDOR_NVIDIA 0x10DE
#define MFW_GPU_VENDOR_INTEL 0x8086

namespace mfw::renderer
{
	class window;
}

namespace mfw::renderer::agnostic
{
	class gpu
	{
	public:
		friend class mfw::renderer::window;
	
		virtual ~gpu();

		static void initialize();
		static void shutdown() {}

		static const gpu &maingpu() { return *maingpu_; }

		using monitor_list_t = forward_list<monitor>;
		const monitor_list_t &monitors() const { return monitors_; }
		const monitor &main_monitor() const { return monitors_.front(); }
		const ucstring &name() const { return name_; }
		uint32_t vendor_id() const { return vendorid; }
		uint32_t device_id() const { return deviceid; }
		bool primary() const { return primary_; }

		static const gpu *find(uint32_t id);

	protected:
		monitor_list_t monitors_{};
		bool primary_{false};
		ucstring name_{};
		uint32_t vendorid{0};
		uint32_t deviceid{0};

	#if MFW_OS == MFW_OS_WINDOWS
		struct enum_info
		{
			gpu &gpu;
			uint32_t index{0};
			u16string identifier{};
		};

		static int32_t monitor_enum(HMONITOR monitor, HDC device, RECT *rect, intptr_t param);
	#elif MFW_OS == MFW_OS_LINUX
		xcb_connection_t *display_{nullptr};
	#endif

		using gpu_list_t = forward_list<unique_ptr<gpu>>;
		static gpu_list_t &gpus();

		static const gpu *maingpu_;
	};
}

#endif