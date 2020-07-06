#ifndef __MFW_PRIVATE_BUILDER_REFERENCES_FILE_REFERENCE_HPP
#define __MFW_PRIVATE_BUILDER_REFERENCES_FILE_REFERENCE_HPP

#pragma once

#include <private/mfw/builder/base_cached_file.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/serializable.hpp>
//#include <private/mfw/builder/references/tool_section_reference.hpp>

namespace mfw::builder
{
	class tool_section_reference;

	class file_reference final : public core::serializable
	{
	public:
		enum class flags : uchar_t
		{
			none = 0,
			added_by_tool = MFW_BIT(1),
			out_of_date = MFW_BIT(2),
		};
		MFW_CLASS_ENUM_FLAGS(flags)

		pstring filter() const { return as_string<pstring>(get_value()); }
		pstring path() const { return as_string<pstring>(get_name()); }
		flags get_flags() const { return flags_; }

	private:
		friend class builder;
		friend class tool_section_reference;

		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;

		flags flags_{flags::none};
	};
}

#endif