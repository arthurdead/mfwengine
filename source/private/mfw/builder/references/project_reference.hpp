#ifndef __MFW_PRIVATE_BUILDER_REFERENCES_PROJECT_REFERENCE_H
#define __MFW_PRIVATE_BUILDER_REFERENCES_PROJECT_REFERENCE_H

#pragma once

#include <private/mfw/builder/base_cached_file.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/serializable.hpp>
//#include <private/mfw/builder/references/solution_reference.hpp>
#include <private/mfw/builder/references/builder_section_reference.hpp>

namespace mfw::builder
{
	class solution_reference;

	class project_reference final : public base_cached_file
	{
	public:
		pstring filter() const { return as_string<pstring>(get_value()); }

		const builder_section_reference *builder_section() const
		{ return reinterpret_cast<const builder_section_reference *>(get_child(u8"builder"_sv)); }
		const core::serializable *plugin_section() const
		{ return get_child(u8"plugin"_sv); }

	private:
		friend class builder;

		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;

		vector<project_reference *> depends{};
	};
}

#endif