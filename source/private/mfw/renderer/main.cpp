#include <public/mfw/core/application.hpp>
#include <public/mfw/core/commandline.hpp>
#include <private/mfw/renderer/renderer.hpp>
//#include <private/mfw/renderer/vulkan/renderer.hpp>

/*
#if MFW_COMPILER == MFW_COMPILER_MSVC
	#pragma warning(push)
	#pragma warning(disable: 4464)
#endif
#include <glslang/OGLCompilersDLL/InitializeDll.h>
#if MFW_COMPILER == MFW_COMPILER_MSVC
	#pragma warning(pop)
#endif
*/

namespace mfw::renderer::agnostic
{
	class factory
	{
	public:
		factory()
		{
			core::commandline &cmdline{core::commandline::instance()};

			//const core::univalue &value{cmdline.value(u8"renderer"_sv, u8"vulkan"_sv)};
			//if(value == u"vulkan"_sv) {
			//	renderer.reset(new vulkan::renderer{});
			//} else {
			//	renderer.reset(new agnostic::renderer{});
			//}
			renderer.reset(new agnostic::renderer{});
		}

		unique_ptr<agnostic::renderer> renderer{};
	};

	MFW_DECLARE_GLOBAL_ALLOCATOR(factory, factory)

	agnostic::renderer &agnostic::renderer::instance()
	{
		return *__factory_global_allocator.instance().renderer;
	}
}

mfw::core::exit_status application_main(
#if MFW_OS == MFW_OS_WINDOWS
bool thread
#endif
)
{
	mfw::core::exit_status status{};

	/*if(!thread) {
		if(!glslang::InitProcess()) {
			code = mfw::core::exit_code::error;
		}
	} else {
		if(!glslang::InitThread()) {
			code = mfw::core::exit_code::error;
		}
	}*/

	return status;
}

mfw::core::exit_status application_exit(
#if MFW_OS == MFW_OS_WINDOWS
bool thread
#endif
)
{
	mfw::core::exit_status status{};

	/*if(!thread) {
		if(!glslang::DetachProcess()) {
			code = mfw::core::exit_code::error;
		}
	} else {
		if(!glslang::DetachThread()) {
			code = mfw::core::exit_code::error;
		}
	}*/

	return status;
}