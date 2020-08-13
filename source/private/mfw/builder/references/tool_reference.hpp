#ifndef MFW_PRIVATE_BUILDER_REFERENCES_TOOL_REFERENCE_HPP
#define MFW_PRIVATE_BUILDER_REFERENCES_TOOL_REFERENCE_HPP

#pragma once

#include <private/mfw/builder/base_cached_file.hpp>
#include <public/mfw/core/serializable.hpp>
#include <private/mfw/builder/references/builder_section_reference.hpp>

namespace mfw::builder
{
	class output_tool_reference;

	class tool_reference final : public base_cached_file
	{
	public:
		bool single_input() const;
		pstring unity_build_ext() const;
		bool is_shell() const { return has_child(u8"shell"_sv); }
		pstring path() const;
		ucstring shell() const;
		const core::serializable *options_map() const { return get_child(u8"options_map"_sv); }
		const builder_section_reference *builder_section() const
		{ return reinterpret_cast<const builder_section_reference *>(get_child(u8"builder"_sv)); }
		const core::serializable *output_args() const;
		pstring output_default_path() const;

	private:
		friend class builder;

		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;
	};
}

#endif