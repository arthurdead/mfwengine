#ifndef __MFW_PRIVATE_BUILDER_BASE_CACHED_FILE_HPP
#define __MFW_PRIVATE_BUILDER_BASE_CACHED_FILE_HPP

#pragma once

#include <public/mfw/core/serializable.hpp>

namespace mfw::builder
{
	class base_cached_file : public core::serializable
	{
	public:
		bool loaded_from_cache() const { return loaded_from_cache_; }

	private:
		friend class builder;
	
		bool loaded_from_cache_{false};
	};
}

#endif