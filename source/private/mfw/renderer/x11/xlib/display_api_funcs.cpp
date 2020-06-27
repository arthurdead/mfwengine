#include <private/mfw/renderer/x11/xlib/display_api_funcs.hpp>

#include <dirent.h>
#include <cstring>

#ifdef __MFW_PRIVATE_RENDERER_VULKAN_VULKAN_H
	#error
#endif
#define VK_USE_PLATFORM_XLIB_KHR
#include <private/mfw/renderer/vulkan/vulkan.hpp>
#include <private/mfw/renderer/vulkan/shared.hpp>

#ifdef __MFW_PRIVATE_RENDERER_OPENGL_EGL_EGL_H
	#error
#endif
#define USE_X11
#include <private/mfw/renderer/opengl/egl/egl.hpp>
#include <private/mfw/renderer/opengl/egl/shared.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_display_api_xlib, u8"renderer/display_api/x11/xlib"_p)

	bool display_api_funcs_xlib::collect_monitors(ptr_vector<monitor> &monitors)
	{
		DIR *x11dir{opendir("/tmp/.X11-unix")};
		if(x11dir) {
			dirent *entry{nullptr};
			while(true) {
				entry = readdir(x11dir);
				if(!entry) {
					break;
				}

				if((entry->d_name[0] == '.') ||
					(entry->d_name[0] == '.' && entry->d_name[1] == '.')) {
					continue;
				}

				ucstring display_name{};
				display_name.assign(uc_str(entry->d_name), static_cast<size_t>(strlen(entry->d_name)));
				display_name.erase(display_name.cbegin(), display_name.cbegin()+1);
				display_name.insert(0, 1, u8':');

				Display *xlib_disp{XOpenDisplay(c_str(display_name))};
				if(xlib_disp) {
					int32_t numscreen{XScreenCount(xlib_disp)};
					for(int32_t i{0}; i < numscreen; i++) {
						Window root{RootWindow(xlib_disp, i)};
						
						int32_t nummon{0};
						XRRMonitorInfo *moninfos{XRRGetMonitors(xlib_disp, root, true, &nummon)};
						for(int32_t j{0}; j < nummon; j++) {
							XRRMonitorInfo &info{moninfos[j]};
					
							monitor &mon{monitors.emplace_back()};
							MFW_MESSAGE("TODO get the actual monitor name")
							mon.name = uc_str(XGetAtomName(xlib_disp, info.name));
							mon.x = info.x;
							mon.x = info.y;
							mon.width = info.width;
							mon.height = info.height;
							log_display_api_xlib().info(u8"found {}"_sv, mon.name);
							if(!monitor::mainmonitor && info.primary) {
								monitor::mainmonitor = &mon;
							}
						}
						XRRFreeMonitors(moninfos);
					}
				}
			}
			closedir(x11dir);
			log_display_api_xlib().info(u8"selected {} as main monitor"_sv, monitor::mainmonitor->name);
			return !monitors.empty();
		} else {
			return false;
		}
	}

	void display_api_funcs_xlib::create_window(window &win, const monitor &mon, size_t w, size_t h, ssize_t x, ssize_t y)
	{
		
	}
}