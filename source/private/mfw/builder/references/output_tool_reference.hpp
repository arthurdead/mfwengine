#ifndef __MFW_PRIVATE_BUILDER_REFERENCES_OUTPUT_TOOL_REFERENCE_HPP
#define __MFW_PRIVATE_BUILDER_REFERENCES_OUTPUT_TOOL_REFERENCE_HPP

#pragma once

#include <public/mfw/core/serializable.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>
#include <private/mfw/builder/references/tool_section_reference.hpp>

namespace mfw::builder
{
	class output_tool_reference final : public core::serializable
	{
	public:
		const tool_reference *tool() const { return tool_; }
		const tool_section_reference *section() const { return section_; }

	private:
		friend class builder;

		const tool_reference *tool_{nullptr};
		tool_section_reference *section_{nullptr};
	};
}

#endif