#include <private/mfw/builder/base_plugin.hpp>
#include <public/mfw/core/filesystem_interface.hpp>

namespace mfw::builder
{
	base_plugin::base_plugin(const ucstring &name)
		: name_{name}
	{
		log_.set_name(name_);
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		filesys.add_searchpath({name_, name_}, {{}, u8"data"_sv});
	}
}