#include <private/mfw/builder/file_timestamp_builder.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/logging_interface.hpp>

namespace mfw::builder
{
	MFW_DECLARE_LOG_CONTEXT(log_filestamp, u8"builder/file_timestamp_builder"_p)
	
	bool file_timestamp_builder::add_include_dir(const core::searchpath &search)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		vector<pstring> resolved{};
		filesys.resolve(search, resolved);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &dir : resolved) {
			if(!contains(include_dirs, dir)) {
				include_dirs.emplace_back(dir);
			}
		}

		return true;
	}
	
	bool file_timestamp_builder::generate(const core::searchpath &input, const core::searchpath &output)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		pstring path{filesys.resolve(input, true)};
		if(path.empty()) {
			return false;
		}
		
		pstring ext{path.extension()};
		if(file_supports_includes(ext)) {
			if(!generate_from_include(path)) {
				return false;
			}
		} else {
			if(!generate_base(path)) {
				return false;
			}
		}
		
		core::interfaces::file *file{filesys.open_file(output, core::open_flags::all)};
		
		size_t len{stamps.size()};
		file->write(&len, 1, sizeof(size_t));
		for(const stamp_data_t &data : stamps) {
			len = data.path.native().length()+1;
			file->write(&len, 1, sizeof(size_t));
			file->write(c_str(data.path), len, sizeof(pchar_t));
			file->write(&data.time, 1, sizeof(uint64_t));
		}
		
		delete file;
		
		clear();
		
		return true;
	}
	
	bool file_timestamp_builder::check(const core::searchpath &output) const
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		core::interfaces::file *file{filesys.open_file(output, core::open_flags::readwrite)};
		if(!file) {
			return true;
		}
		
		bool changed{false};
		
		size_t siz{0};
		file->read(&siz, 1, sizeof(size_t));
		for(size_t i{0}; i < siz; i++) {
			size_t len{0};
			file->read(&len, 1, sizeof(size_t));
			pchar_t *name{new pchar_t[len]{u8'\0'}};
			file->read(name, len, sizeof(pchar_t));
			pstring path{name};
			delete[] name;
			uint64_t oldtime{0};
			file->read(&oldtime, 1, sizeof(uint64_t));
			size_t newtime{filesys.get_file_modified_time({path})};
			if(oldtime != newtime) {
				file->seek(-sizeof(uint64_t), core::seek::current);
				file->write(&newtime, 1, sizeof(uint64_t));
				changed = true;
				break;
			}
		}
		
		delete file;
		
		return changed;
	}
	
	bool file_timestamp_builder::generate_base(const pstring &path)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		stamp_data_t &data{stamps.emplace_back()};
		data.path = path;
		data.time = filesys.get_file_modified_time({path});
		
		return true;
	}
	
	bool file_timestamp_builder::was_already_processed(const pstring &path) const
	{
		for(const stamp_data_t &data : stamps) {
			if(data.path == path) {
				return true;
			}
		}
		
		return false;
	}
	
	bool file_timestamp_builder::generate_from_include(const pstring &path)
	{
		if(was_already_processed(path)) {
			return true;
		}
		
		if(!generate_base(path)) {
			return false;
		}
		
		if(!collect_includes(path)) {
			return false;
		}
		
		return true;
	}

	bool file_timestamp_builder::found_include(const pstring &path, const ucstring &value, bool local)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		pstring includepath{as_string<pstring>(value)};

		bool found{false};
		if(local) {
			pstring tmp{path};
			tmp.remove_filename();
			tmp /= move(includepath);
			if(filesys.exists({tmp})) {
				includepath = move(tmp);
				found = true;
			}
		}

		if(!include_dirs.empty()) {
			if(!found) {
				for(const pstring &include_dir : include_dirs) {
					pstring tmp{include_dir/includepath};
					if(filesys.exists({tmp})) {
						includepath = move(tmp);
						found = true;
						break;
					}
				}
			}
		}

		if(!found) {
			return true;
		}

		if(!generate_from_include(includepath)) {
			return false;
		}
		
		return true;
	}

	bool file_timestamp_builder::collect_includes(const pstring &path)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		ucstring text{};
		filesys.open_text_file({path}, text);
		
		ucstring::const_iterator it{text.cbegin()};
		ucstring::const_iterator end{text.cend()};
		while(it < end) {
			if(*it == u8'#') {
				it++;
				if(it >= end) {
					break;
				}
				if(skip_space(it, end)) {
					break;
				}
				ucstring name{};
				if(read_name(it, end, name)) {
					break;
				}
				if(name == u8"include"_sv) {
					if(skip_space(it, end)) {
						break;
					}
					bool local{false};
					if(read_value(it, end, name, local)) {
						break;
					}
					if(!name.empty()) {
						if(!found_include(path, name, local)) {
							return false;
						}
					}
				}
			}
			it++;
		}

		return true;
	}
	
	bool file_timestamp_builder::skip_space(ucstring::const_iterator &it, ucstring::const_iterator end)
	{
		while(true) {
			if(it >= end || *it == u8'\0') {
				return true;
			}
			if(*it != u8' ' &&
				*it != u8'\t' &&
				*it != u8'\n' &&
				*it != u8'\r') {
				break;
			}
			it++;
		}
		return false;
	}
	
	bool file_timestamp_builder::read_name(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &name)
	{
		name.clear();
		
		while(true) {
			if(it >= end || *it == u8'\0') {
				return true;
			}
			if(*it != u8' ' &&
				*it != u8'\t' &&
				*it != u8'\n' &&
				*it != u8'\r') {
				name += *it;
			} else {
				break;
			}
			it++;
		}
		return false;
	}

	bool file_timestamp_builder::read_value(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &value, bool &local)
	{
		value.clear();
		
		if(*it != u8'"' && *it != u8'<') {
			return false;
		}
		
		bool inside{false};
		while(true) {
			if(it >= end || *it == u8'\0') {
				return true;
			}
			if(*it == u8'"') {
				if(inside) {
					break;
				} else {
					local = true;
					inside = true;
				}
			} else if(*it == u8'<') {
				local = false;
				inside = true;
			} else if(*it == u8'>') {
				break;
			} else {
				if(inside) {
					value += *it;
				}
			}
			it++;
		}
		
		return false;
	}
	
	bool file_timestamp_builder::is_cpp_header(const pstring &ext)
	{
		static const pstring file_exts[]{
			u8".h"_p,
			u8".hh"_p,
			u8".hx"_p,
			u8".hxx"_p,
			u8".hpp"_p,
			u8".h++"_p,
			u8".ipp"_p,
			u8".tpp"_p,
			u8".inl"_p,
			u8".inc"_p,
		};
		for(const pstring &it : file_exts) {
			if(ext == it) {
				return true;
			}
		}
		return false;
	}
	
	bool file_timestamp_builder::is_cpp_source(const pstring &ext)
	{
		static const pstring file_exts[]{
			u8".cpp"_p,
			u8".c"_p,
			u8".cc"_p,
			u8".cx"_p,
			u8".cxx"_p,
			u8".c++"_p,
			u8".S"_p,
		};
		for(const pstring &it : file_exts) {
			if(ext == it) {
				return true;
			}
		}
		return false;
	}
}