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
	
		bool build_for_file(const core::searchpath &search, const core::searchpath &output_dir);

		bool add_include_dir(const core::searchpath &search);

		void clear_files() { files.clear(); }
		void clear_include_dirs() { include_dirs.clear(); extra_dirs = nullptr; }
		void clear() { clear_files(); clear_include_dirs(); }

		bool timestamps_changed(core::serializable &timestamps, const ucstring &name, const core::searchpath &output_dir);

	private:
		bool timestamp_changed(core::serializable &timestamp, const core::searchpath &output_dir);
		bool timestamps_changed(core::serializable &timestamps, const core::searchpath &output_dir);
		
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

		struct file_data_t //: use_allocator<file_data_t>
		{
			using list_type = vector<file_data_t *>;
			
			list_type depends_on_{};
			list_type depended_by_{};
			uint64_t modified{0};
			pstring filepath{};

			void write_to(core::serializable &out);

			void depends_on(file_data_t &other, bool recurse=true, bool do_by=true);
			void depended_by(file_data_t &other);
		};

		file_data_t *find_or_create_data(const pstring &filepath);

		const includes_dirs_t *extra_dirs{nullptr};
		includes_dirs_t include_dirs{};
		
		using list_type = ptr_vector<file_data_t>;
		list_type files{};

		class include_collector final
		{
		public:
			bool collect_includes(file_data_t *&data, file_timestamp_builder &stampbuilder);

			bool found_include(file_data_t *&data, file_timestamp_builder &stampbuilder, const ucstring &value, bool local);

		private:
			bool read_value(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &value, bool &local);
			bool read_name(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &name);
			bool skip_space(ucstring::const_iterator &it, ucstring::const_iterator end);
		};
	};
}

#endif