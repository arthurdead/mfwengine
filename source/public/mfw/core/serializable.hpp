#ifndef MFW_PUBLIC_CORE_SERIALIZABLE_HPP
#define MFW_PUBLIC_CORE_SERIALIZABLE_HPP

#pragma once

#include <public/mfw/stl/memory.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/univalue.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/optional.hpp>

#include <public/mfw/core/internal/serializable.hpp>

namespace mfw::core
{
	class MFW_VISIBILITY_PUBLIC serializable_parser_callbacks
	{
	protected:
		MFW_VISIBILITY_LOCAL virtual ~serializable_parser_callbacks() noexcept = default;
		
	public:
		using string_view_type = stl::osstring_view;

		using const_optional_child = stl::optional<stl::reference_wrapper<const serializable>>;
		using include_dirs_t = stl::vector<stl::pstring>;

		MFW_VISIBILITY_LOCAL virtual const_optional_child get_inherit(string_view_type name) const noexcept;
		MFW_VISIBILITY_LOCAL const include_dirs_t &include_dirs() const noexcept;

	protected:
		include_dirs_t m_include_dirs{};
	};

	using serializable_view = type_view_t<serializable>;
}

#endif