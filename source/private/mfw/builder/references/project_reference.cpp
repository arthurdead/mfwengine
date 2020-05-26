#include <private/mfw/builder/references/project_reference.hpp>
#include <private/mfw/builder/references/tool_section_reference.hpp>
#include <private/mfw/builder/references/builder_section_reference.hpp>

namespace mfw::builder
{
	core::serializable *project_reference::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		if(depth == 0) {
			if(name_ == u8"builder"_sv) {
				return new builder_section_reference{};
			} else {
				return new tool_section_reference{};
			}
		}
		return new core::serializable{};
	}
}