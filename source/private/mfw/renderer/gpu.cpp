#include <private/mfw/renderer/gpu.hpp>
#include <private/mfw/renderer/renderer.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <ShellScalingApi.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <dirent.h>
	#include <cstring>
	#include <xcb/xcb.h>
#endif

namespace mfw::renderer::agnostic
{
	const gpu *gpu::maingpu_{nullptr};

	gpu::gpu_list_t &gpu::gpus() {
		static gpu_list_t _gpulist{};
		return _gpulist;
	}
	
	gpu::~gpu()
	{
	#if MFW_OS == MFW_OS_LINUX
		xcb_disconnect(display_);
	#endif
	}

	void gpu::initialize()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
		SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);

		for(uint32_t i{0}; ; i++) {
			DISPLAY_DEVICEW display{};
			display.cb = sizeof(DISPLAY_DEVICEW);

			if(!EnumDisplayDevicesW(nullptr, i, &display, 0)) {
				break;
			}

			if(!bool_cast(display.StateFlags & DISPLAY_DEVICE_ACTIVE)) {
				continue;
			}

			gpus().emplace_front();
			gpu_list_t::value_type &gpu_{gpus().front()};
			gpu_.reset(agnostic::renderer::instance().create_gpu());

			gpu_->name_ = uc_str(display.DeviceString);

			ucstring deviceid{uc_str(display.DeviceID)};
			ucstring devidsub{deviceid.substr(17, 4)};
			to_int(devidsub, gpu_->deviceid, 16);

			ucstring venidsub{deviceid.substr(8, 4)};
			to_int(venidsub, gpu_->vendorid, 16);

			gpu_->primary_ = bool_cast(display.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE);

			if(gpu_->primary_) {
				maingpu_ = gpu_.get();
			}

			DEVMODEW mode{};
			mode.dmSize = sizeof(DEVMODEW);
			EnumDisplaySettingsW(display.DeviceName, ENUM_CURRENT_SETTINGS, &mode);

			RECT rect{};
			rect.left = mode.dmPosition.x;
			rect.top = mode.dmPosition.y;
			rect.right = (mode.dmPosition.x + static_cast<int32_t>(mode.dmPelsWidth));
			rect.bottom = (mode.dmPosition.y + static_cast<int32_t>(mode.dmPelsHeight));

			const HDC device{CreateDCW(display.DeviceName, display.DeviceString, nullptr, &mode)};

			enum_info info{*gpu_};
			info.index = 0;
			info.identifier = uc_str(display.DeviceName);

			EnumDisplayMonitors(device, &rect, reinterpret_cast<MONITORENUMPROC>(gpu::monitor_enum), reinterpret_cast<int64_t>(&info));

			DeleteDC(device);
		}
	#elif MFW_OS == MFW_OS_LINUX
		DIR *x11dir{opendir("/tmp/.X11-unix")};
		if(x11dir) {
			dirent *entry{nullptr};
			while(true) {
				entry = readdir(x11dir);
				if(!entry) {
					break;
				}
				
				if(entry->d_name[0] != 'X') {
					continue;
				}
				
				char display_name[64]{":"};
				strcat(display_name, entry->d_name + 1);
				
				xcb_connection_t *xdisp{xcb_connect(display_name, nullptr)};
				if(!xcb_connection_has_error(xdisp)) {
					gpus().emplace_front();
					gpu_list_t::value_type &gpu_{gpus().front()};
					gpu_.reset(agnostic::renderer::instance().create_gpu());
					
					gpu_->name_ = {};
					gpu_->primary_ = true;
					
					gpu_->display_ = xdisp;
					
					gpu_->deviceid = 0;
					gpu_->vendorid = 0;
					
					if(gpu_->primary_) {
						maingpu_ = gpu_.get();
					}
					
					const xcb_setup_t *setup{xcb_get_setup(xdisp)};
					
					//xcb_screen_iterator_t it{xcb_setup_roots_iterator(setup)};
					
					int32_t numscreen{xcb_setup_roots_length(setup)};
					for(int32_t i{0}; i < numscreen; i++) {
						
					}
					
					MFW_DEBUGBREAK();
					
					/*int32_t numscreen{XScreenCount(xdisp)};
					for(int32_t i{0}; i < numscreen; i++) {
						Window root{RootWindow(xdisp, i)};
						
						int32_t nummon{0};
						XRRMonitorInfo *moninfos{XRRGetMonitors(xdisp, root, true, &nummon)};
						for(int32_t j{0}; j < nummon; j++) {
							XRRMonitorInfo &info{moninfos[j]};
							
							gpu_->monitors_.emplace_front();
							monitor *mon{&gpu_->monitors_.front()};
							
							mon->gpu_ = gpu_.get();
							
							mon->name = uc_str(XGetAtomName(xdisp, info.name));
							
							mon->primary_ = info.primary;
							
							set_rect_bounds(mon->mrect, info.x, info.y, info.width, info.height);
							mon->wrect = mon->mrect;
							
							mon->dpix = ((info.width * 25.4) / info.mwidth);
							mon->dpiy = ((info.height * 25.4) / info.mheight);
						}
						XRRFreeMonitors(moninfos);
					}*/
				}
			}
			closedir(x11dir);
		}
	#else
		#error
	#endif
	}

#if MFW_OS == MFW_OS_WINDOWS
	int32_t gpu::monitor_enum(HMONITOR hmon, HDC device, RECT *rect, intptr_t param)
	{
		enum_info *einfo{reinterpret_cast<enum_info *>(param)};

		const ucstring &identifier{einfo->identifier};

		gpu &gpu_{einfo->gpu};

		uint32_t index{einfo->index};
		einfo->index++;

		DISPLAY_DEVICEW display{};
		display.cb = sizeof(DISPLAY_DEVICEW);

		if(!EnumDisplayDevicesW(c_str(identifier), index, &display, 0)) {
			return true;
		}

		if(!bool_cast(display.StateFlags & DISPLAY_DEVICE_ATTACHED)) {
			return true;
		}

		if(!bool_cast(display.StateFlags & DISPLAY_DEVICE_ACTIVE)) {
			return true;
		}

		gpu_.monitors_.emplace_front();
		monitor *mon{&gpu_.monitors_.front()};
		mon->monitor_ = hmon;

		MONITORINFOEXW info{};
		info.cbSize = sizeof(MONITORINFOEXW);

		GetMonitorInfoW(mon->monitor_, &info);

		mon->mrect = info.rcMonitor;
		mon->wrect = info.rcWork;

		GetDpiForMonitor(mon->monitor_, MDT_EFFECTIVE_DPI, &mon->dpix, &mon->dpiy);

		mon->name = uc_str(display.DeviceString);

		return true;
	}
#endif

	const gpu *gpu::find(uint32_t id)
	{
		for(const gpu_list_t::value_type &it : gpus()) {
			if(it->device_id() == id) {
				return it.get();
			}
		}
		return nullptr;
	}
};