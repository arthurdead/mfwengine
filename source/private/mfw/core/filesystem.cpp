#include <private/mfw/core/filesystem.hpp>
#include <private/mfw/core/file.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/library.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/pch_literals.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <fnmatch.h>
	#include <sys/stat.h>
#endif
#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <cwctype>
#else
	#error
#endif

namespace mfw::core
{
	namespace __filesystem_internal
	{
		using stl::filesystem::directory_iterator;
		using stl::filesystem::recursive_directory_iterator;
		using stl::filesystem::exists;
		using stl::filesystem::current_path;
		using stl::filesystem::create_directories;
		using stl::filesystem::is_directory;
		using stl::filesystem::create_symlink;
		using stl::filesystem::remove_all;
		using stl::filesystem::weakly_canonical;

		static pstring &clean_path(pstring &fullpath)
		{
			fullpath.make_preferred();
			fullpath = weakly_canonical(fullpath);
			return fullpath;
		}

		static bool check_if_exists(const pstring &path, bool check)
		{
			if(check) {
				return exists(path);
			} else {
				return true;
			}
		}

		static bool matches_file_pattern(const upchar_t *file, const upchar_t *pattern)
		{
		#if MFW_OS == MFW_OS_LINUX
			return (fnmatch(c_str(pattern), c_str(file), FNM_CASEFOLD) == 0);
		#else
			#error
		#endif
		}

		static bool matches_file_pattern(const pstring &file, const pstring &pattern)
		{
			bool matches{matches_file_pattern(uc_str(file), uc_str(pattern))};
			return matches;
		}

		template <typename T = directory_iterator>
		static void find_files(pstring &dir, const ucstring_view &pattern, vector<pstring> &files)
		{
			if(!exists(dir)) {
				return;
			}

			if(!is_directory(dir)) {
				clean_path(dir);
				files.push_back(dir);
				return;
			}

			T it{dir};
			for(const pstring &file : it) {
				if(matches_file_pattern(file, pattern)) {
					pstring cleaned{file};
					clean_path(cleaned);
					files.emplace_back(move(cleaned));
				}
			}
		}

		static void do_file_glob(const pstring &glob, vector<pstring> &files)
		{
			pstring lastdir{};

			pstring::const_iterator it{glob.begin()};
			while(it != glob.end()) {
				pstring dir{*it};
				ucstring str{as_string<ucstring>(dir)};
				size_t pos{str.find(u8'*')};
				if(pos != ucstring::npos) {
					if(dir.has_extension()) {
						break;
					}

					pstring::const_iterator rem_it{it};
					rem_it++;
					pstring rem{};
					while(rem_it != glob.end()) {
						rem /= *rem_it;
						rem_it++;
					}

					vector<pstring> dirs{};
					if(dir == u8"**"_sv) {
						find_files<recursive_directory_iterator>(lastdir, u8"*"_sv, dirs);
					} else {
						find_files<directory_iterator>(lastdir, u8"*"_sv, dirs);
					}
					
					files.push_back(lastdir);

					for(pstring &file : dirs) {
						if(!is_directory(file)) {
							if(dir == u8"**"_sv) {
								pstring pattern{file.parent_path() / rem};
								if(matches_file_pattern(file, pattern)) {
									clean_path(file);
									files.push_back(file);
								}
							}
							continue;
						}

						if(dir != u8"**"_sv) {
							pstring pattern{lastdir / dir};
							if(!matches_file_pattern(file, pattern)) {
								continue;
							}
						}

						pstring newglob{file / rem};

						do_file_glob(newglob, files);
					}

					return;
				} else if(!is_directory(lastdir / dir)) {
					break;
				}

				lastdir /= dir;

				it++;
			}

			ucstring str{as_string<ucstring>(glob)};
			if(lastdir == str) {
				clean_path(lastdir);
				files.push_back(lastdir);
			} else {
				find_files(lastdir, str, files);
			}
		}
	}

	MFW_DECLARE_LOG_CONTEXT(log_filesystem, u8"core/filesystem"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(filesystem, core::filesystem)

	core::filesystem &core::filesystem::instance() {
		return __filesystem_global_allocator.instance();
	}
	MFW_CORE_API interfaces::filesystem & MFW_CORE_CALL interfaces::filesystem::instance()
		{ return core::filesystem::instance(); }

	exit_status core::filesystem::initialize()
	{
		return {};
	}

	exit_status core::filesystem::shutdown()
	{
		return {};
	}

	void core::filesystem::print_searchmap() const
	{
		for(const search_map_t::value_type &it : searchmap) {
			log_filesystem().info(u8"{}:"_sv, it.first);
			for(const pstring &file : it.second) {
				log_filesystem().info(u8"\t{}"_sv, as_string<ucstring>(file));
			}
		}
	}

	pstring core::filesystem::clean(const searchpath &search) const
	{
		pstring resolved{resolve(search, false)};
		if(resolved.empty()) {
			resolved = search.dir();
			__filesystem_internal::clean_path(resolved);
		}

		return resolved;
	}

	bool core::filesystem::add_searchpath(const searchpath &search, const searchpath &relative)
	{
		const ucstring &name{search.name()};
		const pstring &dir{search.dir()};

		if(search.empty()) {
			return false;
		}

		search_map_t::iterator it{searchmap.find(name)};
		if(it == searchmap.end()) {
			it = searchmap.insert_or_assign(name, vector<pstring>{}).first;
		}

		if(!relative.empty()) {
			vector<pstring> resolved{};
			resolve(relative, resolved, false);

			if(resolved.empty()) {
				return false;
			}

			vector<pstring> &files{it->second};

			for(const pstring &file : resolved) {

				pstring clean{file / dir};
				__filesystem_internal::clean_path(clean);

				if(find(files.cbegin(), files.cend(), clean) == files.cend()) {
					//__filesystem_internal::create_directories(clean);
					files.emplace_back(move(clean));
				}
			}

			return true;
		} else {
			if(!dir.is_absolute()) {
				return false;
			}

			pstring &clean{__filesystem_internal::clean_path(it->second.emplace_back(dir))};
			//__filesystem_internal::create_directories(clean);

			return true;
		}

		return false;
	}

	bool core::filesystem::remove_searchpath(const searchpath &)
	{
		MFW_MESSAGE("implement")
		MFW_DEBUGBREAK();
		return false;
	}

	const vector<pstring> *core::filesystem::get_paths(const ucstring &name) const
	{
		search_map_t::const_iterator it{searchmap.find(name)};
		if(it == searchmap.end()) {
			return nullptr;
		}

		return &it->second;
	}

	pstring core::filesystem::get_working_dir() const
	{
		return __filesystem_internal::current_path();
	}

	bool core::filesystem::set_working_dir(const searchpath &search) const
	{
		pstring resolved{resolve(search, false)};
		if(resolved.empty()) {
			return false;
		}

		if(!create_directories({resolved})) {
			return false;
		}

		__filesystem_internal::current_path(resolved);
		return true;
	}

	pstring core::filesystem::resolve(const searchpath &search, bool exists) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved, exists);

		pstring fullpath{};

		if(!resolved.empty()) {
			vector<pstring>::const_reverse_iterator it{resolved.crbegin()};
			while(it != resolved.crend()) {
				const pstring &file{*it};
				if(__filesystem_internal::check_if_exists(file, exists)) {
					fullpath = file;
					__filesystem_internal::clean_path(fullpath);
					break;
				}
				it++;
			}
		}
		if(fullpath.empty() && !resolved.empty()) {
			fullpath = *resolved.crbegin();
			__filesystem_internal::clean_path(fullpath);
		}

		return fullpath;
	}

	bool core::filesystem::resolve(const searchpath &search, vector<pstring> &paths, bool exists) const
	{
		const pstring &dir{search.dir()};
		const ucstring &name{search.name()};

		pstring tmp{};

		if(dir.is_absolute()) {
			tmp = dir;
			__filesystem_internal::clean_path(tmp);
			if(__filesystem_internal::check_if_exists(tmp, exists)) {
				paths.emplace_back(tmp);
			}
		} else {
			search_map_t::const_iterator search_it{};
			if(name.empty()) {
			#if 0
				search_it = searchmap.find(u8"executable"_s);
			#else
				search_it = searchmap.end();
			#endif
			} else {
				search_it = searchmap.find(name);
			}
			if(search_it != searchmap.end()) {
				const vector<pstring> &resolved{search_it->second};
				vector<pstring>::const_reverse_iterator it{resolved.crbegin()};
				while(it != resolved.crend()) {
					const pstring &file{*it};
					if(!dir.empty()) {
						tmp = file / dir;
					} else {
						tmp = file;
					}
					__filesystem_internal::clean_path(tmp);
					if(__filesystem_internal::check_if_exists(tmp, exists)) {
						paths.emplace_back(tmp);
					}
					it++;
				}
			} else {
				const pstring &file{get_working_dir()};
				if(!dir.empty()) {
					tmp = file / dir;
				} else {
					tmp = file;
				}
				__filesystem_internal::clean_path(tmp);
				if(__filesystem_internal::check_if_exists(tmp, exists)) {
					paths.emplace_back(tmp);
				}
			}
		}

		return true;
	}

	bool core::filesystem::matches_glob(const searchpath &search, const pstring &pattern) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved, false);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &file : resolved) {
			if(!__filesystem_internal::matches_file_pattern(file, pattern)) {
				return false;
			}
		}

		return true;
	}

	bool core::filesystem::glob(const searchpath &search, vector<pstring> &files) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved, false);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &file : resolved) {
			__filesystem_internal::do_file_glob(file, files);
		}

		return true;
	}

	bool core::filesystem::is_directory(const searchpath &search) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved, false);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &file : resolved) {
			if(!__filesystem_internal::is_directory(file)) {
				return false;
			}
		}

		return true;
	}

	interfaces::file *core::filesystem::open_file(const searchpath &search, open_flags flags) const
	{
		bool write{bool_cast(flags & open_flags::write)};
		bool truncate{bool_cast(flags & open_flags::truncate)};
		bool create{bool_cast(flags & open_flags::create)};

		pstring resolved{resolve(search, !create)};
		if(resolved.empty()) {
			return nullptr;
		}

		bool read{bool_cast(flags & open_flags::read)};

	#if MFW_OS == MFW_OS_WINDOWS
		uint32_t access{0};
		if(read) {
			access |= GENERIC_READ;
		}
		if(write || truncate) {
			access |= GENERIC_WRITE;
		}

		uint32_t disposition{0};
		if(create && !truncate) {
			disposition |= OPEN_ALWAYS;
		} else if(create && truncate) {
			disposition |= CREATE_ALWAYS;
		} else if(!create && !truncate) {
			disposition |= OPEN_EXISTING;
		} else if(!create && truncate) {
			disposition |= TRUNCATE_EXISTING;
		}
	#elif MFW_OS == MFW_OS_LINUX
		const char8_t *disposition{nullptr};
		if(read && !write && !truncate && !create) {
			disposition = u8"rb";
		} else if(!read && write && truncate && create) {
			disposition = u8"wb";
		} else if(!read && write && !truncate && create) {
			disposition = u8"ab";
		} else if(read && write && !truncate && !create) {
			disposition = u8"rb+";
		} else if(read && write && truncate && create) {
			disposition = u8"wb+";
		} else if(read && write && !truncate && !create) {
			disposition = u8"ab+";
		}
	#endif

		if(create) {
			if(!create_directories({resolved})) {
				return nullptr;
			}
		}

	#if MFW_OS == MFW_OS_WINDOWS
		core::file::handle_t hndl{CreateFileW(c_str(resolved), access, FILE_SHARE_READ|FILE_SHARE_WRITE, nullptr, disposition, FILE_ATTRIBUTE_NORMAL, nullptr)};
		if(hndl == core::file::invalid_handle) {
			return nullptr;
		}
	#elif MFW_OS == MFW_OS_LINUX
		core::file::handle_t hndl{fopen64(c_str(resolved), c_str(disposition))};
		if(hndl == core::file::invalid_handle) {
			return nullptr;
		}
	#else
		#error
	#endif

		return new core::file{hndl, move(resolved)};
	}

	bool core::filesystem::exists(const searchpath &search) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved, false);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &file : resolved) {
			if(!__filesystem_internal::exists(file)) {
				return false;
			}
		}

		return true;
	}

	bool core::filesystem::remove(const searchpath &search) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &file : resolved) {
			if(__filesystem_internal::remove_all(file) == 0) {
				return false;
			}
		}

		return true;
	}

	bool core::filesystem::create_symlink(const searchpath &from, const searchpath &to) const
	{
		pstring to_resolved{resolve(to, false)};
		if(to_resolved.empty()) {
			return false;
		}

		if(__filesystem_internal::exists(to_resolved)) {
			return true;
		}

		if(!create_directories({to_resolved})) {
			return false;
		}

		pstring from_resolved{resolve(from)};
		if(from_resolved.empty()) {
			return false;
		}

		__filesystem_internal::create_symlink(from_resolved, to_resolved);
		return true;
	}

	bool core::filesystem::create_directories(const searchpath &search) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved, false);

		if(resolved.empty()) {
			return false;
		}

		bool failed{false};
		for(pstring &file : resolved) {
			if(file.has_extension()) {
				file.remove_filename();
			}

			if(__filesystem_internal::exists(file)) {
				continue;
			}

			if(!__filesystem_internal::create_directories(file)) {
				failed = true;
			}
		}

		return !failed;
	}

	bool core::filesystem::open_text_file(const searchpath &search, ucstring &str) const
	{
		vector<pstring> resolved{};
		resolve(search, resolved);

		if(resolved.empty()) {
			return false;
		}

		for(const pstring &path : resolved) {
			interfaces::file *file_{open_file({path}, open_flags::read)};
			if(!file_) {
				continue;
			}

			size_t filesize{file_->size()};
			if(filesize == 0) {
				delete file_;
				continue;
			}

			byte BOM[5]{};
			file_->read(BOM);
			file_->rewind();

			bool utf8{BOM[0] == 0xEF && BOM[1] == 0xBB && BOM[2] == 0xBF};
			bool utf16_le{BOM[0] == 0xFF && BOM[1] == 0xFE};
			bool utf16_be{BOM[0] == 0xFE && BOM[1] == 0xFF};
			bool utf16{utf16_le || utf16_be};
			bool utf32_le{BOM[0] == 0x00 && BOM[1] == 0x00 && BOM[2] == 0xFE && BOM[3] == 0xFF};
			bool utf32_be{BOM[0] == 0xFF && BOM[1] == 0xFE && BOM[2] == 0x00 && BOM[3] == 0x00};
			bool utf32{utf32_le || utf32_be};

			/*if(utf32) {
				file_->seek(4, seek::begin);

				u32string u32str{};
				u32str.resize(filesize);
				file_->read(u32str.data(), u32str.size());

				if(*(u32str.end()-1) == U'\0') {
					u32str.erase(u32str.end()-1);
				}

				MFW_MESSAGE("TODO!!!!")
				MFW_DEBUGBREAK();
				//replace_all(u32str, U"\r\n"sv, U"\n"sv);

				u8string u8str{};
				MFW_MESSAGE("TODO!!!!")
				MFW_DEBUGBREAK();
				//to_string(u32str, u8str);

				str += u8str;
			} else if(utf16) {
				file_->seek(2, seek::begin);

				u16string u16str{};
				u16str.resize(filesize);
				file_->read(u16str.data(), u16str.size());

				if(*(u16str.end()-1) == u'\0') {
					u16str.erase(u16str.end()-1);
				}

				MFW_MESSAGE("TODO!!!!")
				MFW_DEBUGBREAK();
				//replace_all(u16str, u"\r\n"sv, u"\n"sv);

				u8string u8str{};
				to_string(u16str, u8str);

				str += u8str;
			} else {*/
				if(utf8) {
					file_->seek(3, seek::begin);
				}

				u8string u8str{};
				u8str.resize(filesize);
				file_->read(u8str.data(), u8str.size());

				if(*(u8str.end()-1) == u8'\0') {
					u8str.erase(u8str.end()-1);
				}

				replace_all(u8str, u8"\r\n"sv, u8"\n"sv);

				str += u8str;
			//}

			delete file_;
		}

		return true;
	}

	bool core::filesystem::save_text_file(const searchpath &search, const ucstring &str) const
	{
		pstring resolved{resolve(search, false)};

		if(resolved.empty()) {
			return false;
		}

		if(!create_directories({resolved})) {
			return false;
		}

		interfaces::file *file_{open_file({resolved}, open_flags::all)};
		if(!file_) {
			return false;
		}

	#if 0
		byte BOM[3]{};
		BOM[0] = byte{0xEF};
		BOM[1] = byte{0xBB};
		BOM[2] = byte{0xBF};
		file_->write(BOM);
	#endif

		file_->write(str.c_str(), str.length()-1);

		delete file_;

		return true;
	}

	bool core::filesystem::initialize(const pstring &exepath)
	{
	#if MFW_OS == MFW_OS_LINUX
		#define __MFW_OS_TARGET "linux"
	#elif MFW_OS == MFW_OS_WINDOWS
		#define __MFW_OS_TARGET "windows"
	#else
		#error
	#endif
		
	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		#define __MFW_CONFIGURATION_TARGET "debug"
	#else
		#define __MFW_CONFIGURATION_TARGET "release"
	#endif

	#if MFW_PROCESSOR == MFW_PROCESSOR_X86_64
		#define __MFW_PROCESSOR_TARGET "x86_64"
	#elif MFW_PROCESSOR == MFW_PROCESSOR_X86
		#define __MFW_PROCESSOR_TARGET "x86"
	#else
		#error
	#endif

		MFW_MESSAGE("get rid of the above")

		//set_working_dir(exepath);

		add_searchpath({exepath, u8"executable"_sv});

		add_searchpath({u8"core"_p, u8"root"_sv}, {{}, u8"executable"_sv});
		add_searchpath({u8"bin"_p / __MFW_OS_TARGET "_" __MFW_PROCESSOR_TARGET "_" __MFW_CONFIGURATION_TARGET, u8"bin"_sv}, {{}, u8"root"_sv});
		library::add_directory({{}, u8"bin"_sv});

		//add_searchpath({u8"thirdparty"_p, u8"root"_sv}, {{}, u8"executable"_sv});
		//add_searchpath({u8"bin"_p / __MFW_PROCESSOR_TARGET / __MFW_CONFIGURATION_TARGET, u8"bin"_sv}, {{}, u8"root"_sv});
		//library::add_directory({{}, u8"bin"_sv});

		return true;
	}

	uint64_t core::filesystem::get_file_modified_time(const searchpath &search) const
	{
		pstring resolved{resolve(search)};
		if(resolved.empty()) {
			return 0;
		}

	#if MFW_OS == MFW_OS_LINUX
		struct stat64 buffer{};
		stat64(c_str(resolved), &buffer);
		return buffer.st_mtim.tv_sec;
	#else
		#error
	#endif
	}
}