#include <private/mfw/renderer/render_api.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/environment.hpp>

//#define __MFW_VULKAN_BY_DEFAULT

#include <private/mfw/renderer/vulkan/render_api_funcs.hpp>
#include <private/mfw/renderer/opengl/render_api_funcs.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_render_api, u8"renderer/render_api"_p)

	namespace __render_api_internal
	{
		static render_api __render_api{render_api::unknown};
		static unique_ptr<interfaces::render_api_funcs> __render_funcs{};
	}

	interfaces::render_api_funcs &interfaces::render_api_funcs::instance() {
		return *__render_api_internal::__render_funcs;
	}

	render_api get_render_api()
	{
		return __render_api_internal::__render_api;
	}

	bool detect_render_api()
	{
		core::commandline &cmdline{core::commandline::instance()};

		const core::univalue *value{cmdline.value(u8"render_api"_s)};
		if(value) {
			if(*value == u8"directx") {
			#if MFW_OS_IS(LINUX)
				log_render_api().error(u8"cant use directx on linux"_sv);
			#elif MFW_OS_IS(WINDOWS)
				log_render_api().error(u8"directx not supported"_sv);
			#else
				#error
			#endif
			} else if(*value == u8"opengl") {
				__render_api_internal::__render_api = render_api::opengl;
			} else if(*value == u8"vulkan"_sv) {
				__render_api_internal::__render_api = render_api::vulkan;
			} else {
				log_render_api().error(u8"unknown render api {}"_sv, *value);
			}
		} else {
			MFW_MESSAGE("detect whats supported")
		#ifdef __MFW_VULKAN_BY_DEFAULT
			__render_api_internal::__render_api = render_api::vulkan;
		#else
			__render_api_internal::__render_api = render_api::opengl;
		#endif
		}

		switch(__render_api_internal::__render_api) {
			case render_api::vulkan: { __render_api_internal::__render_funcs.reset(new render_api_funcs_vulkan{}); break; }
			case render_api::opengl: { __render_api_internal::__render_funcs.reset(new render_api_funcs_opengl{}); break; }
		}

		return !!__render_api_internal::__render_funcs;
	}
}