#include <private/mfw/builder/references/tool_section_reference.hpp>
#include <private/mfw/builder/references/file_reference.hpp>
#include <private/mfw/builder/references/option_reference.hpp>
#include <private/mfw/builder/references/output_tool_reference.hpp>

namespace mfw::builder
{
	void tool_section_reference::add_file(const pstring &path, file_reference::flags file_flags, const pstring &filter, const ucstring &cond)
	{
		core::serializable &files{child(u8"files"_sv)};
		core::serializable::iterator it{files.begin()};
		bool found{false};
		while(it != files.end()) {
			const file_reference &file_main{reinterpret_cast<const file_reference &>(*it)};
			if(file_main.path() == path) {
				found = true;
				break;
			}
			it++;
		}
		file_reference *file{nullptr};
		if(!found) {
			file = &reinterpret_cast<file_reference &>(files.child(as_string<ucstring>(path)));
		} else {
			file = &reinterpret_cast<file_reference &>(*it);
		}
		if(!filter.empty()) {
			file->set_value(as_string<core::univalue>(filter));
		}
		file->flags_ |= file_flags|file_reference::flags::added_by_tool;
		ucstring tmp{};
		core::append_expression(file->get_condition(), cond, tmp);
		file->set_condition(tmp);
	}

	core::serializable *tool_section_reference::allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const
	{
		if(depth == 0) {
			if(name_ == u8"output_tool"_sv) {
				return new output_tool_reference{};
			}
		} else if(depth == 1) {
			const ucstring &parent_name{parent->get_name()};
			if(parent_name == u8"files"_sv) {
				return new file_reference{};
			} else if(parent_name == u8"options"_sv) {
				return new option_reference{};
			}
		}
		return new core::serializable{};
	}
}