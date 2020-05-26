#include <private/mfw/builder/references/builder_section_reference.hpp>
#include <private/mfw/builder/references/tool_section_reference.hpp>

namespace mfw::builder
{
	core::serializable *builder_section_reference::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		return new core::serializable{};
	}
}