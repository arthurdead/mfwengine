#include <private/mfw/ide/ide.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/library.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/debugging.hpp>

namespace mfw::ide
{
	MFW_DECLARE_GLOBAL_ALLOCATOR(ide, ide)
	MFW_DECLARE_LOG_CONTEXT(log_ide, u8"ide/ide"_p)

	ide &ide::instance() {
		return __ide_global_allocator.instance();
	}

	ide::ide()
		: core::interfaces::global_initializer{u8"ide"_sv, {u8"renderer"_sv}}
	{
		
	}

	ide::~ide()
	{

	}

	core::exit_status ide::initialize()
	{
		ucstring_view help{};
		bool valid{false};

		const core::commandline &cmdline{core::commandline::instance()};

		valid = cmdline.validate(help);
		if(!valid) {
			return core::exit_status::fatal;
		}

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		pstring exepath{core::executable_path()};
		exepath.remove_filename();

		filesys.add_searchpath({exepath, u8"executable"_sv});
		filesys.add_searchpath({exepath, u8"root"_sv});
		filesys.add_searchpath({u8"/run/media/arthurdead/New Volume/mfwengine/package/core/shaders"_p, u8"shaders"_sv});

		if(!renderer::interfaces::renderer::instance().initialize_render_api()) {
			return core::exit_status::fatal;
		}

		renderer::interfaces::renderer::instance().do_stuff();

		return core::exit_status::success;
	}
}