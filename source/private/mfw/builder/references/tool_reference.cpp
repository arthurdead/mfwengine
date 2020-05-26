#include <private/mfw/builder/references/tool_reference.hpp>

namespace mfw::builder
{
	core::serializable *tool_reference::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		return new core::serializable{};
	}
	
	ucstring tool_reference::shell() const
	{
		const core::serializable *file{get_child(u8"shell"_sv)};
		if(!file) {
			return {};
		}

		return file->get_value().get_string();
	}

	pstring tool_reference::path() const
	{
		const core::serializable *file{get_child(u8"path"_sv)};
		if(!file) {
			return {};
		}

		return as_string<pstring>(file->get_value());
	}

	pstring tool_reference::unity_build_ext() const
	{
		const core::serializable *ext{get_child(u8"unity_build_ext"_sv)};
		if(!ext) {
			return {};
		}

		return as_string<pstring>(ext->get_value());
	}

	bool tool_reference::single_input() const
	{
		return get_child_bool(u8"single_input"_sv);
	}

	const core::serializable *tool_reference::output_args() const
	{
		return get_child(u8"output"_sv);
	}

	pstring tool_reference::output_default_path() const
	{
		const core::serializable *output{get_child(u8"output"_sv)};
		if(output) {
			return as_string<pstring>(output->get_value());
		}

		return {};
	}
}