#ifndef __MFW_PRIVATE_BUILDER_REFERENCES_TOOL_SECTION_REFERENCE_HPP
#define __MFW_PRIVATE_BUILDER_REFERENCES_TOOL_SECTION_REFERENCE_HPP

#pragma once

#include <public/mfw/core/serializable.hpp>
#include <private/mfw/builder/references/project_reference.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>
#include <private/mfw/builder/references/builder_section_reference.hpp>
#include <private/mfw/builder/references/file_reference.hpp>

namespace mfw::builder
{
	class tool_section_reference final : public core::serializable
	{
	public:
		const tool_reference *tool() const { return tool_; }
		const output_tool_reference *output_tool() const
		{ return reinterpret_cast<const output_tool_reference *>(get_child(u8"output_tool"_sv)); }

		const core::serializable &options() const;

		const builder_section_reference *builder_section() const
		{ return reinterpret_cast<const builder_section_reference *>(get_child(u8"builder"_sv)); }
		const core::serializable *plugin_section() const
		{ return get_child(u8"plugin"_sv); }

		bool out_of_date() const { return out_of_date_; }
		bool unity_build() const { return unity_build_; }

		void add_file(const pstring &path, file_reference::flags flags, const pstring &filter, const ucstring &cond);
		void add_file(const pstring &path, file_reference::flags flags, const pstring &filter, const ucstring &cond) const
		{ const_cast<tool_section_reference *>(this)->add_file(path, flags, filter, cond); }

	private:
		friend class builder;

		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;

		const tool_reference *tool_{nullptr};
		bool out_of_date_{true};
		bool unity_build_{false};
	};
}

#endif