#include <private/mfw/renderer/display_api.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/environment.hpp>

#if MFW_OS_IS(LINUX)
	#define __MFW_XCB_BY_DEFAULT

	#include <private/mfw/renderer/x11/xcb/display_api_funcs.hpp>
#endif

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_display_api, u8"renderer/display_api"_p)

	namespace __display_api_internal
	{
		static display_api __display_api{display_api::unknown};
		static unique_ptr<interfaces::display_api_funcs> __display_funcs{};
	}

	interfaces::display_api_funcs &interfaces::display_api_funcs::instance() {
		return *__display_api_internal::__display_funcs;
	}

	display_api get_display_api()
	{
		return __display_api_internal::__display_api;
	}

	bool detect_display_api()
	{
		core::commandline &cmdline{core::commandline::instance()};

		const core::univalue *value{cmdline.value(u8"display_api"_s)};
		if(value) {
			if(*value == u8"winapi") {
			#if MFW_OS_IS(LINUX)
				log_display_api().error(u8"cant use winapi on linux"_sv);
			#elif MFW_OS_IS(WINDOWS)
				log_display_api().error(u8"winapi not supported"_sv);
			#else
				#error
			#endif
			} else if(*value == u8"xlib"_sv) {
			#if MFW_OS_IS(WINDOWS)
				log_display_api().error(u8"cant use xlib on windows"_sv);
			#elif MFW_OS_IS(LINUX)
				log_display_api().error(u8"xlib not supported"_sv);
			#else
				#error
			#endif
			} else if(*value == u8"xcb"_sv) {
			#if MFW_OS_IS(WINDOWS)
				log_display_api().error(u8"cant use xcb on windows"_sv);
			#elif MFW_OS_IS(LINUX)
				__display_api_internal::__display_api = display_api::xcb;
			#else
				#error
			#endif
			} else if(*value == u8"wayland"_sv) {
			#if MFW_OS_IS(WINDOWS)
				log_display_api().error(u8"cant use wayland on windows"_sv);
			#elif MFW_OS_IS(LINUX)
				log_display_api().error(u8"wayland not supported"_sv);
			#else
				#error
			#endif
			} else {
				log_display_api().error(u8"unknown display api {}"_sv, *value);
			}
		} else {
		#if MFW_OS_IS(WINDOWS)
			log_display_api().error(u8"winapi not supported"_sv);
		#elif MFW_OS_IS(LINUX)
			core::environment_var var{u8"XDG_SESSION_TYPE"_sv};
			if(var.is_set()) {
				const ucstring &value{var.value()};
				if(value == u8"x11"_sv) {
				#ifdef __MFW_XCB_BY_DEFAULT
					__display_api_internal::__display_api = display_api::xcb;
				#else
					log_display_api().error(u8"xlib not supported"_sv);
				#endif
				} else if(value == u8"wayland"_sv) {
					log_display_api().error(u8"wayland not supported"_sv);
				} else {
					log_display_api().error(u8"unknown display api {}"_sv, value);
				}
			} else {
				var.reset(u8"WAYLAND_DISPLAY"_sv);
				if(var.is_set()) {
					log_display_api().error(u8"wayland not supported"_sv);
				} else {
					var.reset(u8"DISPLAY"_sv);
					if(var.is_set()) {
					#ifdef __MFW_XCB_BY_DEFAULT
						__display_api_internal::__display_api = display_api::xcb;
					#else
						log_display_api().error(u8"xlib not supported"_sv);
					#endif
					} else {
						log_display_api().error(u8"unknown display api"_sv);
					}
				}
			}
		#else
			#error
		#endif
		}

		switch(__display_api_internal::__display_api) {
		#if MFW_OS_IS(LINUX)
			case display_api::xcb: { __display_api_internal::__display_funcs.reset(new display_api_funcs_xcb{}); break; }
		#endif
		}

		return !!__display_api_internal::__display_funcs;
	}
}