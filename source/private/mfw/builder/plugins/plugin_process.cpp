#include <private/mfw/builder/plugins/plugin_process.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <private/mfw/builder/references/output_tool_reference.hpp>
#include <public/mfw/core/environment.hpp>
#include <public/mfw/core/library.hpp>
#include <public/mfw/core/commandline.hpp>

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
		info_.merge_sections_files_options = true;
		info_.process_out_of_date = false;
		info_.ignore_output = false;
	}
	
	void plugin_process::initialize(interfaces::builder_funcs &funcs)
	{
		super::initialize(funcs);
		
		core::commandline &cmdline{core::commandline::instance()};
		
		ssize_t jobs{static_cast<ssize_t>(cmdline.get_int(u8"jobs"_s, 0))};
		if(jobs == -1) {
			jobs = __MFW_PROCESSES_UNLIMITED;
		}
		max_processes_ = jobs;
		printcmdline = cmdline.get_bool(u8"printcmdline"_s);
	}

	bool plugin_process::execute_tool(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const tool_info_t &info, const file_reference &file, const core::serializable &options)
	{
		vector<const file_reference *> files{};
		files.emplace_back(&file);
		return execute_tool(solution, project, tool_section, info, files, options);
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

	bool plugin_process::generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options)
	{
		const tool_reference *tool{tool_section.tool()};
		if(!tool) {
			return true;
		}

		tool_info_t &tool_info{toolsinfos.emplace_back()};
		
		tool_info.tool = tool;
		tool_info.output_tool = tool_section.output_tool();
		
		tool_info.name = tool_section.get_name();
		
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		tool_info.workdir = filesys.resolve({{}, name()}, false);
		
		const ucstring &tool_name{tool_info.name};
	
		if(tool_name == u8"compiler"_sv) {
			if(!compiler.setup(tool_info, tool_section, options)) {
				return false;
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
			const core::univalue &folder_value{child.get_value()};
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
		
		const core::serializable *subprocess{tool->get_child(u8"subprocess"_sv)};
		if(subprocess) {
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
			
			drive = subprocess->get_child(u8"kill_process"_sv);
			if(drive) {
				const core::univalue &value{drive->get_value()};
				tool_info.kill_proc = value.get_string();
			}
		} else {
			if(tool->is_shell()) {
				tool_info.cmd = tool->shell();
			} else {
				tool_info.path = tool->path();
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
				flags->get_child_bool(u8"file"_sv) ||
				flags->get_child_bool(u8"files"_sv)) {
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
				str += u8' ';
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
		ucstring filepath_str{as_string<ucstring>(path)};
		if(!info.drive.empty()) {
			filepath_str.insert(0, info.drive);
		}
		
		str += u8'"';
		str += filepath_str;
		str += u8'"';
		str += u8' ';
	}

	namespace __plugin_process_internal
	{
		static constexpr void get_fmt_str(plugin_process::tool_info_t::implib_arch_t arch, ucstring_view &table, ucstring_view &trampoline)
		{
			if(arch == plugin_process::tool_info_t::implib_arch_t::x86_64) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/x86_64/table.hpp>
				};
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/x86_64/trampoline.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::i386) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/i386/table.hpp>
				};
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/i386/trampoline.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::arm) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/arm/table.hpp>
				};
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/arm/trampoline.hpp>
				};
			} else if(arch == plugin_process::tool_info_t::implib_arch_t::aarch64) {
				table = ucstring_view{
					#include <private/mfw/builder/implib/aarch64/table.hpp>
				};
				trampoline = ucstring_view{
					#include <private/mfw/builder/implib/aarch64/trampoline.hpp>
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

	bool plugin_process::generate_implib(const solution_reference &solution, const project_reference &project, const pstring &path, const gen_lib_vars_t &vars, bool regen)
	{
		const pstring &folder{vars.folder};
		tool_info_t::implib_arch_t arch{vars.arch};
		
		if(arch == tool_info_t::implib_arch_t::unknown) {
			return false;
		}
		
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		pstring asm_path{folder};
		asm_path /= u8"asm.S"_p;
		
		pstring load_path{folder};
		load_path /= u8"load.c"_p;
		
		bool do_load{regen || builder_funcs().regen_cache() || !filesys.exists({load_path})};
		bool do_asm{regen || builder_funcs().regen_cache() || !filesys.exists({asm_path})};
		
		if(!do_load && !do_asm) {
			return true;
		}
		
		core::library::export_vec_t exports{};
		if(do_load || do_asm) {
			if(!core::library::get_library_exports({path}, exports)) {
				log().error(u8"implib invalid file"_sv);
				return false;
			}
		}
		
		ucstring_view table_fmt{};
		ucstring_view trampoline_fmt{};
		if(do_load || do_asm) {
			__plugin_process_internal::get_fmt_str(arch, table_fmt, trampoline_fmt);
		}
		
		static constexpr ucstring_view load_fmt{
			#include <private/mfw/builder/implib/load.hpp>
		};
		
		pstring filename{path.filename()};
		filename.replace_extension();
		ucstring lib_suffix{as_string<ucstring>(filename)};
		lib_suffix += u8"_implib"_sv;
		
		ucstring asm_str{};
		
		if(do_asm) {
			size_t ptr_size{__plugin_process_internal::get_ptr_size(arch)};
			
			asm_str = table_fmt;
			replace_all(asm_str, u8"$table_size"_sv, as_string<ucstring>(exports.size() * ptr_size));
			replace_all(asm_str, u8"$$"_sv, u8"$"_sv);
			
			size_t i{0};
			for(const core::library::export_t &sym : exports) {
				size_t offset{i * ptr_size};
				ucstring sym_tmp{trampoline_fmt};
				replace_all(sym_tmp, u8"$number"_sv, as_string<ucstring>(i));
				replace_all(sym_tmp, u8"$offset"_sv, as_string<ucstring>(offset));
				replace_all(sym_tmp, u8"$sym"_sv, sym.name);
				asm_str += move(sym_tmp);
				i++;
			}
			
			replace_all(asm_str, u8"${lib_suffix}"_sv, lib_suffix);
			replace_all(asm_str, u8"$$"_sv, u8"$"_sv);
			
			filesys.save_text_file({asm_path}, asm_str);
			
			const pstring &output1{vars.output1};
			if(!output1.empty()) {
				if(!compile_cpp_file(solution, project, asm_path, output1)) {
					return false;
				}
			}
		}
		
		if(do_load) {
			asm_str = load_fmt;
			replace_all(asm_str, u8"$has_dlopen_callback"_sv, u8"0"_sv);
			replace_all(asm_str, u8"$no_dlopen"_sv, u8"1"_sv);
			replace_all(asm_str, u8"$lazy_load"_sv, u8"0"_sv);
			replace_all(asm_str, u8"$dlopen_callback"_sv, u8"dlopen_callback"_sv);
			replace_all(asm_str, u8"$load_name"_sv, as_string<ucstring>(filename));
			ucstring sym_names{};
			for(const core::library::export_t &sym : exports) {
				sym_names += u8'"';
				sym_names += sym.name;
				sym_names += u8"\",\n  "_sv;
			}
			sym_names.erase(sym_names.end()-4, sym_names.end());
			replace_all(asm_str, u8"$sym_names"_sv, sym_names);
			replace_all(asm_str, u8"${lib_suffix}"_sv, lib_suffix);
			replace_all(asm_str, u8"$$"_sv, u8"$"_sv);
			
			filesys.save_text_file({load_path}, asm_str);
			
			const pstring &output2{vars.output2};
			if(!output2.empty()) {
				if(!compile_cpp_file(solution, project, load_path, output2)) {
					return false;
				}
			}
		}
		
		return true;
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

		ucstring str{};
		
		process_options(options, str, info);
		
		if(!unity_build) {
			for(const file_reference *file : files) {
				pstring file_path{file->path()};
				
				if(info.needs_implib) {
					if(file->get_flag_bool(u8"delay"_sv)) {
						gen_lib_vars_t gen_vars{};
						gen_vars.arch = info.implib.arch;
						
						pstring &folder{gen_vars.folder};
						
						folder = filesys.resolve({{}, name()}, false);
						folder /= u8"implibs"_p;
						
						pstring lib_name{file_path};
						lib_name = lib_name.filename();
						lib_name.replace_extension();
						
						folder /= lib_name;
						
						gen_vars.output1 = folder;
						gen_vars.output1 /= u8"asm.o"_p;
						
						gen_vars.output2 = folder;
						gen_vars.output2 /= u8"load.o"_p;
						
						if(generate_implib(solution, project, file_path, gen_vars, false)) {
							if(!gen_vars.output1.empty()) {
								add_file_to_str(gen_vars.output1, str, info);
							}
							if(!gen_vars.output2.empty()) {
								add_file_to_str(gen_vars.output2, str, info);
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
			
			unity_file_path = filesys.resolve({unity_file_path}, false);

			add_file_to_str(unity_file_path, str, info);
		}

		if(unity_build) {
			vector<byte> unity_build_file{};

			for(const file_reference *file : files) {
				core::interfaces::file *file_handle{filesys.open_file({file->path()}, core::open_flags::read)};
				if(!file_handle) {
					log().error(u8"could not open file: {}"_sv, file->path());
					return false;
				}

				size_t size{file_handle->size()};
				vector<byte> temp{};
				temp.resize(size);
				file_handle->read(temp.data(), temp.size());

				unity_build_file.insert(unity_build_file.end(), temp.begin(), temp.end());

				delete file_handle;
			}

			core::interfaces::file *file_handle{filesys.open_file({unity_file_path, name()}, core::open_flags::all)};
			if(!file_handle) {
				log().error(u8"could not open file: {}"_sv, unity_file_path);
				return false;
			}

			file_handle->write(unity_build_file.data(), unity_build_file.size());

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
		const core::serializable *args{info.tool->output_args()};
		if(args) {
			for(const core::serializable &child : *args) {
				const ucstring &arg_name{child.get_name()};
				const core::serializable *arg{options.get_child(arg_name)};
				if(arg) {
					const core::univalue &value{arg->get_value()};
					output_path = as_string<pstring>(value);
					break;
				}
			}
		}
		
		if(output_path.empty()) {
			output_path = info.tool->output_default_path();
		}
		
		if(!output_path.empty()) {
			filesys.create_directories({output_path});
		}
		
		str.insert(0, info.base_args);
		
		if(printcmdline) {
			log().info(str);
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
				
				ucstring filepath_str{as_string<ucstring>(unity_file_path)};
				if(!info.drive.empty()) {
					filepath_str.insert(0, info.drive);
				}
				
				cmd.file = filepath_str;
				cmd.command = command;
			} else {
				for(const file_reference *file : files) {
					compile_command_t::command_t &cmd{compile.files.emplace_back()};
					
					ucstring filepath_str{as_string<ucstring>(file->path())};
					if(!info.drive.empty()) {
						filepath_str.insert(0, info.drive);
					}
					
					cmd.file = filepath_str;
					cmd.command = command;
				}
			}
		}

		if(!unity_build && files.size() == 1) {
			vars.file = files[0];
		}

		if(!multi_process()) {
			proc_info_t proc_info{};
			proc_info.setup(move(vars), str, info);
			
			if(!proc_info.start(log())) {
				return false;
			}
			
			return process_done(proc_info.proc, vars, info);
		} else {
			proc_info_t &proc_info{processes.emplace_back()};
			proc_info.setup(move(vars), str, info);
			
			if(!hit_process_limit()) {
				if(!proc_info.start(log())) {
					return false;
				}
			}
			
			return true;
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
	
	bool plugin_process::proc_info_t::start(core::log_context &log)
	{
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
			}
		}
		return started;
	}
	
	bool plugin_process::process_done(core::process &proc, const proc_vars_t &vars, const tool_info_t &info)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
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

		if(do_print) {
			if(exit_code == 0) {
				log().set_severity(core::log_severity::success);
			} else {
				log().set_severity(core::log_severity::error);
			}

			const file_reference *file{vars.file};
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
			if(!output_path.empty() && out_sec) {
				if(vars.file) {
					const file_reference *file{vars.file};
					out_sec->add_file(output_path, file->get_flags(), file->filter(), file->get_condition());
				} else {
					out_sec->add_file(output_path, {}, {}, {});
				}
			}
			
			const tool_info_t::implib_t &implib{info.implib};
			if(!implib.folder.empty()) {
				gen_lib_vars_t gen_vars{};
				gen_vars.arch = implib.arch;
				gen_vars.folder = implib.folder;
				gen_vars.output1 = implib.output1;
				gen_vars.output2 = implib.output2;
				
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
		compile_commands.clear();
		toolsinfos.clear();
		compiler.clear();
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
		
		bool running{true};
		tool_info_t &tool_info{toolsinfos.back()};
		
		proc_vec_t::iterator it{processes.begin()};
		while(it != processes.end()) {
			proc_info_t &proc_info{*it};
			core::process &proc{proc_info.proc};
			const proc_vars_t &vars{proc_info.vars};
			
			if(!hit_process_limit()) {
				if(!proc_info.start(log())) {
					err = true;
					return false;
				}
			}
			
			if(!proc_info.started) {
				running = true;
				continue;
			}
			
			if(proc.running()) {
				running = true;
				
				const ucstring &kill_proc{proc_info.vars.tool_info->kill_proc};
				if(!kill_proc.empty() && !proc_info.killed) {
					vector<core::process> procs{};
					if(core::process::find(kill_proc, procs)) {
						for(core::process &proc : procs) {
							proc.kill();
						}
						proc_info.killed = true;
					}
				}
			} else {
				if(!process_done(proc, vars, tool_info)) {
					err = true;
					return false;
				} else {
					processes.erase(it);
					continue;
				}
			}
			
			it++;
		}
		
		if(processes.empty()) {
			running = false;
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
		compile_command_t &compile{compile_commands.emplace_back()};
		
		/*if(compile.path.empty()) {
			compile.path /= solution.get_name();
			compile.path /= project.get_name();
			compile.path /= u8"compile_commands.json"_p;
			
			core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
			compile.path = filesys.resolve({compile.path, name()}, false);
		}*/
		
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