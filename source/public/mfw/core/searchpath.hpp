#ifndef __MFW_PUBLIC_CORE_SEARCHPATH_H
#define __MFW_PUBLIC_CORE_SEARCHPATH_H

#pragma once

#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/filesystem.hpp>

namespace mfw::core
{
	struct searchpath
	{
		searchpath() = default;

		searchpath(const pstring &path_)
			: path{path_} {}

		searchpath(const ucstring &name)
			: path{name} {}

		searchpath(const ucstring_view &name)
			: path{name} {}
					
		searchpath(const pstring &path_, const ucstring_view &name)
			: path{path_}, name_{name} {}

		bool empty() const { return (name_.empty() && path.empty()); }

		const ucstring &name() const { return name_; }
		const pstring &dir() const { return path; }

		pstring path{};
		ucstring name_{};
	};

	namespace literals
	{
		inline searchpath operator""_sp(const upchar_t *ptr, size_t len)
		{
			pstring path{ptr, ptr+len};
			return searchpath{move(path)};
		}
	}
}

#endif