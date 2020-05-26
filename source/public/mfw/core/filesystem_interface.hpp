#ifndef __MFW_PUBLIC_CORE_FILESYSTEM_INTERFACE_H
#define __MFW_PUBLIC_CORE_FILESYSTEM_INTERFACE_H

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/core/file_interface.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/vector.hpp>

namespace mfw::core
{
	enum class open_flags : uchar_t
	{
		read = MFW_BIT(0),
		write = MFW_BIT(1),
		truncate = MFW_BIT(2),
		create = MFW_BIT(3),

		readwrite = (read|write),
		replace = (create|truncate),
		all = (readwrite|replace),
	};
	MFW_ENUM_FLAGS(open_flags)

	namespace interfaces
	{
		class filesystem
		{
		protected:
			virtual ~filesystem() = default;

		public:
			MFW_CORE_API static filesystem & MFW_CORE_CALL instance();

			virtual bool add_searchpath(const searchpath &search, const searchpath &relative = {}) = 0;
			virtual bool remove_searchpath(const searchpath &search) = 0;

			virtual bool set_working_dir(const searchpath &search) const = 0;
			virtual pstring get_working_dir() const = 0;

			virtual pstring resolve(const searchpath &search, bool exists=true) const = 0;
			virtual bool resolve(const searchpath &search, vector<pstring> &paths, bool exists = true) const = 0;
			virtual const vector<pstring> *get_paths(const ucstring &name) const = 0;
			virtual pstring clean(const searchpath &search) const = 0;

			virtual uint64_t get_file_modified_time(const searchpath &search) const = 0;

			virtual bool glob(const searchpath &search, vector<pstring> &files) const = 0;
			virtual bool matches_glob(const searchpath &search, const pstring &pattern) const = 0;

			virtual bool create_symlink(const searchpath &from, const searchpath &to) const = 0;
			virtual bool remove(const searchpath &search) const = 0;
			virtual bool exists(const searchpath &search) const = 0;
			virtual bool is_directory(const searchpath &search) const = 0;
			virtual bool create_directories(const searchpath &search) const = 0;
			virtual file *open_file(const searchpath &search, open_flags flags) const = 0;

			virtual bool open_text_file(const searchpath &search, ucstring &str) const = 0;
			virtual bool save_text_file(const searchpath &search, const ucstring &str) const = 0;

			virtual void print_searchmap() const = 0;

			virtual bool initialize(const pstring &exepath) = 0;
		};
	}
}

#endif