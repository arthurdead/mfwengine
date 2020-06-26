#include <private/mfw/renderer/opengl/context_api.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/renderer/display_api.hpp>

#define __MFW_EGL_BY_DEFAULT

#include <private/mfw/renderer/opengl/egl/context_api_funcs.hpp>
#if MFW_OS_IS(LINUX)
	#include <private/mfw/renderer/opengl/xcb_glx/context_api_funcs.hpp>
#endif

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_context_api, u8"renderer/render_api/opengl/context_api"_p)

	namespace __context_api_internal
	{
		static context_api __context_api{context_api::unknown};
		static unique_ptr<interfaces::context_api_funcs> __context_funcs{};
	}

	interfaces::context_api_funcs &interfaces::context_api_funcs::instance() {
		return *__context_api_internal::__context_funcs;
	}

	context_api get_context_api()
	{
		return __context_api_internal::__context_api;
	}

	bool detect_context_api()
	{
		core::commandline &cmdline{core::commandline::instance()};

		bool xcb{get_display_api() == display_api::xcb};

		const core::univalue *value{cmdline.value(u8"context_api"_s)};
		if(value) {
			if(*value == u8"wgl") {
			#if MFW_OS_IS(LINUX)
				log_context_api().error(u8"cant use wgl on linux"_sv);
			#elif MFW_OS_IS(WINDOWS)
				log_context_api().error(u8"wgl not supported"_sv);
			#else
				#error
			#endif
			} else if(*value == u8"egl") {
				if(xcb) {
					log_context_api().error(u8"egl doenst support xcb"_sv);
				} else {
					__context_api_internal::__context_api = context_api::egl;
				}
			} else if(*value == u8"glx"_sv) {
			#if MFW_OS_IS(LINUX)
				if(xcb) {
					__context_api_internal::__context_api = context_api::glx;
				} else {
					log_context_api().error(u8"glx only supported on xcb"_sv);
				}
			#elif MFW_OS_IS(WINDOWS)
				log_context_api().error(u8"cant use glx on windows"_sv);
			#else
				#error
			#endif
			} else {
				log_context_api().error(u8"unknown context api {}"_sv, *value);
			}
		} else {
			MFW_MESSAGE("detect whats supported")
		#ifdef __MFW_EGL_BY_DEFAULT
			if(xcb) {
				log_context_api().error(u8"egl doenst support xcb switching to glx instead"_sv);
				__context_api_internal::__context_api = context_api::glx;
			} else {
				__context_api_internal::__context_api = context_api::egl;
			}
		#else
			#if MFW_OS_IS(LINUX)
			if(xcb) {
				__context_api_internal::__context_api = context_api::glx;
			} else {
				log_context_api().error(u8"glx only supported on xcb"_sv);
			}
			#elif MFW_OS_IS(WINDOWS)
			log_context_api().error(u8"wgl not supported"_sv);
			#else
				#error
			#endif
		#endif
		}

		switch(__context_api_internal::__context_api) {
			case context_api::egl: { __context_api_internal::__context_funcs.reset(new context_api_funcs_egl{}); break; }
		#if MFW_OS_IS(LINUX)
			case context_api::glx: {
				if(xcb) {
					__context_api_internal::__context_api = context_api::xcb_glx;
					log_context_api().info(u8"using xcb_glx instead of glx"_sv);
					__context_api_internal::__context_funcs.reset(new context_api_funcs_xcb_glx{});
				}
				break;
			}
		#endif
		}

		return !!__context_api_internal::__context_funcs;
	}
}