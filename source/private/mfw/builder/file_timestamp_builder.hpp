#ifndef __MFW_PRIVATE_BUILDER_FILE_TIMESTAMP_BUILDER_H
#define __MFW_PRIVATE_BUILDER_FILE_TIMESTAMP_BUILDER_H

#pragma once

#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stack.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/list.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/serializable.hpp>

namespace mfw::builder
{
	class file_timestamp_builder final
	{
	public:
		using includes_dirs_t = vector<pstring>;

		bool generate(const core::searchpath &path, const core::searchpath &output);
		bool check(const core::searchpath &output);

		bool add_include_dir(const core::searchpath &search);

	private:
		static bool is_cpp_source(const pstring &ext);
		static bool is_cpp_header(const pstring &ext);
		static bool is_cpp_file(const pstring &ext) {
			return (is_cpp_source(ext) || is_cpp_header(ext));
		}
		static bool is_serializable_file(const pstring &ext) {
			return (ext == u8".sr"_p);
		}
		static bool file_supports_includes(const pstring &ext) {
			return (is_serializable_file(ext) || is_cpp_file(ext));
		}
		
		void clear() {
			stamps.clear();
		}
		
		includes_dirs_t include_dirs{};
		struct stamp_data_t
		{
			pstring path{};
			uint64_t time{};
		};
		using stamps_t = vector<stamp_data_t>;
		stamps_t stamps{};
		
		bool was_already_processed(const pstring &path) const;
		
		bool generate_base(const pstring &path);
		bool generate_from_include(const pstring &path);

		bool collect_includes(const pstring &path);
		
		bool found_include(const pstring &path, const ucstring &value, bool local);
		
		static bool read_value(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &value, bool &local);
		static bool read_name(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &name);
		static bool skip_space(ucstring::const_iterator &it, ucstring::const_iterator end);
	};
}

#endif