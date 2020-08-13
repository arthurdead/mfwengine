#include <public/mfw/core/searchpath.hpp>

namespace mfw::core
{
	MFW_VISIBILITY_LOCAL_PUSH()

	searchpath::searchpath(stl::pchar_t *ptr, stl::size_t len) noexcept
		: m_path{ptr, len} {}

	searchpath_view::searchpath_view(stl::pchar_t *ptr, stl::size_t len) noexcept
		: m_path{ptr, len} {}

	searchpath::searchpath(stl::pstring_view path) noexcept
		: m_path{path} {}
	searchpath::searchpath(stl::pstring &&path) noexcept
		: m_path{stl::move(path)} {}
	searchpath::searchpath(stl::osstring_view name) noexcept
	{ m_path.assign(name); }
	searchpath::searchpath(stl::osstring &&name) noexcept
	{ m_path.assign(stl::move(name)); }

	searchpath::searchpath(stl::pstring_view path, stl::osstring_view name) noexcept
		: m_path{path}, m_name{name} {}
	searchpath::searchpath(stl::pstring_view path, stl::osstring &&name) noexcept
		: m_path{path}, m_name{stl::move(name)} {}
	searchpath::searchpath(stl::pstring &&path, stl::osstring_view name) noexcept
		: m_path{stl::move(path)}, m_name{name} {}
	searchpath::searchpath(stl::pstring &&path, stl::osstring &&name) noexcept
		: m_path{stl::move(path)}, m_name{stl::move(name)} {}

	searchpath_view::searchpath_view(stl::pstring_view path) noexcept
		: m_path{path} {}
	searchpath_view::searchpath_view(stl::osstring_view name) noexcept
	{ m_path.assign(name); }

	searchpath_view::searchpath_view(stl::pstring_view path, stl::osstring_view name) noexcept
		: m_path{path}, m_name{name} {}

	bool searchpath::empty() const noexcept
	{ return (m_path.empty() && m_name.empty()); }

	bool searchpath_view::empty() const noexcept
	{ return (m_path.empty() && m_name.empty()); }

	searchpath_view::searchpath_view(const searchpath &other) noexcept
		: m_path{other.m_path}, m_name{other.m_name} {}
	searchpath_view &searchpath_view::operator=(const searchpath &other) noexcept
	{ m_path.assign(other.m_path); m_name.assign(other.m_name); }

	searchpath::searchpath(const searchpath_view &other) noexcept
		: m_path{other.m_path}, m_name{other.m_name} {}
	searchpath &searchpath::operator=(const searchpath_view &other) noexcept
	{ m_path.assign(other.m_path); m_name.assign(other.m_name); }

	searchpath::operator searchpath_view() const noexcept
	{ return searchpath_view{m_path, m_name}; }

	searchpath &searchpath::clear() noexcept
	{ m_path.clear(); m_name.clear(); }

	MFW_VISIBILITY_LOCAL_POP()
}

MFW_VISIBILITY_LOCAL_PUSH()

::mfw::core::searchpath operator""_sp(const ::mfw::stl::pchar_t *ptr, ::mfw::stl::size_t len) noexcept
{ return ::mfw::core::searchpath{ptr, len}; }

::mfw::core::searchpath_view operator""_spv(const ::mfw::stl::pchar_t *ptr, ::mfw::stl::size_t len) noexcept
{ return ::mfw::core::searchpath_view{ptr, len}; }

MFW_VISIBILITY_LOCAL_POP()