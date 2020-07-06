#ifndef __MFW_PUBLIC_CORE_SEARCHPATH_HPP
#define __MFW_PUBLIC_CORE_SEARCHPATH_HPP

#pragma once

#include <public/mfw/stl/string.hpp>

namespace mfw::core
{
	struct SearchPath final
	{
		SearchPath() noexcept = default;
		SearchPath(const SearchPath &) noexcept = default;
		SearchPath(SearchPath &&) noexcept = default;

		SearchPath(const stl::pstring &path) noexcept
			: m_path{path} {}
		SearchPath(const stl::osstring &name) noexcept
		{ m_path.assign(name); }
		SearchPath(stl::osstring_view name) noexcept {
			const stl::oschar_t *ptr{name.data()};
			m_path.assign(ptr, ptr+name.length());
		}
					
		SearchPath(const stl::pstring &path, stl::osstring_view name) noexcept
			: m_path{path}, m_name{name} {}

		bool empty() const noexcept
		{ return (m_path.empty() && m_name.empty()); }

		stl::pstring m_path{};
		stl::osstring m_name{};
	};
}

::mfw::core::SearchPath operator""_sp(const ::mfw::stl::pchar_t *ptr, ::mfw::stl::size_t len) noexcept;

#endif