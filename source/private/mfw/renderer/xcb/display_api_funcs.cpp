#include <private/mfw/renderer/xcb/display_api_funcs.hpp>
#include <public/mfw/core/filesystem_interface.hpp>

#include <dirent.h>
#include <cstring>
#include <xcb/xproto.h>
#include <xcb/randr.h>
#include <xcb/glx.h>

namespace mfw::renderer
{
	#define __MFW_XCB_REPLY_FUNC(base, var, cargs) \
		base##_cookie_t var##_cookie{base##_unchecked cargs }; \
		base##_reply_t *var##_reply{base##_reply(xdisp, var##_cookie, nullptr)};

	#define __MFW_XCB_ITER_FUNC(base, var, type, ...) \
		if(var##_reply) { \
			int mon_len{base##_length(var##_reply)}; \
			type##_iterator_t var##_it{base##_iterator(var##_reply)}; \
			for(int var##_i = 0; var##_i < var##_len; var##_i++) { \
				type##_t &var##_data{*var##_it.data}; \
				__VA_ARGS__ \
				type##_next(&var##_it); \
			} \
			delete var##_reply; \
		}

	#define __MFW_XCB_ARR_FUNC(base, var, type, ...) \
		if(var##_reply) { \
			int var##_len{base##_length(var##_reply)}; \
			type *var##_arr{base(var##_reply)}; \
			for(int var##_i = 0; var##_i < var##_len; prov_i++) { \
				type &var##_data{var##_arr[var##_i]}; \
				__VA_ARGS__ \
			} \
			delete var##_reply; \
		}

	#define __MFW_XCB_MAKE_NAME(str, var) \
		str.assign(uc_str(var##_nm_ptr), static_cast<size_t>(var##_nm_len));

	#define __MFW_XCB_NAME_FUNC(base, var, ...) \
		if(var##_reply) { \
			int var##_nm_len{base##_name_length(var##_reply)}; \
			char *var##_nm_ptr{base##_name(var##_reply)}; \
			__VA_ARGS__ \
			delete var##_reply; \
		}

	namespace __display_api_funcs_internal
	{
		static void get_atom_string(xcb_connection_t *xdisp, xcb_atom_t atom, ucstring &str)
		{
			__MFW_XCB_REPLY_FUNC(xcb_get_atom_name, atm_name, (xdisp, atom))
			__MFW_XCB_NAME_FUNC(xcb_get_atom_name, atm_name, {
				__MFW_XCB_MAKE_NAME(str, atm_name)
			})
		}

		static void read_file_int(ucstring_view path, size_t &i)
		{
			FILE *tmp_file{fopen(c_str(path), "r")};
			if(!tmp_file) {
				return;
			}

			ucstring str{};
			while(true) {
				char c{'\0'};
				if(!fread(&c, sizeof(char), 1, tmp_file) ||
					c == '\0' ||
					c == '\n') {
					break;
				}
				str += c;
			}

			fclose(tmp_file);

			str.erase(str.begin(), str.cbegin()+2);

			to_int(str, i, 16);
		}

		template <typename T>
		static bool get_cpu_list(ptr_vector<graphics_card> &gpus)
		{
			constexpr ucstring_view dir{u8"/sys/bus/pci/devices"_sv};
			DIR *devdir{opendir(c_str(dir))};
			if(devdir) {
				dirent *entry{nullptr};
				while(true) {
					entry = readdir(devdir);
					if(!entry) {
						break;
					}

					if((entry->d_name[0] == '.') ||
						(entry->d_name[0] == '.' && entry->d_name[1] == '.')) {
						continue;
					}

					ucstring tmp{dir};
					tmp += u8'/';
					tmp.append(uc_str(entry->d_name), static_cast<size_t>(strlen(entry->d_name)));
					tmp.append(u8"/class"_sv);

					size_t class_id{0};
					read_file_int(tmp, class_id);

					#define __MFW_DEVICE_CLASS_VGA 0x030000
					#define __MFW_DEVICE_CLASS_XGA 0x030100
					#define __MFW_DEVICE_CLASS_3D 0x030200
					#define __MFW_DEVICE_CLASS_OTHER 0x038000

					if(class_id != __MFW_DEVICE_CLASS_VGA &&
						class_id != __MFW_DEVICE_CLASS_XGA &&
						class_id != __MFW_DEVICE_CLASS_3D &&
						class_id != __MFW_DEVICE_CLASS_OTHER) {
						continue;
					}

					tmp.erase(tmp.cend()-6, tmp.cend());
					tmp.append(u8"/vendor"_sv);

					size_t vendor_id{0};
					read_file_int(tmp, vendor_id);

					tmp.erase(tmp.cend()-7, tmp.cend());
					tmp.append(u8"/device"_sv);

					size_t device_id{0};
					read_file_int(tmp, device_id);

					graphics_card &gpu{gpus.emplace_back()};
					gpu.allocate_display_data<typename T::gpu_display_data_t>();
					gpu.vendor = vendor_id;
					gpu.device = device_id;
				}
				closedir(devdir);
				return !gpus.empty();
			} else {
				return false;
			}
		}

		ptr_vector<display_api_funcs_xcb::screen_info_t> __scrns{};
	}

	bool display_api_funcs_xcb::collect_gpus(ptr_vector<graphics_card> &gpus)
	{
		return __display_api_funcs_internal::get_cpu_list<display_api_funcs_xcb>(gpus);
	}

	bool display_api_funcs_xcb::collect_monitors(ptr_vector<monitor> &monitors)
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

				xcb_connection_t *xdisp{xcb_connect(c_str(display_name), nullptr)};
				int has_error{xcb_connection_has_error(xdisp)};
				if(!has_error) {
					const xcb_setup_t *setup{xcb_get_setup(xdisp)};

					int scrn_len{xcb_setup_roots_length(setup)};
					xcb_screen_iterator_t scrn_it{xcb_setup_roots_iterator(setup)};
					for(int scrn_i = 0; scrn_i < scrn_len; scrn_i++) {
						xcb_screen_t &scrn{*scrn_it.data};

						screen_info_t &scrn_info{__display_api_funcs_internal::__scrns.emplace_back()};
						scrn_info.xdisp = xdisp;
						scrn_info.root = scrn.root;
						scrn_info.visual = scrn.root_visual;
						scrn_info.white_pixel = scrn.white_pixel;

						/*__MFW_XCB_REPLY_FUNC(xcb_randr_get_providers, prov, (xdisp, scrn.root))
						__MFW_XCB_ARR_FUNC(xcb_randr_get_providers_providers, prov, xcb_randr_provider_t, {
							__MFW_XCB_REPLY_FUNC(xcb_randr_get_provider_info, prov_info, (xdisp, prov_data, 0))
							__MFW_XCB_NAME_FUNC(xcb_randr_get_provider_info, prov_info, {
								ucstring prov_name{};
								__MFW_XCB_MAKE_NAME(prov_name, prov_info)
								MFW_DEBUGBREAK();
							})
						})*/

						__MFW_XCB_REPLY_FUNC(xcb_randr_get_monitors, mon, (xdisp, scrn.root, 1))
						__MFW_XCB_ITER_FUNC(xcb_randr_get_monitors_monitors, mon, xcb_randr_monitor_info, {
							MFW_MESSAGE("TODO get the actual monitor name")
							ucstring mon_name{};
							__display_api_funcs_internal::get_atom_string(xdisp, mon_data.name, mon_name);
							monitor &mon_info{monitors.emplace_back()};
							mon_info.name = mon_name;
							mon_info.width = mon_data.width;
							mon_info.height = mon_data.height;
							mon_info.x = mon_data.x;
							mon_info.y = mon_data.y;
							monitor_display_data_t &disp_data{mon_info.allocate_display_data<monitor_display_data_t>()};
							disp_data.scrn_info = &scrn_info;
							if(mon_data.primary) {
								monitor::mainmonitor = &mon_info;
							}
						})

						xcb_screen_next(&scrn_it);
					}
				}
			}
			closedir(x11dir);
			return !monitors.empty();
		} else {
			return false;
		}
	}

	bool display_api_funcs_xcb::init_windows()
	{
		return true;
	}

	void display_api_funcs_xcb::create_window(window &win, const monitor &monitor, size_t w, size_t h, ssize_t x, ssize_t y)
	{
		const monitor_display_data_t &mon_disp_data{monitor.get_display_data<monitor_display_data_t>()};
		const screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		window_display_data_t &win_disp_data{win.allocate_display_data<window_display_data_t>()};

		win_disp_data.id = xcb_generate_id(scrn_info.xdisp);
		uint32_t values[2]{
			scrn_info.white_pixel,
			XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_KEY_PRESS
		};
		xcb_create_window(scrn_info.xdisp, XCB_COPY_FROM_PARENT, win_disp_data.id, scrn_info.root, x, y, w, h, 10, XCB_WINDOW_CLASS_INPUT_OUTPUT, scrn_info.visual, XCB_CW_BACK_PIXEL|XCB_CW_EVENT_MASK, values);
	}

	void display_api_funcs_xcb::show_window(window &win, bool show)
	{
		const monitor &mon{*win.mon_};
		const monitor_display_data_t &mon_disp_data{mon.get_display_data<monitor_display_data_t>()};
		const screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};

		if(show) {
			xcb_map_window(scrn_info.xdisp, win_disp_data.id);
		} else {
			xcb_unmap_window(scrn_info.xdisp, win_disp_data.id);
		}
	}

	void display_api_funcs_xcb::flush(monitor &mon)
	{
		const monitor_display_data_t &mon_disp_data{mon.get_display_data<monitor_display_data_t>()};
		const screen_info_t &scrn_info{*mon_disp_data.scrn_info};

		xcb_flush(scrn_info.xdisp);
	}

	void display_api_funcs_xcb::destroy_window(window &win)
	{
		const monitor &mon{*win.mon_};
		const monitor_display_data_t &mon_disp_data{mon.get_display_data<monitor_display_data_t>()};
		const screen_info_t &scrn_info{*mon_disp_data.scrn_info};
		window_display_data_t &win_disp_data{win.get_display_data<window_display_data_t>()};

		xcb_destroy_window(scrn_info.xdisp, win_disp_data.id);
	}
}