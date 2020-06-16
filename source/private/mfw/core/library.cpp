#include <public/mfw/core/library.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/core/searchpath.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
	#include <libloaderapi.h>
	#include <DbgHelp.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <dlfcn.h>
	#include <public/mfw/core/environment.hpp>
	#include <elf.h>
#endif

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_library, u8"core/library"_p)
	
	namespace __library_internal
	{
		static library::library_list_t libraries{};
		
	#if MFW_OS == MFW_OS_LINUX
		ucstring dl_err_str()
		{
			char *ptr{dlerror()};
			return ucstring{uc_str(ptr)};
		}
	#endif
	}

	MFW_CORE_API bool MFW_CORE_CALL library::add_directory(const searchpath &search)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		pstring fullpath{filesys.resolve(search)};
		if(fullpath.empty()) {
			return false;
		}

		if(!filesys.is_directory(fullpath)) {
			return false;
		}

	#if MFW_OS == MFW_OS_WINDOWS
		cookie_list_t &list{cookies()};
		if(list.find(fullpath) != list.end()) {
			return false;
		}

		DLL_DIRECTORY_COOKIE cookie{AddDllDirectory(fullpath.c_str())};
		if(!cookie) {
			return false;
		}

		list.insert_or_assign(move(fullpath), cookie);
		return true;
	#elif MFW_OS == MFW_OS_LINUX
		ucstring value{as_string<ucstring>(fullpath)};
		environment_var envvar{u8"LD_RUN_PATH"_sv, u8':'};
		envvar.append(value);
		envvar.commit();
		return true;
	#endif
	}

	MFW_CORE_API bool MFW_CORE_CALL library::remove_directory(const searchpath &search)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		pstring fullpath{filesys.resolve(search)};
		if(fullpath.empty()) {
			return false;
		}

		if(!filesys.is_directory(fullpath)) {
			return false;
		}

	#if MFW_OS == MFW_OS_WINDOWS
		cookie_list_t &list{cookies()};
		cookie_list_t::const_iterator it{list.find(fullpath)};
		if(it == list.end()) {
			return false;
		}

		DLL_DIRECTORY_COOKIE cookie{it->second};

		if(!RemoveDllDirectory(cookie)) {
			return false;
		}

		list.erase(it);
		return true;
	#elif MFW_OS == MFW_OS_LINUX
		ucstring value{as_string<ucstring>(fullpath)};
		environment_var envvar{u8"LD_RUN_PATH"_sv, u8':'};
		envvar.remove(value);
		envvar.commit();
		return true;
	#endif
	}

	library::library_list_t::iterator library::find_library(const pstring &)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		handle_t modul{nullptr};
		if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, c_str(name), &modul)) {
			return libraries().end();
		}

		return find_library(modul);
	#else
		MFW_MESSAGE("TODO!!!")
		return __library_internal::libraries.end();
	#endif
	}

	library::library_list_t::iterator library::find_library(handle_t dll)
	{
		library_list_t &list{__library_internal::libraries};

		if(!dll) {
			return list.end();
		}

		library_list_t::iterator it{list.begin()};
		while(it != list.end()) {
			const library &lib{*it};
			if(lib.handle() == dll) {
				return it;
			}
			it++;
		}

		return list.end();
	}

	MFW_CORE_API library * MFW_CORE_CALL library::load_library(const searchpath &search)
	{
		pstring resolved{interfaces::filesystem::instance().resolve(search)};
		if(resolved.empty()) {
			resolved = search.dir();
		}

		library_list_t::iterator it{find_library(resolved)};
		if(it != __library_internal::libraries.end()) {
			return &(*it);
		}

		library tmp{};
		if(!tmp.load({resolved})) {
			return nullptr;
		}

		return &add_to_list(tmp);
	}

	MFW_CORE_API bool MFW_CORE_CALL library::unload_library(const searchpath &search)
	{
		pstring resolved{interfaces::filesystem::instance().resolve(search)};
		if(resolved.empty()) {
			return false;
		}

		library_list_t::iterator it{find_library(resolved)};
		if(it == __library_internal::libraries.end()) {
			return false;
		}

		library &lib{*it};
		return lib.unload();
	}
	
	namespace __library_internal
	{
	#if MFW_OS_IS(LINUX)
		#define __MFW_ELF_TYPE(name) \
			template <size_t value> \
			struct Elf_##name { using type = void; }; \
			template <> \
			struct Elf_##name<ELFCLASS32> { using type = Elf32_##name; }; \
			template <> \
			struct Elf_##name<ELFCLASS64> { using type = Elf64_##name; }; \
			template <size_t value> \
			using Elf_##name##_t = typename Elf_##name<value>::type;
	
		__MFW_ELF_TYPE(Off)
		__MFW_ELF_TYPE(Half)
		__MFW_ELF_TYPE(Word)
		__MFW_ELF_TYPE(Sword)
		__MFW_ELF_TYPE(Xword)
		__MFW_ELF_TYPE(Sxword)
		__MFW_ELF_TYPE(Addr)
		__MFW_ELF_TYPE(Section)
		__MFW_ELF_TYPE(Versym)
		__MFW_ELF_TYPE(Shdr)
		__MFW_ELF_TYPE(Ehdr)
		__MFW_ELF_TYPE(Syminfo)
		__MFW_ELF_TYPE(Sym)
		__MFW_ELF_TYPE(Chdr)
		
		#define __MFW_ELF_TYPE_LOCAL(name) \
			using Elf_##name = Elf_##name##_t<value>;
	
		template <size_t value>
		static ucstring get_name_from_table(const byte *name_table, Elf_Word_t<value> index)
		{
			const char *name_ptr{reinterpret_cast<const char *>(name_table + index)};
			ucstring name{reinterpret_cast<const ucchar_t *>(name_ptr)};
			return name;
		}
	#endif
	
	#if MFW_OS_IS(LINUX)
		template <size_t value>
	#endif
		static bool get_library_exports(const byte *data, library::export_vec_t &exports, bool file)
		{
			if(!data) {
				return false;
			}
			
		#if MFW_OS_IS(LINUX)
			__MFW_ELF_TYPE_LOCAL(Off)
			__MFW_ELF_TYPE_LOCAL(Half)
			__MFW_ELF_TYPE_LOCAL(Word)
			__MFW_ELF_TYPE_LOCAL(Sword)
			__MFW_ELF_TYPE_LOCAL(Xword)
			__MFW_ELF_TYPE_LOCAL(Sxword)
			__MFW_ELF_TYPE_LOCAL(Addr)
			__MFW_ELF_TYPE_LOCAL(Section)
			__MFW_ELF_TYPE_LOCAL(Versym)
			__MFW_ELF_TYPE_LOCAL(Shdr)
			__MFW_ELF_TYPE_LOCAL(Ehdr)
			__MFW_ELF_TYPE_LOCAL(Syminfo)
			__MFW_ELF_TYPE_LOCAL(Sym)
			__MFW_ELF_TYPE_LOCAL(Chdr)
			
			const Elf_Ehdr &hdr{*reinterpret_cast<const Elf_Ehdr *>(data)};
			
			if(hdr.e_type != ET_EXEC &&
				hdr.e_type != ET_DYN) {
				return false;
			}
			
			const Elf_Shdr *section_table{reinterpret_cast<const Elf_Shdr *>(data + hdr.e_shoff)};
			
			const Elf_Shdr *section_dynsym{nullptr};
			
			for(Elf_Half i{0}; i < hdr.e_shnum; i++) {
				const Elf_Shdr &section_header{section_table[i]};
				
				if(section_header.sh_type == SHT_DYNSYM) {
					section_dynsym = &section_header;
				}
				
				if(section_dynsym) {
					break;
				}
			}
			
			if(!section_dynsym) {
				return false;
			}
			
			const Elf_Sym *symbol_table{reinterpret_cast<const Elf_Sym *>(data + section_dynsym->sh_offset)};
			const Elf_Shdr &symbol_name_section{section_table[section_dynsym->sh_link]};
			const byte *symbol_name_table{reinterpret_cast<const byte *>(data + symbol_name_section.sh_offset)};
			
			Elf_Xword symbol_count{section_dynsym->sh_size / section_dynsym->sh_entsize};
			
			for(Elf_Xword i{0}; i < symbol_count; i++) {
				const Elf_Sym &symbol_header{symbol_table[i]};
				
				MFW_MESSAGE("remove 64 later")
				int32_t type{ELF64_ST_TYPE(symbol_header.st_info)};
				int32_t bind{ELF64_ST_BIND(symbol_header.st_info)};
				int32_t visibility{ELF64_ST_VISIBILITY(symbol_header.st_other)};
				
				if((type != STT_FUNC && type != STT_OBJECT) ||
					(bind != STB_GLOBAL && bind != STB_WEAK) ||
					visibility != STV_DEFAULT) {
					continue;
				}
				
				if(symbol_header.st_shndx == SHN_UNDEF ||
					symbol_header.st_shndx == SHN_ABS ||
					symbol_header.st_shndx == SHN_COMMON ||
					symbol_header.st_shndx == SHN_XINDEX) {
					continue;
				}

				ucstring name{get_name_from_table<value>(symbol_name_table, symbol_header.st_name)};
				if(name.empty() ||
					name.find(u8"_init"_sv) != ucstring::npos ||
					name.find(u8"_fini"_sv) != ucstring::npos) {
					continue;
				}

				library::export_t &exp{exports.emplace_back()};
				exp.index = i;
				exp.name = move(name);
			}
			
			return true;
		#elif MFW_OS_IS(WINDOWS)
			const IMAGE_DOS_HEADER &dosheader{*reinterpret_cast<const IMAGE_DOS_HEADER *>(data)};
			const IMAGE_NT_HEADERS &ntheader{*reinterpret_cast<const IMAGE_NT_HEADERS *>(data + dosheader->e_lfanew)};
			const IMAGE_OPTIONAL_HEADER &optheader{&ntheader->OptionalHeader};
			const IMAGE_DATA_DIRECTORY &datadir{&optheader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT]};
			const IMAGE_EXPORT_DIRECTORY &exportdir{*reinterpret_cast<const IMAGE_EXPORT_DIRECTORY *>(data + datadir->VirtualAddress)};

			const uint32_t *sym_names{reinterpret_cast<const uint32_t *>(data + exportdir->AddressOfNames)};
			const uint32_t *sym_funcs{reinterpret_cast<const uint32_t *>(data + exportdir->AddressOfFunctions)};
			const uint16_t *sym_ordinals{reinterpret_cast<const uint16_t *>(data + exportdir->AddressOfNameOrdinals)};

			size_t num_names{exportdir->NumberOfNames};
			size_t num_funcs{exportdir->NumberOfFunctions};

			exports.resze(num_funcs);

			for(size_t i{0}; i < num_funcs; i++) {
				const char *sym_name{reinterpret_cast<const char *>(data + sym_names[i])};
				uint16_t sym_ordinal{sym_ordinals[i]};
				const void *sym_func{reinterpret_cast<const void *>(data + sym_funcs[sym_ordinal])};

				ucstring decorated{};
				convert(reinterpret_cast<const ucchar_t *>(sym_name), decorated);

				wchar_t undecorated[512]{L'\0'};
				if(!UnDecorateSymbolNameW(reinterpret_cast<const wchar_t *>(decorated.c_str()), undecorated, size(undecorated), UNDNAME_NAME_ONLY)) {
					unload();
					return false;
				}

				library::export_t &exp{exports[i]};
				info.ptr = sym_func;
				info.name = reinterpret_cast<const char16_t *>(undecorated);
			}

			return true;
		#else
			#error
		#endif
		}
	}
	
	MFW_CORE_API bool MFW_CORE_CALL library::get_library_exports(const searchpath &search, export_vec_t &exports)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		unique_ptr<core::interfaces::file> file{filesys.open_file(search, open_flags::read)};
		if(!file) {
			return false;
		}
		
		vector<byte> code{};
		file->read(code);
		
		const byte *data{code.data()};
		
	#if MFW_OS_IS(LINUX)
		if(code[EI_MAG0] != ELFMAG0 ||
			code[EI_MAG1] != ELFMAG1 ||
			code[EI_MAG2] != ELFMAG2 ||
			code[EI_MAG3] != ELFMAG3) {
			return false;
		}
		
		if(code[EI_CLASS] == ELFCLASS32) {
			if(!__library_internal::get_library_exports<ELFCLASS32>(data, exports, true)) {
				return false;
			}
		} else if(code[EI_CLASS] == ELFCLASS64) {
			if(!__library_internal::get_library_exports<ELFCLASS64>(data, exports, true)) {
				return false;
			}
		} else {
			return false;
		}
	#elif MFW_OS_IS(WINDOWS)
		if(!__library_internal::get_library_exports(data, exports, true)) {
			return false;
		}
	#else
		#error
	#endif
		
		return true;
	}

	MFW_CORE_API ucstring MFW_CORE_CALL library::symbol_name(size_t index) const
	{
		if((index >= symbol_count()) || !valid()) {
			return {};
		}

		const export_t &info{exports[index]};
		return info.name;
	}

	MFW_CORE_API const void * MFW_CORE_CALL library::symbol(const ucstring_view &name) const
	{
		if(name.empty() || !valid()) {
			return nullptr;
		}

		for(const export_vec_t::value_type &it : exports) {
			if(it.name == name) {
				return it.ptr;
			}
		}

		return nullptr;
	}

	MFW_CORE_API const void * MFW_CORE_CALL library::symbol(size_t index) const
	{
		if((index >= symbol_count()) || !valid()) {
			return nullptr;
		}

		const export_t &info{exports[index]};
		return info.ptr;
	}

	MFW_CORE_API size_t MFW_CORE_CALL library::symbol_count() const
	{
		if(!valid()) {
			return 0;
		}

		return exports.size();
	}

	MFW_CORE_API bool MFW_CORE_CALL library::unload_all_libraries()
	{
		library_list_t &list{__library_internal::libraries};

		library_list_t::iterator it{list.begin()};
		while(it != list.end()) {
			library &lib{*it};
			lib.flags_ &= ~flags::in_list;
			it++;
			if(!lib.unload()) {
				return false;
			}
		}
		list.clear();
		return true;
	}

	MFW_CORE_API bool MFW_CORE_CALL library::remove_all_directories()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		cookie_list_t &list{cookies()};
		for(const cookie_list_t::value_type &it : list) {
			if(!RemoveDllDirectory(it.second)) {
				return false;
			}
		}
		list.clear();
		return true;
	#elif MFW_OS == MFW_OS_LINUX
		environment_var envvar{u8"LD_RUN_PATH"_sv, u8':'};
		envvar.unset();
		envvar.commit();
		return true;
	#else
		#error
	#endif
	}

	MFW_CORE_API bool MFW_CORE_CALL library::load(const searchpath &search)
	{
		unload();
		
		pstring filename{search.dir()};

	#if MFW_OS == MFW_OS_WINDOWS
		module_ = LoadLibraryExW(c_str(path), nullptr, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
		if(!module_) {
			return false;
		}

		wchar_t tmp[MAX_PATH]{L'\0'};
		GetModuleFileNameW(module_, tmp, size(tmp));
		fullpath = move(tmp);
	#elif MFW_OS == MFW_OS_LINUX
		filename.replace_extension(u8".so"_p);
		ucstring path{as_string<ucstring>(filename)};
		
		static constexpr const uint32_t dlopen_flags{RTLD_NOW|RTLD_GLOBAL|RTLD_DEEPBIND};
		
		module_ = dlopen(c_str(path), dlopen_flags);
		if(!module_) {
			ucstring err{__library_internal::dl_err_str()};
			if(err.find(u8": cannot open shared object file: No such file or directory"_sv) != ucstring::npos) {
				environment_var envvar{u8"LD_RUN_PATH"_sv, u8':'};
				for(const ucstring &i : envvar) {
					pstring tmp{i};
					tmp /= filename;
					path = as_string<ucstring>(tmp);
					module_ = dlopen(c_str(path), dlopen_flags);
					if(!module_) {
						err = __library_internal::dl_err_str();
						if(err.find(u8": cannot open shared object file: No such file or directory"_sv) == ucstring::npos) {
							log_library().error(u8"failed to load {}: {}", path, err);
							break;
						}
					} else {
						break;
					}
				}
			} else {
				log_library().error(u8"failed to load {}: {}", path, err);
			}
		}
		
		if(!module_) {
			return false;
		}
	#else
		#error
	#endif

		if(!__library_internal::get_library_exports
	#if MFW_OS_IS(LINUX)
		#if MFW_PROCESSOR_FLAGGED(64BITS)
		<ELFCLASS64>
		#elif MFW_PROCESSOR_FLAGGED(32BITS)
		<ELFCLASS32>
		#else
			#error
		#endif
	#endif
		(reinterpret_cast<const byte *>(handle()), exports, false)) {
			return false;
		}

		return true;
	}

	MFW_CORE_API bool MFW_CORE_CALL library::unload()
	{
		if(!module_) {
			return false;
		}

		if(bool_cast(flags_ & flags::in_list)) {
			flags_ &= ~flags::in_list;
			library_list_t::iterator it{find_library(handle())};
			if(it != __library_internal::libraries.end()) {
				flags_ |= flags::dont_unload;
				__library_internal::libraries.erase(it);
				flags_ &= ~flags::dont_unload;
			}
		}

		if(!bool_cast(flags_ & flags::dont_unload)) {
		#if MFW_OS == MFW_OS_WINDOWS
			if(!FreeLibrary(module_)) {
				return false;
			}
		#elif MFW_OS == MFW_OS_LINUX
			if(module_) {
				if(dlclose(module_)) {
					return false;
				}
			}
		#else
			#error
		#endif
		} else {
			flags_ &= ~flags::dont_unload;
		}

		flags_ = static_cast<flags>(0);
		exports.clear();
		module_ = nullptr;
		return true;
	}

	MFW_CORE_API library & MFW_CORE_CALL library::operator=(const library &lib)
	{
		unload();

		if(!lib.valid()) {
			return *this;
		}

		const_cast<library &>(lib).flags_ |= flags::dont_unload;

		module_ = lib.handle();
		fullpath = lib.path();
		exports = lib.exports;

		const_cast<library &>(lib).unload();

		return *this;
	}

	MFW_CORE_API library & MFW_CORE_CALL library::add_to_list(library &lib)
	{
		if(!lib.valid()) {
			return lib;
		}

		library_list_t &list{__library_internal::libraries};
		library_list_t::iterator it{find_library(lib.handle())};
		if(it != list.end()) {
			lib.flags_ |= flags::dont_unload;
			lib.unload();
			return (*it);
		}

		lib.flags_ |= flags::dont_unload;

		library &listlib{list.emplace_back(move(lib))};
		listlib.flags_ &= ~flags::dont_unload;
		listlib.flags_ |= flags::in_list;

		lib.flags_ |= flags::dont_unload;
		lib.unload();

		return listlib;
	}
}