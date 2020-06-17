#include <private/mfw/builder/plugins/plugin_process.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <private/mfw/builder/references/output_tool_reference.hpp>
#include <public/mfw/core/environment.hpp>
#include <public/mfw/core/library.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/debugging.hpp>

namespace mfw::builder
{
	plugin_process::plugin_process()
		: base_plugin{u8"process"_s}
	{
		info_.process_files_conditions = false;
		info_.process_options_conditions = false;
		info_.process_tools_patterns = false;
		info_.process_sections_args_unmaped = false;
		info_.process_build_set = false;
		info_.process_single_input = true;
		info_.merge_sections_files_options = false;
		info_.process_out_of_date = false;
		info_.ignore_output = false;
	}
	
	void plugin_process::insert_help(ucstring &help)
	{
		help += u8R"(
			$(optional,min=1,description="max number of processes")
			jobs 1
			
			$(optional,count=0,description="print cmdline of each process")
			printcmdline
			
			$(optional,count=0,description="only generate the compile commands dont actually run any tool")
			only_compile_commands
		)"_sv;
	}
	
	void plugin_process::initialize(interfaces::builder_funcs &funcs)
	{
		super::initialize(funcs);
		
		core::commandline &cmdline{core::commandline::instance()};
		
		ssize_t jobs{static_cast<ssize_t>(cmdline.get_int(u8"jobs"_s, 1))};
		if(jobs == -1) {
			jobs = __MFW_PROCESSES_UNLIMITED;
		}
		max_processes_ = jobs;
		printcmdline = cmdline.get_bool(u8"printcmdline"_s);
		only_compile_commands = cmdline.get_bool(u8"only_compile_commands"_s);
	}

	bool plugin_process::tool_execute_info_t::setup(const tool_info_t &tool_info, const tool_section_reference &tool_section, const core::serializable &options_)
	{
		//options = options_;
		
	#if MFW_OS_IS(LINUX)
		MFW_MESSAGE("sad hardcode")
		options.child(u8"-c"_sv);
	
		const core::serializable *preprocessor_definitions{tool_info.tool->get_child(u8"preprocessor_definitions"_sv)};
		if(preprocessor_definitions) {
			const ucstring &name{preprocessor_definitions->get_child(0).get_name()};
			MFW_MESSAGE("VERY sad hardcode")
			options.child(name).child(u8"_GNU_SOURCE"_sv);
		}
	#else
		#error
	#endif
		
		section = &tool_section;
		tool = &tool_info;
		
		const core::serializable *output{tool_info.tool->get_child(u8"output"_sv)};
		if(output) {
			for(const core::serializable &args : *output) {
				output_arg = options.find(args.get_name());
				if(output_arg != options.end()) {
					break;
				}
			}
			if(output_arg == options.end()) {
				const ucstring &name{output->get_child(0).get_name()};
				options.child(name);
				output_arg = force_cast<core::serializable::iterator>(options.end()-1);
			}
		}
		
		return true;
	}

	namespace __plugin_process_internal
	{
		enum class compiler_output_e : uchar_t
		{
			default_,
			clang,
			emscripten,
			include_what_you_use,
		};
		
		static bool get_lib_dirs(const pstring &tool_path, ucstring &compiler_lib_dirs, bool only_first, compiler_output_e output_type)
		{
			bool emscripten{output_type == compiler_output_e::emscripten};
			bool include_what_you_use{output_type == compiler_output_e::include_what_you_use};
			bool clang{
				output_type == compiler_output_e::clang ||
				emscripten ||
				include_what_you_use
			};
			
			core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
			
			pstring temp_file_path{};
			
			if(emscripten) {
				temp_file_path = filesys.resolve({u8"__lib_search_dir.cpp"_p, u8"process"_sv}, false);
				
				core::interfaces::file *file_handle{filesys.open_file({temp_file_path}, core::open_flags::all)};
				if(!file_handle) {
					return false;
				}
				
				file_handle->write(u8"int main(int argc, char **argv) { return 0; }"_sv);
				
				delete file_handle;
			}
			
			core::process proc{};
			proc.set_path(tool_path);
			ucstring args{};
			if(clang && only_first) {
				if(emscripten) {
					args += u8'"';
					args += as_string<ucstring>(temp_file_path);
					args += u8"\" "_sv;
				}
				args += u8"-print-resource-dir"_sv;
			} else {
				args = u8"-print-search-dirs"_sv;
			}
			proc.set_args(args);
			proc.start(true);
			
			if(emscripten) {
				filesys.remove({temp_file_path});
			}
			
			const ucstring &output{proc.output()};
			if(output.empty() || proc.exit_code() != 0) {
				return false;
			}
			
			if(clang && only_first) {
				if(output_type == compiler_output_e::clang) {
					compiler_lib_dirs = output;
				} else if(emscripten && output.find(u8"warning"_sv) != ucstring::npos) {
					size_t start{output.find(u8'\n')};
					size_t end{output.find(u8'\n', start)};
					compiler_lib_dirs = output.substr(start, (end-start));
				} else if(include_what_you_use || emscripten) {
					size_t end{output.find(u8'\n')};
					compiler_lib_dirs = output.substr(0, end);
				}
				return true;
			}
			
			constexpr ucstring_view find_str{u8"libraries: ="_sv};
			
			size_t start{output.find(find_str)};
			if(start == ucstring::npos) {
				return false;
			}
			
			start += find_str.length();
			
			ucstring lib_str{output.substr(start, ucstring::npos)};
			
			start = 0;
			while(true) {
				size_t i{lib_str.find(u8':', start)};
				if(i == ucstring::npos) {
					break;
				}
				
				size_t len{i-start};
				
				ucstring str{lib_str.substr(start, len)};
				if(str.back() == u8'/') {
					str.pop_back();
				}
				
				if(only_first) {
					compiler_lib_dirs += move(str);
					break;
				} else {
					compiler_lib_dirs += u8"-L \""_sv;
					compiler_lib_dirs += move(str);
					compiler_lib_dirs += u8"\" "_sv;
				}
				
				start++;
				start += len;
			}
			
			if(!only_first) {
				compiler_lib_dirs.pop_back();
			}
			
			return true;
		}
		
		static bool get_lib_dir(const ucstring &name, ucstring &compiler_lib_dir, compiler_output_e output_type)
		{
			pstring tool_path{core::process::get_path(name)};
			return get_lib_dirs(tool_path, compiler_lib_dir, true, output_type);
		}
		
		static void collect_dirs(size_t start, size_t end, const ucstring &output, ucstring &compiler_inc_dirs)
		{
			size_t pos{start};
			while(true) {
				pos++;
				if(pos >= end) {
					break;
				}
				
				size_t newline{output.find(u'\n', pos)};
				if(newline == ucstring::npos || newline >= end) {
					break;
				}
				pos++;
				ucstring line{output.substr(pos, (newline-pos))};
				if(line.empty()) {
					break;
				}
				compiler_inc_dirs += u8"-I \""_sv;
				compiler_inc_dirs += line;
				compiler_inc_dirs += u8"\" "_sv;
				
				pos = newline;
			}
		}
		
		static bool get_inc_dirs(const pstring &tool_path, ucstring &compiler_inc_dirs, bool cpp)
		{
			core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
			
			pstring temp_file_path{u8"__inc_search_dirs"_p};
			if(cpp) {
				temp_file_path.replace_extension(u8".cpp"_p);
			} else {
				temp_file_path.replace_extension(u8".c"_p);
			}
			
			temp_file_path = filesys.resolve({temp_file_path, u8"process"_sv}, false);
			
			core::interfaces::file *file_handle{filesys.open_file({temp_file_path}, core::open_flags::all)};
			if(!file_handle) {
				return false;
			}
			
			file_handle->write(u8"int main(int argc, char **argv) { return 0; }"_sv);
			
			delete file_handle;
			
			ucstring args{u8"-E -v \""_s};
			args += as_string<ucstring>(temp_file_path);
			args += u8'"';
			
			core::process proc{};
			proc.set_path(tool_path);
			proc.set_args(args);
			proc.start(true);
			
			filesys.remove({temp_file_path});
			
			const ucstring &output{proc.output()};
			if(output.empty() || proc.exit_code() != 0) {
				return false;
			}
			
			ucstring_view find_str{u8"#include \"...\" search starts here:"_sv};
			size_t start{output.find(find_str)};
			if(start == ucstring::npos) {
				return false;
			}
			start += find_str.length();
			
			size_t dirs1{start};
			
			find_str = u8"#include <...> search starts here:"_sv;
			start = output.find(find_str);
			if(start == ucstring::npos) {
				return false;
			}
			start += find_str.length();
			
			size_t dirs2{start};
			
			find_str = u8"End of search list."_sv;
			start = output.find(find_str);
			if(start == ucstring::npos) {
				return false;
			}
			start += find_str.length();
			
			size_t end{start};
			
			collect_dirs(dirs1, dirs2, output, compiler_inc_dirs);
			collect_dirs(dirs2, end, output, compiler_inc_dirs);
			
			compiler_inc_dirs.pop_back();
			
			return true;
		}
	}

	bool plugin_process::populate_vars(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section)
	{
		const ucstring &sec_name{tool_section.get_name()};
		
		if(sec_name == u8"linker"_sv) {
			ucstring compiler_lib_dir{};
			if(!__plugin_process_internal::get_lib_dir(u8"gcc"_s, compiler_lib_dir, __plugin_process_internal::compiler_output_e::default_)) {
				return false;
			}
			builder_funcs().add_variable(u8"gcc_lib_dir"_s, compiler_lib_dir);
		}
		
		return true;
	}
	
	bool plugin_process::compiler_tool_info_t::setup(const tool_info_t &tool_info, const tool_section_reference &tool_section, const core::serializable &options_)
	{
		if(!super::setup(tool_info, tool_section, options_)) {
			return false;
		}
		
		const tool_reference *tool{tool_section.tool()};
		
		pstring tool_path{tool->path()};
		if(!tool_path.empty()) {
			info = get_compiler_info(tool_path);
		} else {
			info = get_compiler_info(tool->shell());
		}
		
		if(info & compiler_info_t::flags_t::unix_) {
			__plugin_process_internal::compiler_output_e output_type{__plugin_process_internal::compiler_output_e::default_};
			MFW_MESSAGE("remove this later")
			if(tool_path.native().find("em++"s) != npstring::npos ||
				tool_path.native().find("emcc"s) != npstring::npos) {
				output_type = __plugin_process_internal::compiler_output_e::emscripten;
			} else if(tool_path.native().find("include-what-you-use"s) != npstring::npos) {
				output_type = __plugin_process_internal::compiler_output_e::include_what_you_use;
			} else if(tool_path.native().find("clang"s) != npstring::npos) {
				output_type = __plugin_process_internal::compiler_output_e::clang;
			}
			if(!__plugin_process_internal::get_lib_dirs(tool_path, compiler_lib_dirs, false, output_type)) {
				return false;
			}
			/*if(!__plugin_process_internal::get_inc_dirs(tool_path, compiler_inc_dirs, true)) {
				return false;
			}*/
		}
		
		return true;
	}

	bool plugin_process::generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options)
	{
		const tool_reference *tool{tool_section.tool()};
		if(!tool) {
			return true;
		}
		
		compile_command_t &compile{compile_commands.emplace_back()};

		tool_info_t &tool_info{toolsinfos.emplace_back()};
		
		tool_info.tool = tool;
		tool_info.output_tool = tool_section.output_tool();
		
		tool_info.name = tool_section.get_name();
		
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		tool_info.workdir = filesys.resolve({{}, name()}, false);
		
		const ucstring &tool_name{tool_info.name};
	
		const core::serializable *subprocess{tool->get_child(u8"subprocess"_sv)};
		if(subprocess && subprocess->passes_condition(&builder_funcs())) {
			const core::serializable *path{subprocess->get_child(u8"path"_sv)};
			if(path) {
				const core::univalue &value{path->get_value()};
				tool_info.path = as_string<pstring>(value);
			} else {
				path = subprocess->get_child(u8"shell"_sv);
				if(!path) {
					return false;
				}
				
				const core::univalue &value{path->get_value()};
				tool_info.cmd = value.get_string();
			}
			
			tool_info.base_args += u8'"';
			if(tool->is_shell()) {
				ucstring cmd{tool->shell()};
				tool_info.base_args += cmd;
			} else {
				pstring tool_path{tool->path()};
				tool_info.base_args += as_string<ucstring>(tool_path);
			}
			tool_info.base_args += u8'"';
			tool_info.base_args += u8' ';
			
			const core::serializable *drive{subprocess->get_child(u8"drive"_sv)};
			if(drive) {
				const core::univalue &value{drive->get_value()};
				tool_info.drive = value.get_string();
			}
		} else {
			if(tool->is_shell()) {
				tool_info.cmd = tool->shell();
			} else {
				tool_info.path = tool->path();
			}
		}
		
		const core::serializable *environment{tool->get_child(u8"environment"_sv)};
		if(environment) {
			ucchar_t def_sep{core::environment_var::default_sep};
			
			const core::serializable *default_sep{environment->get_flag(u8"separator"_sv)};
			if(default_sep) {
				const core::univalue &value{default_sep->get_value()};
				def_sep = value.get_string()[0];
			}
			
			for(const core::serializable &child : *environment) {
				const ucstring &name{child.get_name()};
				
				ucchar_t sep{def_sep};
				
				default_sep = child.get_flag(u8"separator"_sv);
				if(default_sep) {
					const core::univalue &value{default_sep->get_value()};
					sep = value.get_string()[0];
				}
				
				core::environment_var var{name, sep};
				if(child.empty()) {
					const core::univalue &value{child.get_value()};
					var.set(value.get_string());
				} else {
					for(const core::serializable &child_value : child) {
						const ucstring &value{child_value.get_name()};
						var.append(value);
					}
				}
				var.commit();
			}
		}
	
		if(tool_name == u8"compiler"_sv) {
			if(!compiler.setup(tool_info, tool_section, options)) {
				return false;
			}
			if(compiler.info & compiler_info_t::flags_t::unix_) {
				tool_info.is_unix_compiler = true;
			}
		} else if(tool_name == u8"linker"_sv) {
			linker_info_t lnkinf{};
			if(!tool_info.path.empty()) {
				lnkinf = get_linker_info(tool_info.path);
			} else {
				lnkinf = get_linker_info(tool_info.cmd);
			}
			if(lnkinf & linker_info_t::flags_t::unix_) {
				tool_info.is_unix_linker = true;
			}
		}
	
		for(const core::serializable &child : tool_section) {
			if(child.get_name() != u8"implib"_sv) {
				continue;
			}
			
			if(!child.passes_condition(&builder_funcs())) {
				continue;
			}
			
			tool_info_t::implib_t &tool_implib{tool_info.implib};
			core::univalue folder_value{child.get_value()};
			builder_funcs().replace_vars(folder_value);
			tool_implib.folder = as_string<pstring>(folder_value);
			
			for(const core::serializable &other : child) {
				if(!other.passes_condition(&builder_funcs())) {
					continue;
				}
				const ucstring &name{other.get_name()};
				if(name == u8"arch"_sv) {
					const core::univalue &value{other.get_value()};
					if(value == u8"x86_64"_sv) {
						tool_implib.arch = tool_info_t::implib_arch_t::x86_64;
					} else if(value == u8"i386"_sv) {
						tool_implib.arch = tool_info_t::implib_arch_t::i386;
					} else if(value == u8"arm"_sv) {
						tool_implib.arch = tool_info_t::implib_arch_t::arm;
					} else if(value == u8"aarch64"_sv) {
						tool_implib.arch = tool_info_t::implib_arch_t::aarch64;
					} else {
						log().error(u8"invalid implib_arch"_sv);
						return false;
					}
				} else if(name == u8"asm"_sv) {
					const core::univalue &value{other.get_value()};
					tool_implib.output1 = as_string<pstring>(value);
				} else if(name == u8"load"_sv) {
					const core::univalue &value{other.get_value()};
					tool_implib.output2 = as_string<pstring>(value);
				} else if(name == u8"symbols"_sv) {
					for(const core::serializable &sym : other) {
						if(!sym.passes_condition(&builder_funcs())) {
							continue;
						}
						const ucstring &sym_name{sym.get_name()};
						const core::univalue &value{sym.get_value()};
						tool_info_t::implib_t::symbol_name_t &sym_info{tool_implib.symbols.emplace_back()};
						sym_info.name = sym_name;
						if(value == u8"unmangled"_sv) {
							sym_info.mangled = false;
							if(!tool_implib.has_glob) {
								tool_implib.has_glob = true;
							}
						}
						if(!tool_implib.has_glob) {
							if(sym_name.find(u8'*') != ucstring::npos) {
								tool_implib.has_glob = true;
							}
						}
					}
				} else if(name == u8"remove_symbols"_sv) {
					for(const core::serializable &sym : other) {
						if(!sym.passes_condition(&builder_funcs())) {
							continue;
						}
						const ucstring &sym_name{sym.get_name()};
						const core::univalue &value{sym.get_value()};
						tool_info_t::implib_t::symbol_name_t &sym_info{tool_implib.remove_symbols.emplace_back()};
						sym_info.name = sym_name;
						if(value == u8"unmangled"_sv) {
							sym_info.mangled = false;
						}
					}
				}
			}
		}
		
		tool_info.needs_implib = tool->get_child_bool(u8"needs_implib"_sv);
		
		const core::serializable *output_regex{tool->get_child(u8"output_regex"_sv)};
		if(output_regex) {
			const core::serializable *info{output_regex->get_child(u8"info"_sv)};
			if(info) {
				for(const core::serializable &child : *info) {
					const ucstring &name{child.get_name()};
					tool_info.info_regex.emplace_back(name);
				}
			}
			info = output_regex->get_child(u8"warning"_sv);
			if(info) {
				for(const core::serializable &child : *info) {
					const ucstring &name{child.get_name()};
					tool_info.warning_regex.emplace_back(name);
				}
			}
			info = output_regex->get_child(u8"error"_sv);
			if(info) {
				for(const core::serializable &child : *info) {
					const ucstring &name{child.get_name()};
					tool_info.error_regex.emplace_back(name);
				}
			}
			info = output_regex->get_child(u8"ignore"_sv);
			if(info) {
				for(const core::serializable &child : *info) {
					const ucstring &name{child.get_name()};
					tool_info.ignore_regex.emplace_back(name);
				}
			}
		}
		
		const core::serializable *equal_char{tool->get_child(u8"equal_char"_sv)};
		if(equal_char) {
			const core::univalue &value{equal_char->get_value()};
			tool_info.equal_char = value.get_string()[0];
		}
		
		tool_info.always_equal = tool->get_child_bool(u8"always_equal"_sv);
		
		const core::serializable *success_codes{tool->get_child(u8"non_fatal_codes"_sv)};
		if(success_codes) {
			for(const core::serializable &child : *success_codes) {
				const ucstring &name{child.get_name()};
				int32_t &code{tool_info.success_codes.emplace_back()};
				to_int(name, code);
			}
		}
		
		bool found{false};
		const core::serializable *plugin{tool_section.plugin_section()};
		if(plugin) {
			const core::serializable *warning_errors{plugin->get_child(u8"warning_errors"_sv)};
			if(warning_errors) {
				found = true;
				const core::univalue &value{warning_errors->get_value()};
				if(value.empty()) {
					tool_info.warning_errors = true;
				} else {
					tool_info.warning_errors = value.get_bool();
				}
			}
		}
		
		if(!found) {
			const core::serializable *warnings_as_errors{tool->get_child(u8"warnings_as_errors"_sv)};
			if(warnings_as_errors) {
				const core::univalue &value{warnings_as_errors->get_value()};
				if(options.get_child(value.get_string())) {
					tool_info.warning_errors = true;
				}
			}
		}
		
		ucstring str{};
		process_options(options, str, tool_info);
		tool_info.base_args += str;
		
		if(!only_compile_commands) {
			tool_info.output_path = get_output_path(*tool_info.tool, options);
		}
		
		return true;
	}

	#define __MFW_QUOTE_STR_BEGIN(var, quote_var, check) \
			bool quote_var{var.find(u8' ', 0) != ucstring::npos}; \
			if(check) { \
				str += u8'"'; \
			}
			
	#define __MFW_QUOTE_STR_END(var, quote_var, check) \
		if(check) { \
			str += u8'"'; \
		} \
		
	#define __MFW_QUOTE_STR_NAME(var, quote_var) \
		__MFW_QUOTE_STR_BEGIN(var, quote_var, quote_var) \
		str += var; \
		__MFW_QUOTE_STR_END(var, quote_var, quote_var)
		
	#define __MFW_QUOTE_STR_VALUE(var, quote_var) \
		__MFW_QUOTE_STR_BEGIN(var, quote_var, quote_var || is_file) \
		if(is_file && !drive.empty()) { \
			str += drive; \
		} \
		str += var; \
		__MFW_QUOTE_STR_END(var, quote_var, quote_var || is_file)

	#define __MFW_APPEND_VALUE(sep) \
		if(!value.empty()) { \
			if(sep != u8'\0') { \
				str += sep; \
			} \
			const ucstring &val_str{value.get_string()}; \
			__MFW_QUOTE_STR_VALUE(val_str, quote_value) \
		}

	void plugin_process::process_option(const core::serializable &option, ucstring &str, const tool_info_t &info)
	{
		const ucstring &drive{info.drive};
		
		const core::serializable *flags{option.get_flags()};
		bool needs_equal{info.always_equal};
		bool is_file{false};
		bool no_space{false};
		bool needs_values{false};
		if(flags) {
			if(flags->get_child_bool(u8"needs_equal"_sv)) {
				needs_equal = true;
			}
			if(flags->get_child_bool(u8"no_space"_sv)) {
				no_space = true;
			}
			if(flags->get_child_bool(u8"needs_values"_sv)) {
				needs_values = true;
			}
			if(flags->get_child_bool(u8"folders"_sv) ||
				flags->get_child_bool(u8"files"_sv) ||
				flags->get_child_bool(u8"folder"_sv) ||
				flags->get_child_bool(u8"file"_sv)) {
				is_file = true;
			}
		}
		
		const ucstring &arg_name{option.get_name()};
		if(option.empty()) {
			const core::univalue &value{option.get_value()};
			if((is_file || needs_values) && value.empty()) {
				return;
			}
			__MFW_QUOTE_STR_NAME(arg_name, quote_arg)
			char8_t sep{u8' '};
			if(no_space) {
				sep = u8'\0';
			} else if(needs_equal) {
				sep = info.equal_char;
			}
			__MFW_APPEND_VALUE(sep)
		} else {
			for(const core::serializable &child : option) {
				const ucstring &name{child.get_name()};
				const core::univalue &value{child.get_value()};
				__MFW_QUOTE_STR_NAME(arg_name, quote_arg)
				char8_t sep{u8' '};
				if(no_space) {
					sep = u8'\0';
				} else if(needs_equal) {
					sep = info.equal_char;
				}
				if(sep != u8'\0') {
					str += sep;
				}
				__MFW_QUOTE_STR_VALUE(name, quote_name)
				__MFW_APPEND_VALUE(u8'=')
				str += u8' ';
			}
			str.pop_back();
		}
		str += u8' ';
	}

	void plugin_process::process_options(const core::serializable &options, ucstring &str, const tool_info_t &info)
	{
		vector<const core::serializable *> do_last{};

		for(const core::serializable &option : options) {
			const core::serializable *flags{option.get_flags()};
			if(flags) {
				if(flags->get_child_bool(u8"always_last"_sv)) {
					do_last.emplace_back(&option);
					continue;
				}
			}
			process_option(option, str, info);
		}
		
		for(const core::serializable *option : do_last) {
			process_option(*option, str, info);
		}
	}

	void plugin_process::print_section(const ucstring &output, size_t start, size_t end, sec_flags &flags, const tool_info_t &info)
	{
		if(start == end || start >= output.length()) {
			return;
		}
		ucstring sub{output.substr(start, end)};
		if(sub.empty()) {
			return;
		}
		if(*(sub.cend()-1) == u8'\n') {
			sub.pop_back();
		}
		if(sub.empty()) {
			return;
		}
		
		for(const ucstring &err : info.ignore_regex) {
			if(sub.find(err) != ucstring::npos) {
				return;
			}
		}
		
		bool found{false};
		for(const ucstring &err : info.error_regex) {
			if(sub.find(err) != ucstring::npos) {
				log().set_severity(core::log_severity::error);
				flags |= sec_flags::errors;
				found = true;
				break;
			}
		}
		if(!found) {
			for(const ucstring &err : info.warning_regex) {
				if(sub.find(err) != ucstring::npos) {
					if(!info.warning_errors) {
						log().set_severity(core::log_severity::warning);
					} else {
						log().set_severity(core::log_severity::error);
					}
					flags |= sec_flags::warnings;
					found = true;
					break;
				}
			}
		}
		if(!found) {
			for(const ucstring &err : info.info_regex) {
				if(sub.find(err) != ucstring::npos) {
					log().set_severity(core::log_severity::info);
					found = true;
					break;
				}
			}
		}
		log().print(sub);
	}

	void plugin_process::print_sections(const ucstring &output, sec_flags &flags, const tool_info_t &info)
	{
		ucstring tmp{output};
		tmp.erase(remove(tmp.begin(), tmp.end(), u8'\r'), tmp.end());
		
		size_t last{0};
		for(size_t i{0}; i < tmp.size(); i++) {
			if(tmp[i] == u8'\n') {
				print_section(tmp, last, (i-last), flags, info);
				last = i+1;
			}
		}
		
		print_section(tmp, last, ucstring::npos, flags, info);
	}
	
	void plugin_process::parse_plugin(const core::serializable &section, bool replace)
	{
		const core::serializable *compile_path{section.get_child(u8"compile_commands"_sv)};
		if(compile_path) {
			const core::univalue &value{compile_path->get_value()};
			compile_command_t &compile{compile_commands.back()};
			compile.path = as_string<pstring>(value.get_string());
			if(replace) {
				builder_funcs().replace_vars(compile.path);
			}
		}
	}
	
	bool plugin_process::generate(const solution_reference &solution, const project_reference &project, const core::serializable &section)
	{
		const ucstring &section_name{section.get_name()};
		
		if(section_name == u8"plugin"_sv) {
			parse_plugin(section, false);
		}
		
		return true;
	}

	void plugin_process::add_file_to_str(const pstring &path, ucstring &str, const tool_info_t &info)
	{
		/*bool is_static{false};
		if(info.is_unix_linker) {
			is_static = (path.extension() == u8".a"_sv);
		}*/
		
		ucstring filepath_str{as_string<ucstring>(path)};
		if(!info.drive.empty()) {
			filepath_str.insert(0, info.drive);
		}
		
		/*if(is_static) {
			str += u8"--whole-archive "_sv;
		}*/
		str += u8'"';
		str += filepath_str;
		str += u8'"';
		/*if(is_static) {
			str += u8" --no-whole-archive"_sv;
		}*/
		str += u8' ';
	}

	namespace __plugin_process_internal
	{
		static constexpr void get_fmt_str_trampoline(plugin_process::tool_info_t::implib_arch_t arch, ucstring_view &trampoline)
		{
			if(arch == plugin_process::tool_info_t::implib_arch_t::x86_64) {
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/x86_64/trampoline.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::i386) {
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/i386/trampoline.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::arm) {
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/arm/trampoline.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::aarch64) {
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/aarch64/trampoline.hpp>
				};
			}
		}
		
		static constexpr void get_fmt_str_table(plugin_process::tool_info_t::implib_arch_t arch, ucstring_view &table)
		{
			if(arch == plugin_process::tool_info_t::implib_arch_t::x86_64) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/x86_64/table.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::i386) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/i386/table.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::arm) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/arm/table.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::aarch64) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/aarch64/table.hpp>
				};
			}
		}
		
		static size_t get_ptr_size(plugin_process::tool_info_t::implib_arch_t arch)
		{
			if(arch == plugin_process::tool_info_t::implib_arch_t::x86_64 ||
				arch == plugin_process::tool_info_t::implib_arch_t::aarch64) {
				return 8;
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::i386 ||
						arch == plugin_process::tool_info_t::implib_arch_t::arm) {
				return 4;
			} else {
				return 0;
			}
		}
	}

	bool plugin_process::compile_cpp_file(const solution_reference &solution, const project_reference &project, const pstring &file, const pstring &output)
	{
		const tool_info_t *tool_compiler{compiler.tool};
		if(!tool_compiler) {
			return false;
		}
		
		file_reference file_main{};
		file_main.set_name(as_string<ucstring>(file));
		
		compiler.output_arg->set_value(as_string<ucstring>(output));
		
		size_t old_max{max_processes_};
		max_processes_ = 0;
		bool executed{execute_tool(solution, project, *compiler.section, *tool_compiler, file_main, compiler.options)};
		max_processes_ = old_max;
		return executed;
	}
	
	bool plugin_process::tool_info_t::implib_t::symbol_name_t::matches(const ucstring &other) const
	{
		if(!mangled) {
			ucstring unmangled_name{};
			core::undecorate(other, unmangled_name);
			return matches_pattern(unmangled_name, name);
		} else {
			return matches_pattern(other, name);
		}
	}

	bool plugin_process::generate_implib(const solution_reference &solution, const project_reference &project, const pstring &path, const gen_lib_vars_t &vars, bool regen)
	{
		const pstring &folder{*vars.folder};
		tool_info_t::implib_arch_t arch{vars.arch};
		
		if(arch == tool_info_t::implib_arch_t::unknown) {
			return false;
		}
		
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		pstring asm_path{folder};
		asm_path /= u8"asm.S"_p;
		
		pstring load_path{folder};
		load_path /= u8"load.c"_p;
		
		bool do_load{regen || builder_funcs().regen_or_no_cache() || !filesys.exists({load_path})};
		bool do_asm{regen || builder_funcs().regen_or_no_cache() || !filesys.exists({asm_path})};
		
		if(!do_load && !do_asm) {
			return true;
		}
		
		vector<ucstring> symbols{};
		if(vars.symbols && !vars.has_glob) {
			for(const tool_info_t::implib_t::symbol_name_t &rem : *vars.symbols) {
				symbols.emplace_back(rem.name);
			}
		}
		
		if((do_load || do_asm) && symbols.empty()) {
			core::library::export_vec_t exports{};
			if(!core::library::get_library_exports({path}, exports) || exports.empty()) {
				log().error(u8"implib invalid file"_sv);
				return false;
			}
			for(const core::library::export_t &sym : exports) {
				const ucstring &name{sym.name};
				bool ignore{false};
				if(vars.remove_symbols) {
					for(const tool_info_t::implib_t::symbol_name_t &rem : *vars.remove_symbols) {
						if(rem.matches(name)) {
							ignore = true;
							break;
						}
					}
					if(ignore) {
						continue;
					}
				}
				if(vars.symbols) {
					for(const tool_info_t::implib_t::symbol_name_t &rem : *vars.symbols) {
						if(rem.matches(name)) {
							symbols.emplace_back(name);
							break;
						}
					}
				} else {
					symbols.emplace_back(name);
				}
			}
		}
		
		if(symbols.empty()) {
			return false;
		}
		
		pstring filename{path.filename()};
		filename.replace_extension();
		ucstring lib_suffix{as_string<ucstring>(filename)};
		lib_suffix += u8"_implib"_sv;
		
		ucstring asm_str{};
		
		if(do_asm) {
			size_t ptr_size{__plugin_process_internal::get_ptr_size(arch)};
			
			ucstring_view table_fmt{};
			__plugin_process_internal::get_fmt_str_table(arch, table_fmt);
			ucstring_view trampoline_fmt{};
			__plugin_process_internal::get_fmt_str_trampoline(arch, trampoline_fmt);
			
			asm_str = table_fmt;
			replace_all(asm_str, u8"$table_size"_sv, as_string<ucstring>(symbols.size() * ptr_size));
			replace_all(asm_str, u8"$$"_sv, u8"$"_sv);
			
			size_t i{0};
			for(const ucstring &sym : symbols) {
				size_t offset{i * ptr_size};
				ucstring sym_tmp{trampoline_fmt};
				replace_all(sym_tmp, u8"$number"_sv, as_string<ucstring>(i));
				replace_all(sym_tmp, u8"$offset"_sv, as_string<ucstring>(offset));
				replace_all(sym_tmp, u8"$sym"_sv, sym);
				asm_str += move(sym_tmp);
				i++;
			}
			
			replace_all(asm_str, u8"${lib_suffix}"_sv, lib_suffix);
			replace_all(asm_str, u8"$$"_sv, u8"$"_sv);
			
			filesys.save_text_file({asm_path}, asm_str);
			
			const pstring &output1{*vars.output1};
			if(!output1.empty()) {
				if(!compile_cpp_file(solution, project, asm_path, output1)) {
					return false;
				}
			}
		}
		
		if(do_load) {
			static constexpr ucstring_view load_fmt{
				#include <private/mfw/builder/implib/load.hpp>
			};
			
			asm_str = load_fmt;
			replace_all(asm_str, u8"$has_dlopen_callback"_sv, u8"0"_sv);
			replace_all(asm_str, u8"$no_dlopen"_sv, u8"1"_sv);
			replace_all(asm_str, u8"$lazy_load"_sv, u8"0"_sv);
			replace_all(asm_str, u8"$dlopen_callback"_sv, u8"dlopen_callback"_sv);
			replace_all(asm_str, u8"$load_name"_sv, as_string<ucstring>(filename));
			ucstring sym_names{};
			for(const ucstring &sym : symbols) {
				sym_names += u8'"';
				sym_names += sym;
				sym_names += u8"\",\n  "_sv;
			}
			sym_names.erase(sym_names.end()-4, sym_names.end());
			replace_all(asm_str, u8"$sym_names"_sv, sym_names);
			replace_all(asm_str, u8"${lib_suffix}"_sv, lib_suffix);
			replace_all(asm_str, u8"$$"_sv, u8"$"_sv);
			
			filesys.save_text_file({load_path}, asm_str);
			
			const pstring &output2{*vars.output2};
			if(!output2.empty()) {
				if(!compile_cpp_file(solution, project, load_path, output2)) {
					return false;
				}
			}
		}
		
		return true;
	}
	
	bool plugin_process::execute_tool(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const tool_info_t &info, const file_reference &file, const core::serializable &options)
	{
		vector<const file_reference *> files{};
		files.emplace_back(&file);
		files.emplace_back(nullptr);
		return execute_tool(solution, project, tool_section, info, files, options);
	}

	bool plugin_process::execute_tool(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const tool_info_t &info, const vector<const file_reference *> &files, const core::serializable &options)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		proc_vars_t vars{};
		vars.solution = &solution;
		vars.project = &project;

		vars.unity_build = tool_section.unity_build();
		
		bool unity_build{vars.unity_build};
		pstring &unity_file_path{vars.unity_file_path};

		ucstring str{info.base_args};
		builder_funcs().replace_vars(str);
		
		if(info.is_unix_linker && !compiler.compiler_lib_dirs.empty()) {
			str += compiler.compiler_lib_dirs;
			str += u8' ';
		}
		/*if(info.is_unix_compiler && !compiler.compiler_inc_dirs.empty()) {
			str += compiler.compiler_inc_dirs;
			str += u8' ';
		}*/
		
		bool nasty_hack{false};
		if(files.back() == nullptr) {
			const_cast<vector<const file_reference *> &>(files).pop_back();
			nasty_hack = true;
		}
		
		if(!unity_build && nasty_hack) {
			process_options(options, str, info);
		}
		
		if(!unity_build) {
			for(const file_reference *file : files) {
				pstring file_path{file->path()};
				
				if(info.needs_implib && !only_compile_commands) {
					if(file->get_flag_bool(u8"delay"_sv)) {
						gen_lib_vars_t gen_vars{};
						gen_vars.arch = info.implib.arch;
						
						pstring folder{};
						
						folder = filesys.resolve({{}, name()}, false);
						folder /= u8"implibs"_p;
						
						pstring lib_name{file_path};
						lib_name = lib_name.filename();
						lib_name.replace_extension();
						
						folder /= lib_name;
						
						pstring output1{folder};
						output1 /= u8"asm.o"_p;
						
						pstring output2{folder};
						output2 /= u8"load.o"_p;
						
						gen_vars.folder = &folder;
						gen_vars.output1 = &output1;
						gen_vars.output2 = &output2;
						
						if(generate_implib(solution, project, file_path, gen_vars, false)) {
							if(!output1.empty()) {
								add_file_to_str(output1, str, info);
							}
							if(!output2.empty()) {
								add_file_to_str(output2, str, info);
							}
							continue;
						} else {
							return false;
						}
					}
				}
				
				add_file_to_str(file_path, str, info);
			}
		} else {
			unity_file_path /= as_string<pstring>(solution.get_name());
			unity_file_path /= as_string<pstring>(project.get_name());
			unity_file_path /= u8"unity_build"_p;
			pstring ext{info.tool->unity_build_ext()};
			if(ext.empty()) {
				ext = u8".unity"_p;
			}
			unity_file_path.replace_extension(ext);
			
			unity_file_path = filesys.resolve({unity_file_path, name()}, false);

			add_file_to_str(unity_file_path, str, info);
		}
		
		str.pop_back();
		
		if(unity_build && !only_compile_commands) {
			vector<byte> unity_build_file{};

			for(const file_reference *file : files) {
				if(file->has_flag(u8"dynamic"_sv)) {
					continue;
				}
				
				core::interfaces::file *file_handle{filesys.open_file({file->path()}, core::open_flags::read)};
				if(!file_handle) {
					log().error(u8"could not open file: {}"_sv, file->path());
					return false;
				}

				vector<byte> temp{};
				file_handle->read(temp);
				temp.emplace_back(static_cast<byte>(u8'\n'));

				unity_build_file.insert(unity_build_file.cbegin(), temp.cbegin(), temp.cend());

				delete file_handle;
			}

			core::interfaces::file *file_handle{filesys.open_file({unity_file_path}, core::open_flags::all)};
			if(!file_handle) {
				log().error(u8"could not open file: {}"_sv, unity_file_path);
				return false;
			}

			file_handle->write(unity_build_file);

			delete file_handle;
		}
		
		const output_tool_reference *outputtool{info.output_tool};
		const tool_section_reference *&out_sec{vars.out_sec};
		if(outputtool) {
			out_sec = outputtool->section();
			if(!out_sec) {
				return false;
			}
		}
		
		pstring &output_path{vars.output_path};
		if(!only_compile_commands) {
			if(!info.output_path.empty()) {
				output_path = info.output_path;
				builder_funcs().replace_vars(output_path);
			} else {
				output_path = get_output_path(*info.tool, options);
			}
		}
		
		if(!output_path.empty() && !only_compile_commands) {
			if(output_path.has_extension()) {
				filesys.create_directories({output_path});
			} else {
				filesys.create_directories({output_path.parent_path()});
			}
		}
		
		if(printcmdline || builder_funcs().debugging()) {
			ucstring tmp{};
			if(!info.path.empty()) {
				tmp += as_string<ucstring>(info.path);
			} else {
				tmp += info.cmd;
			}
			tmp += u8' ';
			tmp += str;
			log().info(tmp);
		}
		
		compile_command_t &compile{compile_commands.back()};
		
		const core::serializable *plugin{tool_section.plugin_section()};
		if(plugin) {
			parse_plugin(*plugin, true);
		}
		
		if(!compile.path.empty()) {
			compile.directory = info.workdir;
			
			const core::serializable *plugin{tool_section.plugin_section()};
			if(plugin) {
				if(!generate(solution, project, *plugin)) {
					return false;
				}
			}
			
			ucstring command{};
			if(!info.path.empty()) {
				command += as_string<ucstring>(info.path);
			} else {
				command += info.cmd;
			}
			command += u8' ';
			command += str;
			
			if(unity_build) {
				compile_command_t::command_t &cmd{compile.files.emplace_back()};
				
				cmd.file = unity_file_path;
				cmd.command = command;
			} else {
				for(const file_reference *file : files) {
					compile_command_t::command_t &cmd{compile.files.emplace_back()};
					
					cmd.file = file->path();
					cmd.command = command;
				}
			}
		}

		if(!unity_build && files.size() == 1) {
			vars.file = files[0];
		}

		if(only_compile_commands) {
			return true;
		} else {
			if(!multi_process()) {
				proc_info_t proc_info{};
				proc_info.setup(move(vars), str, info);
				
				if(!start(proc_info, log())) {
					return false;
				}
				
				return process_done(proc_info.proc, vars, info);
			} else {
				proc_info_t &proc_info{processes.emplace_back()};
				proc_info.setup(move(vars), str, info);
				
				if(!hit_process_limit()) {
					if(!start(proc_info, log())) {
						return false;
					}
				}
				
				return true;
			}
		}
	}
	
	void plugin_process::proc_info_t::setup(proc_vars_t &&vars_, const ucstring &str, const tool_info_t &tool_info)
	{
		vars = vars_;
		vars.tool_info = &tool_info;
		if(!tool_info.path.empty()) {
			proc.set_path(tool_info.path);
		} else {
			proc.set_shell(tool_info.cmd);
		}
		proc.set_workingdir(tool_info.workdir);
		proc.set_args(str);
	}
	
	bool plugin_process::start(proc_info_t &proc_info, core::log_context &log)
	{
		bool &started{proc_info.started};
		core::process &proc{proc_info.proc};
		proc_vars_t &vars{proc_info.vars};
		
		if(!started) {
			core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
			
			started = proc.start();
			if(!started) {
				log.error(u8"program failed to execute"_sv);
				log.add_ident();
				log.error(u8"workingdir: {}\npath: {}\nargs: {}"_sv, proc.workingdir(), proc.path(), proc.args());
				log.remove_ident();
				
				const pstring &output_path{vars.output_path};
				
				if(!output_path.empty()) {
					filesys.remove({output_path});
				}
			} else {
				num_started++;
			}
		}
		return started;
	}
	
	bool plugin_process::process_done(core::process &proc, const proc_vars_t &vars, const tool_info_t &info)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		proc.kill();
		if(num_started) {
			num_started--;
		}
		
		const ucstring &output{proc.output()};
		int32_t exit_code{proc.exit_code()};

		bool success{exit_code == 0};
		
		for(const int32_t &i : info.success_codes) {
			if(exit_code == i) {
				success = true;
				break;
			}
		}
		
		sec_flags flags{sec_flags::none};

		if(!success) {
			flags |= sec_flags::errors;
		}
		
		bool do_print{!output.empty() || !success};
		
		const file_reference *file{vars.file};
		bool dynamic{false};
		if(file) {
			dynamic = file->get_flag_bool(u8"dynamic"_sv);
		}

		if(do_print) {
			if(exit_code == 0) {
				log().set_severity(core::log_severity::success);
			} else {
				log().set_severity(core::log_severity::error);
			}

			if(file) {
				pstring filter{file->filter()};
				
				pstring fullpath{file->path()};
				pstring filename{fullpath.filename()};
				
				log().print(u8"generating {}:"_sv, filter/filename);
				log().add_ident();
			}

			if(!output.empty()) {
				log().print(u8"exit_code {}:"_sv, exit_code);
				log().add_ident();
				log().set_severity(core::log_severity::info);
				print_sections(output, flags, info);
				log().remove_ident();
			} else if(!success) {
				log().print(u8"exit_code {}:"_sv, exit_code);
			}

			if(file) {
				log().remove_ident();
			}
		}

		if(vars.unity_build) {
			filesys.remove({vars.unity_file_path});
		}
		
		const pstring &output_path{vars.output_path};
		const tool_section_reference *out_sec{vars.out_sec};
		
		if(bool_cast(flags & sec_flags::errors) || (bool_cast(flags & sec_flags::warnings) && info.warning_errors)) {
			if(!output_path.empty()) {
				filesys.remove({output_path});
			}
			
			return false;
		} else {
			if(!output_path.empty() && out_sec && !dynamic) {
				if(file) {
					out_sec->add_file(output_path, file->get_flags(), file->filter(), file->get_condition());
				} else {
					out_sec->add_file(output_path, {}, {}, {});
				}
			}
			
			const tool_info_t::implib_t &implib{info.implib};
			if(!implib.folder.empty()) {
				gen_lib_vars_t gen_vars{};
				gen_vars.arch = implib.arch;
				gen_vars.folder = &implib.folder;
				gen_vars.output1 = &implib.output1;
				gen_vars.output2 = &implib.output2;
				gen_vars.symbols = &implib.symbols;
				gen_vars.remove_symbols = &implib.remove_symbols;
				gen_vars.has_glob = implib.has_glob;
				
				if(!generate_implib(*vars.solution, *vars.project, output_path, gen_vars, true)) {
					return false;
				}
			}
			
			return true;
		}
	}
	
	void plugin_process::tool_execute_info_t::clear()
	{
		section = nullptr;
		output_arg = options.end();
		options.clear();
		tool = nullptr;
	}
	
	void plugin_process::cleanup(cleanup_type_t type)
	{
		processes.clear();
		toolsinfos.clear();
		compiler.clear();
		
		if(type == cleanup_type_t::solution) {
			compile_commands.clear();
		}
	}
	
	bool plugin_process::running(bool &err)
	{
		if(!multi_process()) {
			err = false;
			return false;
		} else if(processes.empty()) {
			err = false;
			return false;
		}
		
		bool running{false};
		tool_info_t &tool_info{toolsinfos.back()};
		
		proc_vec_t::iterator it{processes.begin()};
		while(it != processes.end()) {
			proc_info_t &proc_info{*it};
			core::process &proc{proc_info.proc};
			const proc_vars_t &vars{proc_info.vars};
			
			if(proc_info.started) {
				if(proc.running()) {
					running = true;
				} else {
					if(!process_done(proc, vars, tool_info)) {
						processes.clear();
						err = true;
						return false;
					} else {
						processes.erase(it);
						continue;
					}
				}
			} else {
				running = true;
				
				if(!hit_process_limit()) {
					if(!start(proc_info, log())) {
						err = true;
						return false;
					}
				}
			}
			
			it++;
		}
		
		err = false;
		return running;
	}

	bool plugin_process::generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const vector<const file_reference *> &files, const core::serializable &options)
	{
		if(toolsinfos.empty()) {
			return false;
		}

		const tool_info_t &info{toolsinfos.back()};

		if(!execute_tool(solution, project, tool_section, info, files, options)) {
			return false;
		}

		return true;
	}

	bool plugin_process::generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const file_reference &file, const core::serializable &options)
	{
		if(toolsinfos.empty()) {
			return false;
		}

		const tool_info_t &info{toolsinfos.back()};

		if(!execute_tool(solution, project, tool_section, info, file, options)) {
			return false;
		}

		return true;
	}
	
	void plugin_process::compile_command_t::save()
	{
		if(path.empty()) {
			return;
		}
		
		StartArray();
		for(const command_t &cmd : files) {
			StartObject();
			Key(u8"directory"_sv);
			String(as_string<ucstring>(directory));
			Key(u8"command"_sv);
			String(cmd.command);
			Key(u8"file"_sv);
			String(as_string<ucstring>(cmd.file));
			EndObject();
		}
		EndArray();
		
		super::save({path});
	}
	
	bool plugin_process::generate(const solution_reference &solution, const project_reference &project)
	{
		return true;
	}
	
	bool plugin_process::generate(const solution_reference &solution)
	{
		for(compile_command_t &compile : compile_commands) {
			compile.save();
		}
		
		return true;
	}
}