#ifndef MFW_PRIVATE_BUILDER_REFERENCES_SOLUTION_REFERENCE_HPP
#define MFW_PRIVATE_BUILDER_REFERENCES_SOLUTION_REFERENCE_HPP

#pragma once

#include <private/mfw/builder/base_cached_file.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/serializable.hpp>
//#include <private/mfw/builder/references/project_reference.hpp>
#include <private/mfw/builder/references/builder_section_reference.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>

namespace mfw::builder
{
	class project_reference;

	class solution_reference final : public base_cached_file
	{
	public:
		const core::serializable *build_set() const { return get_child(u8"build_set"_sv); }

		const builder_section_reference *builder_section() const
		{ return reinterpret_cast<const builder_section_reference *>(get_child(u8"builder"_sv)); }
		const core::serializable *plugin_section() const
		{ return get_child(u8"plugin"_sv); }

		const ptr_vector<project_reference> &projects() const { return projects_; }

	private:
		friend class builder;

		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;

		ptr_vector<project_reference> projects_{};

		struct tool_info_t
		{
			const tool_reference *tool{nullptr};
			ucstring condition{};
			ucstring name{};
		};

		ptr_vector<tool_info_t> tools_{};
	};
}

#endif