#ifndef __MFW_PRIVATE_BUILDER_REFERENCES_BUILDER_SECTION_REFERENCE_HPP
#define __MFW_PRIVATE_BUILDER_REFERENCES_BUILDER_SECTION_REFERENCE_HPP

#pragma once

#include <public/mfw/core/serializable.hpp>

namespace mfw::builder
{
	class builder_section_reference final : public core::serializable
	{
	public:
		const core::serializable *macros() const { return get_child(u8"macros"_sv); }
		
	private:
		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;
	};
}

#endif