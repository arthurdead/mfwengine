#ifndef __MFW_PRIVATE_BUILDER_REFERENCES_OPTION_REFERENCE_H
#define __MFW_PRIVATE_BUILDER_REFERENCES_OPTION_REFERENCE_H

#pragma once

#include <public/mfw/core/serializable.hpp>

namespace mfw::builder
{
	class option_reference final : public core::serializable
	{
	public:
		//const ucstring &mapped_name() const { return mapped_name_; }

	private:
		friend class builder;

		core::serializable *allocate_child(ssize_t depth, const ucstring_view &name, const core::serializable *parent) const override;
		void merge_child(size_t depth, serializable &child, const serializable &other) const override;

		//ucstring mapped_name_{};
	};
}

#endif