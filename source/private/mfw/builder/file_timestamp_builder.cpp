#include <private/mfw/builder/file_timestamp_builder.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/logging_interface.hpp>

namespace mfw::builder
{
	MFW_DECLARE_LOG_CONTEXT(log_filestamp, u8"buidler/file_timestamp_builder"_p)

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

	void file_timestamp_builder::file_data_t::write_to(core::serializable &out)
	{
		core::serializable &child{out.child(as_string<ucstring>(filepath))};
		child.set_value(modified);

		if(!depends_on_.empty()) {
			core::serializable &depends_on{child.child(u8"depends_on")};
			for(file_data_t *depends : depends_on_) {
				core::serializable &child2{depends_on.child(as_string<ucstring>(depends->filepath))};
				child2.set_value(depends->modified);
			}
		}

		if(!depended_by_.empty()) {
			core::serializable &depended_by{child.child(u8"depended_by")};
			for(file_data_t *depends : depended_by_) {
				core::serializable &child2{depended_by.child(as_string<ucstring>(depends->filepath))};
				child2.set_value(depends->modified);
			}
		}
	}

	bool file_timestamp_builder::build_for_file(const core::searchpath &search, const core::searchpath &output_dir)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		pstring filepath{filesys.resolve(search)};
		if(filepath.empty()) {
			return false;
		}

		if(!find_or_create_data(filepath)) {
			return false;
		}

		pstring resolved{filesys.resolve(output_dir, false)};
		if(!resolved.empty()) {
			for(file_data_t &data : files) {
				pstring basename{data.filepath.filename()};
				basename.replace_extension(u8".timestamp.sr"_p);

				pstring fullpath{resolved/basename};

				core::serializable timestamp{};
				timestamp.from_file({fullpath});
				data.write_to(timestamp);
				timestamp.to_file({fullpath});
			}
		}

		clear();

		return true;
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

	file_timestamp_builder::file_data_t *file_timestamp_builder::find_or_create_data(const pstring &filepath)
	{
		for(file_data_t &data : files) {
			if(data.filepath == filepath) {
				return &data;
			}
		}

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		file_data_t &data{files.emplace_back()};
		data.filepath = filepath;
		data.modified = filesys.get_file_modified_time(data.filepath);

		file_data_t *tmp{&data};

		pstring ext{filepath.extension()};
		if(file_supports_includes(ext)) {
			include_collector collector{};
			if(!collector.collect_includes(tmp, *this)) {
				files.pop_back();
				tmp = nullptr;
			}
		}

		return tmp;
	}
	
	void file_timestamp_builder::file_data_t::depended_by(file_data_t &other)
	{
		file_data_t *other_ptr{&other};
		
		if(other_ptr != this) {
			if(!contains(depended_by_, other_ptr)) {
				depended_by_.emplace_back(other_ptr);
			}
		}
	}

	void file_timestamp_builder::file_data_t::depends_on(file_data_t &other, bool recurse, bool do_by)
	{
		file_data_t *other_ptr{&other};
		
		if(other_ptr != this) {
			if(!contains(depends_on_, other_ptr)) {
				depends_on_.emplace_back(other_ptr);
			}
		}
		
		if(do_by) {
			other.depended_by(*this);
		}
		
		if(recurse) {
			for(file_data_t *depends : other.depends_on_) {
				if(depends == this) {
					continue;
				}
				depends_on(*depends, false, true);
			}
		}
	}

	bool file_timestamp_builder::include_collector::found_include(file_data_t *&data, file_timestamp_builder &stampbuilder, const ucstring &value, bool local)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		pstring includepath{as_string<pstring>(value)};

		bool found{false};
		if(local) {
			pstring tmp{data->filepath};
			tmp.remove_filename();
			tmp /= move(includepath);
			if(filesys.exists({tmp})) {
				includepath = move(tmp);
				found = true;
			}
		}

		if(!found) {
			for(const pstring &include_dir : stampbuilder.include_dirs) {
				pstring tmp{include_dir/includepath};
				if(filesys.exists({tmp})) {
					includepath = move(tmp);
					found = true;
					break;
				}
			}
			if(!found && stampbuilder.extra_dirs) {
				for(const pstring &include_dir : *stampbuilder.extra_dirs) {
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

		file_data_t *other{stampbuilder.find_or_create_data(includepath)};
		if(other) {
			data->depends_on(*other);
		} else {
			return false;
		}
		
		return true;
	}
	
	bool file_timestamp_builder::include_collector::skip_space(ucstring::const_iterator &it, ucstring::const_iterator end)
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
	
	bool file_timestamp_builder::include_collector::read_name(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &name)
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

	bool file_timestamp_builder::include_collector::read_value(ucstring::const_iterator &it, ucstring::const_iterator end, ucstring &value, bool &local)
	{
		value.clear();
		
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

	bool file_timestamp_builder::include_collector::collect_includes(file_data_t *&data, file_timestamp_builder &stampbuilder)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		ucstring text{};
		filesys.open_text_file({data->filepath}, text);
		
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
					if(!found_include(data, stampbuilder, name, local)) {
						return false;
					}
				}
			}
			it++;
		}

		return true;
	}

	bool file_timestamp_builder::timestamp_changed(core::serializable &timestamp, const core::searchpath &output_dir)
	{
		bool changed{false};

		const core::univalue &value{timestamp.get_value()};
		const ucstring &name{timestamp.get_name()};
		pstring path{as_string<pstring>(name)};
		uint64_t timenow{core::interfaces::filesystem::instance().get_file_modified_time(path)};
		uint64_t timethen{static_cast<uint64_t>(value)};
		if(timethen != timenow) {
			changed = true;
		}

		core::serializable *depends_on{timestamp.get_child(u8"depends_on"_sv)};
		if(depends_on) {
			if(timestamps_changed(*depends_on, output_dir)) {
				changed = true;
			}
		}

		if(changed) {
			file_timestamp_builder builder{};
			builder.extra_dirs = &include_dirs;
			builder.build_for_file({path}, output_dir);

			core::serializable *depended_by{timestamp.get_child(u8"depended_by"_sv)};
			if(depended_by) {
				for(core::serializable &depends : *depended_by) {
					pstring depend_path{as_string<pstring>(depends.get_name())};

					builder = file_timestamp_builder{};
					builder.extra_dirs = &include_dirs;
					builder.build_for_file({depend_path}, output_dir);
				}
			}
		}

		return changed;
	}

	bool file_timestamp_builder::timestamps_changed(core::serializable &timestamps, const core::searchpath &output_dir)
	{
		bool changed{false};
		for(core::serializable &timestamp : timestamps) {
			if(timestamp_changed(timestamp, output_dir)) {
				changed = true;
				break;
			}
		}
		return changed;
	}

	bool file_timestamp_builder::timestamps_changed(core::serializable &timestamps, const ucstring &name, const core::searchpath &output_dir)
	{
		bool changed{false};
		core::serializable *timestamp{timestamps.get_child(name)};
		if(timestamp) {
			if(timestamp_changed(*timestamp, output_dir)) {
				changed = true;
			}
		}
		return changed;
	}
}