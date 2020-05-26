#include <private/mfw/builder/references/file_reference.hpp>
#include <private/mfw/builder/references/option_reference.hpp>

namespace mfw::builder
{
	core::serializable *file_reference::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		if(depth == 0) {
			if(get_value() != u8"folder"_sv) {
				return new option_reference{};
			}
			return new file_reference{};
		} else if(depth == 2) {
			return new option_reference{};
		}
		return new core::serializable{};
	}
}