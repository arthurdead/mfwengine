#include <private/mfw/engine/engine.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/logging_interface.hpp>
//#include <public/mfw/renderer/renderer_interface.hpp>

namespace mfw::engine
{
	MFW_DECLARE_LOG_CONTEXT(log_engine, u8"engine"_p)
	MFW_DECLARE_GLOBAL_ALLOCATOR(engine, ::mfw::engine::engine)

	::mfw::engine::engine &::mfw::engine::engine::instance() {
		return __engine_global_allocator.instance();
	}
	MFW_ENGINE_API interfaces::engine & MFW_ENGINE_CALL interfaces::engine::instance()
		{ return ::mfw::engine::engine::instance(); }

	mfw::engine::engine::engine()
	{
		MFW_DEBUGBREAK();
	}

	core::exit_status mfw::engine::engine::initialize()
	{
		MFW_DEBUGBREAK();
		
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

	#if MFW_OS == MFW_OS_LINUX
		#define __MFW_OS_TARGET "linux"
	#elif MFW_OS == MFW_OS_WINDOWS
		#define __MFW_OS_TARGET "windows"
	#else
		#error
	#endif

	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		#define __MFW_CONFIGURATION_TARGET u8"debug"_p
	#else
		#define __MFW_CONFIGURATION_TARGET u8"release"_p
	#endif

	#if MFW_PROCESSOR == MFW_PROCESSOR_X86_64
		#define __MFW_PROCESSOR_TARGET u8"x86_64"_p
	#elif MFW_PROCESSOR == MFW_PROCESSOR_X86
		#define __MFW_PROCESSOR_TARGET u8"x86"_p
	#else
		#error
	#endif

		MFW_MESSAGE("get rid of the above")

		core::commandline &cmdline{core::commandline::instance()};

		//const univalue &game{cmdline.value(u"game"_s, u"test"_uv)};
		core::univalue game{u8"test"_sv};

		filesys.add_searchpath({game.get_string(), u8"root"_sv}, {{}, u8"executable"_sv});
		filesys.add_searchpath({u8"bin"_p / __MFW_OS_TARGET "_" __MFW_PROCESSOR_TARGET "_" __MFW_CONFIGURATION_TARGET, u8"bin"_sv}, {{}, u8"root"_sv});

		filesys.print_searchmap();

		core::library::add_directory({{}, u8"bin"_sv});

		/*if(!gameinfo.from_file({u"gameinfo.sr"_p, u"root"_sv})) {
			return core::exit_code::error;
		}*/

		/*gamelibrary = core::library::load_library(u8"game"_sv);
		if(!gamelibrary.valid()) {
			log_engine.error(u8"could not load game library"_sv);
			//return core::exit_code::error;
		}*/
		
		//renderer::interfaces::renderer::instance().do_stuff();

		return {};
	}

	core::exit_status mfw::engine::engine::shutdown()
	{
		return {};
	}

	core::exit_status mfw::engine::engine::update()
	{
		return {};
	}
}