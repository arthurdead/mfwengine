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

		searchpath(const ucstring &name) {
			to_string(name, path);
		}

		searchpath(const ucstring_view &name) {
			to_string(name, path);
		}
					
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
		#ifdef __MFW_STD_FILESYSTEM_WIDE_CHAR
			#error
		#else
			const char *c_ptr{c_str(ptr)};
		#endif
			pstring path{c_ptr, c_ptr+len};
			return searchpath{move(path)};
		}
	}
}

#endif