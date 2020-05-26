#include <private/mfw/builder/references/option_reference.hpp>
#include <private/mfw/builder/references/file_reference.hpp>

namespace mfw::builder
{
	void option_reference::merge_child(size_t depth, serializable &child, const serializable &other) const
	{
		/*
		option &child_f{reinterpret_cast<option &>(child)};
		const option &other_f{reinterpret_cast<const option &>(other)};
		child_f.mapped_name_ = other_f.mapped_name_;
		*/
	}

	core::serializable *option_reference::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		if(depth == 0) {
			const core::serializable *flags{get_flags()};
			if(flags) {
				if(flags->has_child(u8"folders")) {
					return new file_reference{};
				}
			}
		}
		return new core::serializable{};
	}
}