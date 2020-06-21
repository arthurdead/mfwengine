#include <private/mfw/renderer/render_api.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/environment.hpp>

namespace mfw::renderer
{
	MFW_DECLARE_LOG_CONTEXT(log_render_api, u8"renderer/render_api"_p)

	namespace __render_api_internal
	{
		static render_api __render_api{render_api::unknown};
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
				__render_api_internal::__render_api = render_api::unknown;
			#elif MFW_OS_IS(WINDOWS)
				log_render_api().error(u8"directx not supported"_sv);
				__render_api_internal::__render_api = render_api::unknown;
			#else
				#error
			#endif
			} else if(*value == u8"opengl") {
				log_render_api().error(u8"opengl not supported"_sv);
				__render_api_internal::__render_api = render_api::unknown;
			} else if(*value == u8"vulkan"_sv) {
				__render_api_internal::__render_api = render_api::vulkan;
			} else {
				log_render_api().error(u8"unknown render api {}"_sv, *value);
				__render_api_internal::__render_api = render_api::unknown;
			}
		} else {
			__render_api_internal::__render_api = render_api::vulkan;
		}

		return __render_api_internal::__render_api != render_api::unknown;
	}
}