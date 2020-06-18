#include <private/mfw/builder/plugins/plugin_vscode.hpp>
#include <public/mfw/core/filesystem_interface.hpp>

namespace mfw::builder
{
	plugin_vscode::plugin_vscode()
		: base_plugin{u8"vscode"_s}
	{
		info_.process_files_conditions = false;
		info_.process_options_conditions = false;
		info_.process_tools_patterns = true;
		info_.process_sections_args_unmaped = false;
		info_.process_build_set = true;
		info_.process_single_input = false;
		info_.merge_sections_files_options = false;
		info_.process_out_of_date = true;
		info_.ignore_output = true;

		//core::interfaces::filesystem::instance().remove({{}, name()});
	}

	void plugin_vscode::initialize(interfaces::builder_funcs &funcs)
	{
		super::initialize(funcs);
		
		MFW_MESSAGE("TODO delete symlinks")
		/*
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		core::interfaces::file *file{filesys.open_file(, core::open_flags::read)};
		if(!file) {
			return;
		}
		
		size_t siz{0};
		file->read(&siz, 1, sizeof(size_t));
		for(size_t i{0}; i < siz; i++) {
			size_t len{0};
			file->read(&len, 1, sizeof(size_t));
			pchar_t *name{new pchar_t[len]{u8'\0'}};
			file->read(name, len, sizeof(pchar_t));
			pstring path{name};
			delete[] name;
			filesys.remove({path});
		}
		
		delete file;
		*/
	}

	void plugin_vscode::parse_ide(const core::serializable &options, json_workspace_t &json, bool replace)
	{
		const core::serializable *build{options.get_child(u8"build"_sv)};
		if(build) {
			const core::serializable *command{build->get_child(u8"command"_sv)};
			if(command) {
				const core::univalue &value{command->get_value()};
				json.command = as_string<pstring>(value);
				if(replace) {
					builder_funcs().replace_vars(json.command);
				}
			}
		}
		const core::serializable *debug{options.get_child(u8"debug"_sv)};
		if(debug) {
			const core::serializable *type{debug->get_child(u8"type"_sv)};
			if(type) {
				const core::univalue &value{type->get_value()};
				if(value == u8"gdb"_sv) {
					json.debugger = json_project_t::debugger_type::gdb;
				} else if(value == u8"lldb"_sv) {
					json.debugger = json_project_t::debugger_type::lldb;
				}
			}
			const core::serializable *program{debug->get_child(u8"program"_sv)};
			if(program) {
				const core::univalue &value{program->get_value()};
				json.program = as_string<pstring>(value);
				if(replace) {
					builder_funcs().replace_vars(json.program);
				}
				if(json.cwd.empty()) {
					pstring folder{json.program};
					folder.remove_filename();
					json.cwd = move(folder);
					if(replace) {
						builder_funcs().replace_vars(json.cwd);
					}
				}
			}
			const core::serializable *cwd{debug->get_child(u8"cwd"_sv)};
			if(cwd) {
				const core::univalue &value{cwd->get_value()};
				json.cwd = as_string<pstring>(value);
				if(replace) {
					builder_funcs().replace_vars(json.cwd);
				}
			}
			const core::serializable *args{debug->get_child(u8"args"_sv)};
			if(args) {
				for(const core::serializable &arg : *args) {
					const ucstring &name{arg.get_name()};
					ucstring &str{json.args.emplace_back(name)};
					if(replace) {
						builder_funcs().replace_vars(str);
					}
				}
			}
		}
		const core::serializable *include_paths{options.get_child(u8"includes_paths"_sv)};
		if(include_paths) {
			for(const core::serializable &include : *include_paths) {
				const ucstring &name{include.get_name()};
				pstring &str{json.include_paths.emplace_back(name)};
				if(replace) {
					builder_funcs().replace_vars(str);
				}
			}
		}
		const core::serializable *compile_path{options.get_child(u8"compile_commands"_sv)};
		if(compile_path) {
			const core::univalue &value{compile_path->get_value()};
			json.compile_commands = as_string<pstring>(value.get_string());
			if(replace) {
				builder_funcs().replace_vars(json.compile_commands);
			}
		}
	}

	bool plugin_vscode::generate(const solution_reference &solution, const project_reference &project, const core::serializable &section)
	{
		json_project_t &json{json_projects.back()};

		const ucstring &section_name{section.get_name()};

		if(section_name == u8"plugin"_sv) {
			parse_ide(section, json, false);
			//parse_ide(section, solution_json);
		}

		return true;
	}

	namespace __plugin_vscode_internal
	{
		static void normalize_c_standard(ucstring &str, bool cpp, bool gnu)
		{
			if(cpp) {
				if(gnu) {
					str = u8"gnu18"_sv;
				} else {
					str = u8"c18"_sv;
				}
			} else {
				bool iso{str.find(u8"iso9899"_sv) != ucstring::npos};
				
				if(iso) {
					size_t start{8u};
					
					if(str.compare(start, 4, u8"1990"_sv) == 0 ||
						str.compare(start, 6, u8"199409"_sv) == 0) {
						str = u8"c89"_sv;
					} else if(str.compare(start, 4, u8"1999"_sv) == 0 ||
								str.compare(start, 4, u8"199x"_sv) == 0) {
						str = u8"c99"_sv;
					} else if(str.compare(start, 4, u8"2011"_sv) == 0 ||
								str.compare(start, 4, u8"2017"_sv) == 0) {
						str = u8"c11"_sv;
					} else if(str.compare(start, 4, u8"2018"_sv) == 0) {
						str = u8"c18"_sv;
					}
				} else {
					size_t start{gnu ? 3u : 1u};
				
					if(str.compare(start, 2, u8"90"_sv) == 0) {
						str.replace(start, ucstring::npos, u8"89"_sv);
					} else if(str.compare(start, 2, u8"9x"_sv) == 0) {
						str.replace(start, ucstring::npos, u8"99"_sv);
					} else if(str.compare(start, 2, u8"1x"_sv) == 0) {
						str.replace(start, ucstring::npos, u8"11"_sv);
					} else if(str.compare(start, 2, u8"2x"_sv) == 0) {
						str.replace(start, ucstring::npos, u8"18"_sv);
					}
				}
			}
		}
		
		static void normalize_cpp_standard(ucstring &str, bool cpp, bool gnu)
		{
			if(!cpp) {
				if(gnu) {
					str = u8"gnu++98"_sv;
				} else {
					str = u8"c++98"_sv;
				}
			} else {
				size_t start{gnu ? 5u : 3u};
				
				if(str.compare(start, 2, u8"0x"_sv) == 0) {
					str.replace(start, ucstring::npos, u8"11"_sv);
				} else if(str.compare(start, 2, u8"1y"_sv) == 0) {
					str.replace(start, ucstring::npos, u8"14"_sv);
				} else if(str.compare(start, 2, u8"1z"_sv) == 0) {
					str.replace(start, ucstring::npos, u8"17"_sv);
				} else if(str.compare(start, 2, u8"2a"_sv) == 0) {
					str.replace(start, ucstring::npos, u8"20"_sv);
				} else if(str.compare(start, 7, u8"latest"_sv) == 0) {
					str.replace(start, ucstring::npos, u8"20"_sv);
				}
			}
		}
	}

	bool plugin_vscode::generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options)
	{
		json_project_t &json{json_projects.back()};

		const ucstring &section_name{tool_section.get_name()};

		const tool_reference *tool{tool_section.tool()};

		if(section_name == u8"compiler"_sv) {
			pstring tool_path{};
			if(tool->is_shell()) {
				tool_path = tool->shell();
			} else {
				tool_path = tool->path();
			}
			json.compiler_path = tool_path;
			json.compiler_type = get_compiler_info(tool_path);
			solution_json.compiler_path = tool_path;
			solution_json.compiler_type = json.compiler_type;
			json.compiler_options = options;
			const core::serializable *forced_arg{tool->get_child(u8"forced_includes"_sv)};
			if(forced_arg) {
				for(const core::serializable &child : *forced_arg) {
					const core::serializable *forceds{options.get_child(child.get_name())};
					if(forceds) {
						for(const core::serializable &child : *forceds) {
							const ucstring &name{child.get_name()};
							json.force_includes.emplace_back(name);
							//solution_json.force_includes.emplace_back(name);
						}
					}
				}
			}
			const core::serializable *defines_arg{tool->get_child(u8"preprocessor_definitions"_sv)};
			if(defines_arg) {
				for(const core::serializable &child : *defines_arg) {
					const core::serializable *defines{options.get_child(child.get_name())};
					if(defines) {
						for(const core::serializable &child : *defines) {
							ucstring &str{json.defines.emplace_back()};
							str += child.get_name();
							const core::univalue &value{child.get_value()};
							if(!value.empty()) {
								str += u8'=';
								str += value.get_string();
							}
							//solution_json.defines.emplace_back(str);
						}
					}
				}
			}
			const core::serializable *includes_arg{tool->get_child(u8"includes_paths"_sv)};
			if(includes_arg) {
				for(const core::serializable &child : *includes_arg) {
					const core::serializable *includes_paths{options.get_child(child.get_name())};
					if(includes_paths) {
						for(const core::serializable &child : *includes_paths) {
							const ucstring &name{child.get_name()};
							json.include_paths.emplace_back(name);
							solution_json.include_paths.emplace_back(name);
						}
					}
				}
			}
			const core::serializable *standard{tool->get_child(u8"standard"_sv)};
			if(standard) {
				const core::univalue &opt_value{standard->get_value()};
				standard = options.get_child(opt_value.get_string());
				if(standard) {
					const core::univalue &value{standard->get_value()};
					const ucstring &str{value.get_string()};
					bool cpp{
						str.find(u8"c++"_sv) != ucstring::npos ||
						str.find(u8"gnu++"_sv) != ucstring::npos
					};
					bool gnu{str.find(u8"gnu"_sv) != ucstring::npos};
					json.cpp_standard = str;
					json.c_standard = str;
					__plugin_vscode_internal::normalize_c_standard(json.c_standard, cpp, gnu);
					__plugin_vscode_internal::normalize_cpp_standard(json.cpp_standard, cpp, gnu);
				}
			}
			const core::serializable *plugin{tool_section.plugin_section()};
			if(plugin) {
				parse_ide(*plugin, json, true);
				//parse_ide(*plugin, solution_json);
			}
		}

		return true;
	}
	
	bool plugin_vscode::create_symlink(const core::searchpath &from, const core::searchpath &to, bool dir) const
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		
		pstring resolved{filesys.resolve(to, false)};
		if(resolved.empty()) {
			return false;
		}
		
		if(!filesys.create_symlink(from, resolved, dir)) {
			return false;
		}
		
		MFW_MESSAGE("TODO create symlink list")
		//symlinks.emplace_back(move(resolved));
		return true;
	}

	bool plugin_vscode::generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const file_reference &file, const core::serializable &options)
	{
		if(tool_section.get_name() != u8"compiler"_sv) {
			return true;
		}

		json_project_t &json{json_projects.back()};

		pstring link{};
		link /= as_string<pstring>(solution.get_name());
		link /= as_string<pstring>(project.get_name());
		link /= file.filter();

		pstring filepath{file.path()};
		link /= filepath.filename();

		create_symlink({filepath}, {link, name()});

		return true;
	}
	
	void plugin_vscode::process_option(const core::serializable &option, ucstring &str)
	{
		MFW_MESSAGE("TODO get below from tool")
		ucstring drive{};
		
		const core::serializable *flags{option.get_flags()};
		MFW_MESSAGE("TODO get below from tool")
		bool needs_equal{false};
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
				flags->get_child_bool(u8"files"_sv)) {
				needs_values = true;
				is_file = true;
			}
			if(flags->get_child_bool(u8"folder"_sv) ||
				flags->get_child_bool(u8"file"_sv)) {
				is_file = true;
			}
		}
		
		MFW_MESSAGE("TODO get this from tool")
	#if MFW_OS_IS(WINDOWS)
		ucchar_t equal_char{u8':'};
	#else
		ucchar_t equal_char{u8'='};
	#endif
		
		const ucstring &arg_name{option.get_name()};
		if(option.empty()) {
			const core::univalue &value{option.get_value()};
			if(needs_values && value.empty()) {
				return;
			}
			__MFW_QUOTE_STR_NAME(arg_name, quote_arg)
			char8_t sep{u8' '};
			if(no_space) {
				sep = u8'\0';
			} else if(needs_equal) {
				sep = equal_char;
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
					sep = equal_char;
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
	}

	void plugin_vscode::json_workspace_t::write_cpp_properties(json::file &cpp_properties)
	{
		if(solution) {
			cpp_properties.Key(u8"cpp_properties"_sv);
		}

		cpp_properties.StartObject();
		cpp_properties.Key(u8"version"_sv);
		cpp_properties.Int(4);
		cpp_properties.Key(u8"configurations"_sv);
		cpp_properties.StartArray();
		cpp_properties.StartObject();
		cpp_properties.Key(u8"name"_sv);
		if(compiler_type == compiler_info_t::type_t::msvc) {
			cpp_properties.String(u8"Win32"_sv);
		} else if(compiler_type & compiler_info_t::flags_t::unix_) {
			cpp_properties.String(u8"Linux"_sv);
		} else {
			cpp_properties.String(u8"${default}"_sv);
		}
		cpp_properties.Key(u8"cStandard"_sv);
		if(c_standard.empty()) {
			cpp_properties.String(u8"${default}"_sv);
		} else {
			cpp_properties.String(c_standard);
		}
		cpp_properties.Key(u8"cppStandard"_sv);
		if(cpp_standard.empty()) {
			cpp_properties.String(u8"${default}"_sv);
		} else {
			cpp_properties.String(cpp_standard);
		}
		cpp_properties.Key(u8"intelliSenseMode"_sv);
		MFW_MESSAGE("sad hardcode")
		if(compiler_type == compiler_info_t::type_t::msvc) {
			cpp_properties.String(u8"msvc-x64"_sv);
		} else if(compiler_type == compiler_info_t::type_t::gcc) {
			cpp_properties.String(u8"gcc-x64"_sv);
		} else if(compiler_type & compiler_info_t::flags_t::clang) {
			cpp_properties.String(u8"clang-x64"_sv);
		} else {
			cpp_properties.String(u8"${default}"_sv);
		}
		cpp_properties.Key(u8"compileCommands"_sv);
		cpp_properties.String(compile_commands);
		cpp_properties.Key(u8"compilerArgs"_sv);
		cpp_properties.StartArray();
		for(const core::serializable &arg : compiler_options) {
			ucstring str{};
			process_option(arg, str);
			cpp_properties.String(str);
		}
		cpp_properties.EndArray();
		cpp_properties.Key(u8"forcedInclude"_sv);
		cpp_properties.StartArray();
		for(const pstring &include : force_includes) {
			cpp_properties.String(include);
		}
		cpp_properties.EndArray();
		cpp_properties.Key(u8"browse"_sv);
		cpp_properties.StartObject();
		cpp_properties.Key(u8"databaseFilename"_sv);
	#if 1
		pstring database{path};
		database /= u8".vscode/browse.db"_p;
		cpp_properties.String(database);
	#else
		cpp_properties.String(u8""_sv);
	#endif
		cpp_properties.Key(u8"limitSymbolsToIncludedHeaders"_sv);
		cpp_properties.Bool(true);
		cpp_properties.Key(u8"path"_sv);
		cpp_properties.StartArray();
		for(const pstring &include : include_paths) {
			ucstring tmp{as_string<ucstring>(include)};
			tmp += u8"/*"_sv;
			cpp_properties.String(tmp);
		}
		cpp_properties.EndArray();
		cpp_properties.EndObject();
		cpp_properties.Key(u8"defines"_sv);
		cpp_properties.StartArray();
		for(const ucstring &define : defines) {
			cpp_properties.String(define);
		}
		cpp_properties.EndArray();
		cpp_properties.Key(u8"compilerPath"_sv);
		cpp_properties.String(compiler_path);
		cpp_properties.Key(u8"includePath"_sv);
		cpp_properties.StartArray();
		for(const pstring &include : include_paths) {
			cpp_properties.String(include);
		}
		cpp_properties.EndArray();
		cpp_properties.EndObject();
		cpp_properties.EndArray();
		cpp_properties.EndObject();
	}

	void plugin_vscode::json_workspace_t::write_tasks(json::file &tasks)
	{
		if(solution) {
			tasks.Key(u8"tasks"_sv);
		}

		tasks.StartObject();
		tasks.Key(u8"version"_sv);
		tasks.String(u8"2.0.0"_sv);
		tasks.Key(u8"tasks"_sv);
		tasks.StartArray();
		tasks.StartObject();
		tasks.Key(u8"label"_sv);
		tasks.String(u8"build"_sv);
		tasks.Key(u8"type"_sv);
		tasks.String(u8"shell"_sv);
		tasks.Key(u8"command"_sv);
		tasks.String(command);
		if(problem_matcher.empty()) {
			if(compiler_type == compiler_info_t::type_t::msvc) {
				problem_matcher.emplace_back(u8"$msCompile"_s);
			} else if(compiler_type & compiler_info_t::flags_t::unix_) {
				problem_matcher.emplace_back(u8"$gcc"_s);
			}
		}
		if(!problem_matcher.empty()) {
			tasks.Key(u8"problemMatcher"_sv);
			tasks.StartArray();
			for(const ucstring &str : problem_matcher) {
				tasks.String(str);
			}
			tasks.EndArray();
		}
		tasks.Key(u8"group"_sv);
		tasks.StartObject();
		tasks.Key(u8"kind"_sv);
		tasks.String(u8"build"_sv);
		tasks.Key(u8"isDefault"_sv);
		tasks.Bool(true);
		tasks.EndObject();
		tasks.EndObject();
		tasks.EndArray();
		tasks.EndObject();
	}

	void plugin_vscode::json_workspace_t::write_launch(json::file &launch)
	{
		if(solution) {
			launch.Key(u8"launch"_sv);
		}

		launch.StartObject();
		launch.Key(u8"version"_sv);
		launch.String(u8"0.2.0"_sv);
		launch.Key(u8"configurations"_sv);
		launch.StartArray();
		launch.StartObject();
		launch.Key(u8"name"_sv);
		launch.String(u8"launch"_sv);
		launch.Key(u8"program"_sv);
		launch.String(program);
		launch.Key(u8"cwd"_sv);
		launch.String(cwd);
		launch.Key(u8"args"_sv);
		launch.StartArray();
		for(const ucstring &arg : args) {
			launch.String(arg);
		}
		launch.EndArray();
		launch.Key(u8"type"_sv);
		if(debugger == debugger_type::gdb) {
			launch.String(u8"cppdbg"_sv);
		} else if(debugger == debugger_type::lldb) {
			launch.String(u8"cpplldb"_sv);
		} else {
			launch.String(u8"${default}"_sv);
		}
		launch.Key(u8"request"_sv);
		launch.String(u8"launch"_sv);
		launch.Key(u8"stopAtEntry"_sv);
		launch.Bool(false);
		launch.Key(u8"environment"_sv);
		launch.StartArray();
		launch.EndArray();
		launch.Key(u8"externalConsole"_sv);
		launch.Bool(false);
		launch.Key(u8"MIMode"_sv);
		if(debugger == debugger_type::gdb) {
			launch.String(u8"gdb"_sv);
		} else if(debugger == debugger_type::lldb) {
			launch.String(u8"lldb"_sv);
		} else {
			launch.String(u8"${default}"_sv);
		}
		launch.Key(u8"setupCommands"_sv);
		launch.StartArray();
		if(debugger == debugger_type::gdb) {
			launch.StartObject();
			launch.Key(u8"description"_sv);
			launch.String(u8"Enable pretty-printing for gdb"_sv);
			launch.Key(u8"text"_sv);
			launch.String(u8"-enable-pretty-printing"_sv);
			launch.Key(u8"ignoreFailures"_sv);
			launch.Bool(true);
			launch.EndObject();
		}
		launch.EndArray();
		launch.EndObject();
		launch.EndArray();
		launch.EndObject();
	}

	void plugin_vscode::json_workspace_t::write_all()
	{
		StartObject();
		Key(u8"settings"_sv);
		StartObject();
		if(solution) {
			Key(u8"projectManager.projectsLocation"_sv);
			String(path/u8"projects.json"_p);
		} else {
			if(compiler_type & compiler_info_t::flags_t::unix_) {
				Key(u8"clangd.arguments"_sv);
				StartArray();
				for(const core::serializable &arg : compiler_options) {
					ucstring str{};
					process_option(arg, str);
					String(str);
				}
				EndArray();
			}
		}
		Key(u8"files.exclude"_sv);
		StartObject();
		for(const pstring &exclude : exclude_files) {
			Key(exclude);
			Bool(true);
		}
		EndObject();
		EndObject();
		Key(u8"folders"_sv);
		StartArray();
		for(const folder_t &folder : folders) {
			StartObject();
			if(!folder.name.empty()) {
				Key(u8"name"_sv);
				String(folder.name);
			}
			Key(u8"path"_sv);
			String(folder.path);
			EndObject();
		}
		EndArray();

		if(solution) {
			write_launch(*this);
			write_tasks(*this);
			EndObject();
		} else {
			EndObject();
		}
	}

	void plugin_vscode::json_project_t::write_all()
	{
		super::write_all();

		write_cpp_properties(cpp_properties);
		write_launch(launch);
		write_tasks(tasks);
	}

	void plugin_vscode::json_project_t::save()
	{
		super::save();

		pstring cpp_path{path/u8".vscode/c_cpp_properties.json"_p};
		pstring launch_path{path/u8".vscode/launch.json"_p};
		pstring tasks_path{path/u8".vscode/tasks.json"_p};

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		if(filesys.exists({compile_commands})) {
			pstring link{path / compile_commands.filename()};
			MFW_MESSAGE("TODO generate symlink list")
		#if 0
			create_symlink({compile_commands}, {link});
		#else
			filesys.create_symlink({compile_commands}, {link});
		#endif
		}

		cpp_properties.save({cpp_path});
		//launch.save({launch_path});
		//tasks.save({tasks_path});
	}

	void plugin_vscode::json_workspace_t::save()
	{
		pstring code_workspace_file{};
		code_workspace_file /= as_string<pstring>(name);
		code_workspace_file.replace_extension(u8".code-workspace"_p);

		pstring workspace_path{path/code_workspace_file};

		exclude_files.emplace_back(u8"**/.vscode/**"_p);
		exclude_files.emplace_back(u8"**/*.code-workspace"_p);
		if(solution) {
			exclude_files.emplace_back(u8"**/projects.json"_p);
		}
		exclude_files.emplace_back(u8"**/compile_commands.json"_p);

		if(!solution) {
			folder_t &folder{folders.emplace_back()};
			folder.path = path;
			folder.name = name;
		}

		write_all();
		super::save({workspace_path});
	}

	bool plugin_vscode::generate(const solution_reference &solution, const project_reference &project)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		json_project_t &json{json_projects.emplace_back()};

		json.path /= as_string<pstring>(solution.get_name());

		const ucstring &proj_name{project.get_name()};
		json.name = proj_name;
		json.path /= as_string<pstring>(proj_name);

		json.path = filesys.resolve({json.path, name()}, false);

		return true;
	}

	void plugin_vscode::json_workspace_t::clear()
	{
		exclude_files.clear();
		folders.clear();
		compiler_path.clear();
		include_paths.clear();
		defines.clear();
		force_includes.clear();
		compiler_options.clear();
		compile_commands.clear();
		c_standard.clear();
		cpp_standard.clear();
		program.clear();
		cwd.clear();
		args.clear();
		compiler_type.clear();
		problem_matcher.clear();
		command.clear();
		debugger = debugger_type::unknown;
		name.clear();
		path.clear();
	}

	void plugin_vscode::cleanup(cleanup_type_t type)
	{
		if(type == cleanup_type_t::solution) {
			json_projects.clear();
			solution_json.clear();
		}
	}

	bool plugin_vscode::generate(const solution_reference &solution)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		const core::serializable *ide{solution.get_child(u8"plugin"_sv)};
		if(ide) {
			parse_ide(*ide, solution_json, true);
		}

		const ucstring &sln_name{solution.get_name()};
		solution_json.name = sln_name;
		solution_json.path = filesys.resolve({sln_name, name()}, false);

		json::file projects_json{};

		projects_json.StartArray();
		for(json_project_t &proj : json_projects) {

			json_workspace_t::folder_t &folder{solution_json.folders.emplace_back()};
			folder.name = proj.name;
			folder.path = proj.path;

			proj.save();

			projects_json.StartObject();
			projects_json.Key(u8"name"_sv);
			projects_json.String(proj.name);
			projects_json.Key(u8"rootPath"_sv);
			projects_json.String(proj.path);
			projects_json.Key(u8"paths"_sv);
			projects_json.StartArray();
			projects_json.EndArray();
			projects_json.Key(u8"group"_sv);
			projects_json.String(u8""_sv);
			projects_json.Key(u8"enabled"_sv);
			projects_json.Bool(true);
			projects_json.EndObject();
		}
		projects_json.EndArray();

		projects_json.save({solution_json.path/u8"projects.json"_p});

		solution_json.save();

		return true;
	}
}