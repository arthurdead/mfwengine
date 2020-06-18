#include <private/mfw/builder/builder.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/library.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/core/debugging.hpp>
#include <private/mfw/builder/references/option_reference.hpp>
#include <private/mfw/builder/references/output_tool_reference.hpp>

#include <private/mfw/builder/plugins/plugin_process.hpp>
#include <private/mfw/builder/plugins/plugin_vscode.hpp>

namespace mfw::builder
{
	MFW_DECLARE_GLOBAL_ALLOCATOR(builder, builder)
	MFW_DECLARE_LOG_CONTEXT(log_builder, u8"builder/builder"_p)

	builder &builder::instance() {
		return __builder_global_allocator.instance();
	}

	namespace __builder_internal
	{
		using get_plugin_func_t = base_plugin &(*)();

		template <typename T>
		class root_file final : public builder::root_file_base
		{
			core::serializable *allocate_selected() const override { return new T{}; }
		};

		class files final : public core::serializable
		{
			core::serializable *allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const override
			{
				if(depth == 0) {
					return new file_reference{};
				}
				return new core::serializable{};
			}
		};

		static void merge_replace_vars(ucstring &str)
		{
			builder::instance().replace_vars(str);
		}

		static unique_ptr<plugin_process> process_plugin{};
		static unique_ptr<plugin_vscode> vscode_plugin{};
	}

	builder::builder()
	{
		shell_proc.set_shell();
	}

	builder::~builder()
	{

	}

	core::exit_status builder::start()
	{
		ucstring_view help{};
		bool valid{false};

		const core::commandline &cmdline{core::commandline::instance()};

		help = u8R"(
			$(optional,count=1,description="list symbols of file")
			list_symbols
		)"_sv;

		valid = cmdline.validate(help);
		if(!valid) {
			return core::exit_status::fatal;
		}
		
		const core::univalue *list_symbols{cmdline.value(u8"list_symbols"_s)};
		if(list_symbols) {
			const ucstring &path{list_symbols->get_string()};
			
			core::library::export_vec_t exports{};
			if(!core::library::get_library_exports({path}, exports)) {
				log_builder().error(u8"invalid file"_sv);
				return core::exit_status::fatal;
			}
			
			for(const core::library::export_t &sym : exports) {
				log_builder().info(sym.name);
			}
			
			return core::exit_status::success;
		}

		help = u8R"(
			$(optional,min=1,description="list of sections to build")
			sections
			
			$(optional,min=1,description="list of projects to build")
			projects

			$(optional,count=0,description="regens cache")
			regen_cache
			
			$(optional,count=0,description="ignores cache")
			no_cache
			
			$(optional,count=0,description="ignores timestamps")
			no_stamps
			
			$(optional,count=0,description="debug")
			debug
			
			$(required,min=1,description="list of plugins to use while building")
			plugins
			
			$(required,count=1,description="root path to mfwbuild files")
			path

			$(required,min=1,description="list of solutions to build")
			solutions
		)"_sv;

		valid = cmdline.validate(help);
		if(!valid) {
			return core::exit_status::fatal;
		}

		pstring root_dir{as_string<pstring>(*cmdline.value(u8"path"_s))};

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		root_dir = filesys.clean({root_dir});

		add_variable(u8"path_folder"_s, as_string<ucstring>(root_dir));
		
		pstring exepath{core::executable_path()};
		add_variable(u8"mfwbuilder"_s, as_string<ucstring>(exepath));

		filesys.add_searchpath({root_dir, u8"path"_sv});
		filesys.add_searchpath({u8"mfwbuild"_p, u8"mfwbuild"_sv}, {{}, u8"path"_sv});
		filesys.add_searchpath({u8"projects"_p, u8"projects"_sv}, {{}, u8"mfwbuild"_sv});
		filesys.add_searchpath({u8"solutions"_p, u8"solutions"_sv}, {{}, u8"mfwbuild"_sv});
		filesys.add_searchpath({u8"tools"_p, u8"tools"_sv}, {{}, u8"mfwbuild"_sv});
		filesys.add_searchpath({u8"data"_p, u8"data"_sv}, {{}, u8"mfwbuild"_sv});
		filesys.add_searchpath({u8"builder"_p, u8"builder"_sv}, {{}, u8"data"_sv});
		filesys.add_searchpath({u8"plugins"_p, u8"plugins"_sv}, {{}, u8"mfwbuild"_sv});

		add_variable(u8"data_folder"_s, as_string<ucstring>(root_dir/u8"mfwbuild/data"_p));

		filesys.set_working_dir({{}, u8"data"_sv});

		regen_cache_ = cmdline.get_bool(u8"regen_cache"_s);
		no_cache_ = cmdline.get_bool(u8"no_cache"_s);
		no_stamps_ = cmdline.get_bool(u8"no_stamps"_s);
		debugging_ = cmdline.get_bool(u8"debug"_s);
		
		if(!parse_plugins()) {
			return core::exit_status::fatal;
		}
		
		/*ucstring tmp_help{};
		
		for(base_plugin *plugin : plugins) {
			tmp_help += u8"$if plugins."_sv;
			tmp_help += plugin->name();
			tmp_help += u8'\n';
			plugin->insert_help(tmp_help);
			tmp_help += u8"$endif"_sv;
		}
		
		if(!tmp_help.empty()) {
			valid = cmdline.validate(tmp_help);
			if(!valid) {
				return core::exit_status::fatal;
			}
		}*/

		const vector<core::univalue> *values_sections{cmdline.values(u8"sections"_s)};
		if(values_sections) {
			for(const core::univalue &value : *values_sections) {
				selected_sections.emplace_back(value.get_string());
			}
		}
		
		const vector<core::univalue> *values_projects{cmdline.values(u8"projects"_s)};
		if(values_projects) {
			for(const core::univalue &value : *values_projects) {
				selected_projects.emplace_back(value.get_string());
			}
		}

		if(!parse_solutions()) {
			return core::exit_status::fatal;
		}

		if(!generate_solutions()) {
			return core::exit_status::fatal;
		}

		return core::exit_status::success;
	}

	bool builder::parse_plugin(const ucstring &name)
	{
		core::library *lib{core::library::load_library({name, u8"plugins"_sv})};
		if(!lib || !lib->valid()) {
			return false;
		}

		size_t syms{lib->symbol_count()};
		if(syms == 0) {
			return false;
		}

		for(size_t i{0}; i < syms; i++) {
			const ucstring &symname{lib->symbol_name(i)};

			if(!matches_pattern(symname, u8"*__*_get_plugin"_sv)) {
				continue;
			}

			__builder_internal::get_plugin_func_t func{lib->symbol<__builder_internal::get_plugin_func_t>(i)};
			if(!func) {
				return false;
			}

			base_plugin &plugin{func()};
			plugins.emplace_back(&plugin);
		}

		return true;
	}

	bool builder::parse_plugins()
	{
		const core::commandline &cmdline{core::commandline::instance()};

		const vector<core::univalue> *values_plugins{cmdline.values(u8"plugins"_s)};
		if(!values_plugins || values_plugins->empty()) {
			return false;
		}

		for(const core::univalue &value : *values_plugins) {
			const ucstring &name{value.get_string()};

			if(name == u8"process"_sv) {
				__builder_internal::process_plugin.reset(new plugin_process{});
				plugins.emplace_back(__builder_internal::process_plugin.get());
			} else if(name == u8"vscode"_sv) {
				__builder_internal::vscode_plugin.reset(new plugin_vscode{});
				plugins.emplace_back(__builder_internal::vscode_plugin.get());
			} else {
				bool found{false};
				for(base_plugin *plugin : plugins) {
					if(plugin->name() == name) {
						found = true;
						break;
					}
				}
				if(found) {
					continue;
				}

				if(!parse_plugin(name)) {
					return false;
				}
			}
		}

		return !plugins.empty();
	}

	bool builder::save_cached_file_internal(const ucstring &name, const pstring &filter, cache_type type, const core::serializable &main_section)
	{
		core::searchpath cache_search{{}, u8"builder"_sv};
		if(type == cache_type::solution) {
			cache_search.path /= u8"solutions"_p;
		} else if(type == cache_type::project) {
			cache_search.path /= u8"projects"_p;
		} else if(type == cache_type::tool) {
			cache_search.path /= u8"tools"_p;
		}
		cache_search.path /= filter;
		cache_search.path /= name;
		cache_search.path.replace_extension(u8".cache.sr"_p);

		if(debugging_) {
			log_builder().info(u8"saving {} cache"_sv, name);
		}

		return main_section.to_file(cache_search);
	}

	bool builder::save_cached_file_project(const ucstring &name, const pstring &filter, const core::serializable &main_section)
	{
		return save_cached_file_internal(name, filter, cache_type::project, main_section);
	}

	bool builder::open_cached_file_project(const ucstring &name, const pstring &filter, root_file_base &root_file, bool &cached)
	{
		return open_cached_file_internal(name, filter, cache_type::project, root_file, cached);
	}

	bool builder::save_cached_file_solution(const ucstring &name, const core::serializable &main_section)
	{
		return save_cached_file_internal(name, {}, cache_type::solution, main_section);
	}

	bool builder::open_cached_file_solution(const ucstring &name, root_file_base &root_file, bool &cached)
	{
		return open_cached_file_internal(name, {}, cache_type::solution, root_file, cached);
	}

	bool builder::save_cached_file_tool(const ucstring &name, const core::serializable &main_section)
	{
		return save_cached_file_internal(name, {}, cache_type::tool, main_section);
	}

	bool builder::open_cached_file_tool(const ucstring &name, root_file_base &root_file, bool &cached)
	{
		return open_cached_file_internal(name, {}, cache_type::tool, root_file, cached);
	}

	bool builder::open_cached_file_internal(const ucstring &name, const pstring &filter, cache_type type, root_file_base &root_file, bool &cached)
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

		pstring resolved{};
		if(type == cache_type::solution) {
			resolved = filesys.resolve({name+u8".sr"_s, u8"solutions"_sv});
		} else if(type == cache_type::project) {
			resolved = filesys.resolve({name+u8".sr"_s, u8"projects"_sv});
		} else if(type == cache_type::tool) {
			resolved = filesys.resolve({name+u8".sr"_s, u8"tools"_sv});
		}
		if(resolved.empty()) {
			return false;
		}

		core::searchpath cache_search{{}, u8"builder"_sv};
		if(type == cache_type::solution) {
			cache_search.path /= u8"solutions"_p;
		} else if(type == cache_type::project) {
			cache_search.path /= u8"projects"_p;
		} else if(type == cache_type::tool) {
			cache_search.path /= u8"tools"_p;
		}
		cache_search.path /= filter;
		cache_search.path /= name;
		cache_search.path.replace_extension(u8".cache.sr"_p);
		
		if(regen_or_no_cache()) {
			cached = false;
		} else {
			if(filesys.exists(cache_search)) {
				cached = true;
			} else {
				cached = false;
			}
		}
		
		root_file.which_ = name;

		core::searchpath timestamp_dir{{}, u8"builder"_sv};
		if(type == cache_type::solution) {
			timestamp_dir.path /= u8"solutions"_p;
		} else if(type == cache_type::project) {
			timestamp_dir.path /= u8"projects"_p;
		} else if(type == cache_type::tool) {
			timestamp_dir.path /= u8"tools"_p;
		}

		core::searchpath output{timestamp_dir};
		output.path /= resolved.filename();
		output.path.concat(u8".timestamp.bin"_s);

		if((regen_cache_ || !filesys.exists({output})) && !no_cache_) {
			if(debugging_) {
				log_builder().info(u8"generating timestamp for {}"_sv, name);
			}
			
			if(!timestamp_builder.generate({resolved}, output)) {
				return false;
			}
		} else {
			if(cached) {
				if(debugging_) {
					log_builder().info(u8"checking if {} changed"_sv, name);
				}
				
				if(timestamp_builder.check(output)) {
					cached = false;
				}
			}
		}

		if(cached) {
			if(debugging_) {
				log_builder().info(u8"loading {} from cache"_sv, name);
			}
			
			if(root_file.from_file(cache_search)) {
				base_cached_file &main_section{*reinterpret_cast<base_cached_file *>(root_file.get_child(name))};
				main_section.loaded_from_cache_ = true;
			} else {
				return false;
			}
		} else {
			if(debugging_) {
				log_builder().info(u8"loading {}"_sv, name);
			}
			
			if(!root_file.from_file({resolved})) {
				return false;
			}
		}

		return true;
	}

	bool builder::parse_solutions()
	{
		const core::commandline &cmdline{core::commandline::instance()};

		const vector<core::univalue> *values_solutions{cmdline.values(u8"solutions"_s)};
		if(!values_solutions || values_solutions->empty()) {
			return false;
		}

		for(const core::univalue &value : *values_solutions) {
			const ucstring &name{value.get_string()};

			__builder_internal::root_file<solution_reference> solution_root{};

			bool cached{true};
			if(!open_cached_file_solution(name, solution_root, cached)) {
				return false;
			}

			solution_reference &solution_main{reinterpret_cast<solution_reference &>(*solution_root.get_child(name))};

			if(!parse_solution(solution_main)) {
				return false;
			}

			if(!cached && !no_cache_) {
				save_cached_file_solution(name, solution_main);
			}

			solutions.emplace_back(move(solution_main));
		}

		return true;
	}

	bool builder::parse_tool(tool_reference &tool)
	{
		if(tool.loaded_from_cache_) {
			return true;
		}
		
		#define __MFW_BUILDER_ADD_SELF_OPTION(name) \
			core::serializable &name{tool.child(MFW_MACRO_CONCATENATE(u8, MFW_MACRO_CONCATENATE(#name, _sv)))}; \
			name.child(MFW_MACRO_CONCATENATE(u8, MFW_MACRO_CONCATENATE(#name, _sv))); \
		
		MFW_MESSAGE("get rid of this when i am not lazy")
		__MFW_BUILDER_ADD_SELF_OPTION(includes_paths)
		__MFW_BUILDER_ADD_SELF_OPTION(preprocessor_definitions)
		__MFW_BUILDER_ADD_SELF_OPTION(forced_includes)
		__MFW_BUILDER_ADD_SELF_OPTION(output)
		
		const core::serializable *shell{tool.get_child(u8"shell"_sv)};
		if(shell) {
			const core::univalue &value{shell->get_value()};
			pstring filename{core::process::get_path(value.get_string())};
			if(!filename.empty()) {
				core::serializable &path{tool.child(u8"path"_sv)};
				if(!path.has_value()) {
					path.set_value(as_string<ucstring>(filename));
					tool.erase(u8"shell"_sv);
				}
			}
		}
		
		core::serializable *subprocess{tool.get_child(u8"subprocess"_sv)};
		if(subprocess) {
			shell = subprocess->get_child(u8"shell"_sv);
			if(shell) {
				const core::univalue &value{shell->get_value()};
				pstring filename{core::process::get_path(value.get_string())};
				if(!filename.empty()) {
					core::serializable &path{subprocess->child(u8"path"_sv)};
					if(!path.has_value()) {
						path.set_value(as_string<ucstring>(filename));
						subprocess->erase(u8"shell"_sv);
					}
				}
			}
		}

		return true;
	}

	bool builder::load_tool(const ucstring &name, const solution_reference *solution)
	{
		if(is_hardcoded_tool(name)) {
			return false;
		}

		__builder_internal::root_file<tool_reference> tool_root{};

		bool cached{true};
		if(!open_cached_file_tool(name, tool_root, cached)) {
			return false;
		}

		tool_reference *tool_main{reinterpret_cast<tool_reference *>(tool_root.get_child(name))};
		if(!tool_main) {
			return false;
		}

		if(!parse_tool(*tool_main)) {
			return false;
		}

		if(!cached && !no_cache_) {
			save_cached_file_tool(name, *tool_main);
		}

		tools.emplace_back(move(*tool_main));
		return true;
	}

	bool builder::parse_solution(solution_reference &solution)
	{
		if(!solution.loaded_from_cache_) {
			add_variable(u8"solution_name"_s, solution.get_name());
		}
		
		const core::serializable *sln_tools{solution.get_child(u8"tools"_sv)};
		if(!sln_tools) {
			return false;
		}

		for(const core::serializable &child : *sln_tools) {
			const ucstring &value{child.get_value().get_string()};

			const tool_reference *tool{find_or_load_tool(value, &solution)};
			if(!tool) {
				return false;
			}
			
			const ucstring &name{child.get_name()};

			const ucstring &cond{child.get_condition()};
			if(!cond.empty()) {
				solution_reference::tool_info_t &info{solution.tools_.emplace_back()};
				info.tool = tool;
				info.condition = cond;
				info.name = name;
			} else {
				add_variable(name, value);
			}
		}
		
		if(!solution.loaded_from_cache_) {
			const builder_section_reference *sec{solution.builder_section()};
			if(sec) {
				generate_builder_section(*sec, builder_sec_type::solution);
			}
		}

		const core::serializable *projects{solution.get_child(u8"projects"_sv)};
		if(!projects) {
			return false;
		}

		if(!parse_projects(*projects, {}, solution)) {
			return false;
		}

		return true;
	}
	
	bool builder::process_dependency(const core::serializable &src, core::serializable &dst, bool self)
	{
		for(const core::serializable &it : src) {
			ucstring condition{it.get_condition()};
			
			if(!condition.empty()) {
				MFW_MESSAGE("this is really sad i need to make a expression optmizer")
				if(self) {
					replace_all(condition, u8"is_self"_sv, u8"true"_sv);
				} else {
					replace_all(condition, u8"is_self"_sv, u8"false"_sv);
				}
				if(condition == u8"false"_sv ||
					condition.find(u8"!true"_sv) != ucstring::npos) {
					continue;
				} else if(condition == u8"true"_sv ||
							condition == u8"!false"_sv) {
					condition.clear();
				} else {
					replace_all(condition, u8"!false && "_sv, {});
					if(replace_all(condition, u8"(!false) && ("_sv, {})) {
						condition.pop_back();
					}
				}
			}
			
			ucstring name{it.get_name()};
			replace_vars(name);
			core::univalue value{it.get_value()};
			replace_vars(value);
			
			core::serializable &child{dst.child(name)};
			child.set_value(value);
			child.set_condition(condition);
			
			if(!process_dependency(it, child, self)) {
				return false;
			}
		}
		
		return true;
	}

	bool builder::parse_dependency(project_reference &project, const project_reference &other)
	{
		bool self{&project == &other};
		
		const builder_section_reference *sec{other.builder_section()};
		if(sec) {
			MFW_MESSAGE("remove dependency later")
			const core::serializable *dependency{sec->get_child(u8"dependency"_sv)};
			if(dependency) {
				ucstring other_name{other.get_name()};
				add_variable(u8"dependency_name"_s, other_name);
				to_upper(other_name, other_name);
				add_variable(u8"dependency_name_upper"_s, other_name);
				add_variable(u8"dependency_filter"_s, as_string<ucstring>(other.filter()));
				
				core::serializable parsed{};
				if(!process_dependency(*dependency, parsed, self)) {
					return false;
				}
				
				project.merge(parsed, false);
			}
		}
		return true;
	}

	bool builder::parse_dependencies(project_reference &project, const project_reference &other)
	{
		if(!parse_dependency(project, other)) {
			return false;
		}
		
		const ucstring &proj_name{project.get_name()};

		for(const project_reference *it : other.depends) {
			const ucstring &name{it->get_name()};
			if(name == proj_name) {
				continue;
			}
			if(!parse_dependencies(project, *it)) {
				return false;
			}
		}

		return true;
	}

	project_reference *builder::find_or_load_project(const ucstring &name, const pstring &filter, const ucstring &condition, solution_reference &solution)
	{
		ptr_vector<project_reference> &projects{solution.projects_};
		for(project_reference &proj : projects) {
			if(proj.get_name() == name) {
				return &proj;
			}
		}

		return nullptr;//load_project(name, filter, condition, solution);
	}

	bool builder::parse_builder_section(const builder_section_reference &sec, project_reference &project, solution_reference &solution)
	{
		if(!project.loaded_from_cache_) {
			if(!parse_dependency(project, project)) {
				return false;
			}
		}

		const ucstring &proj_name{project.get_name()};

		const core::serializable *depends{sec.get_child(u8"depends"_sv)};
		if(depends) {
			for(const core::serializable &it : *depends) {
				const ucstring &name{it.get_name()};
				if(name == proj_name) {
					continue;
				}
				if(!it.passes_condition(this)) {
					continue;
				}
				project_reference *other{find_or_load_project(name, {}, {}, solution)};
				if(!other) {
					log_builder().error(u8"project {} missing dependency {}"_sv, project.get_name(), name);
					return false;
				} else {
					project.depends.emplace_back(other);
					if(!project.loaded_from_cache_) {
						if(!parse_dependencies(project, *other)) {
							return false;
						}
					}
				}
			}
		}

		return true;
	}
	
	project_reference *builder::load_project(const ucstring &name, const pstring &filter, const ucstring &condition, solution_reference &solution)
	{
		__builder_internal::root_file<project_reference> project_root{};

		const ucstring &solution_name{solution.get_name()};
		pstring cache_filter{solution_name/filter};

		bool cached{true};
		if(!open_cached_file_project(name, cache_filter, project_root, cached)) {
			return nullptr;
		}
		
		project_reference &project_main{reinterpret_cast<project_reference &>(*project_root.get_child(name))};
		project_main.set_value(as_string<core::univalue>(filter));
		project_main.set_condition(condition);
		
		if(!parse_project(project_main, solution)) {
			return nullptr;
		}

		if(!cached && !no_cache_) {
			save_cached_file_project(name, cache_filter, project_main);
		}

		return &solution.projects_.emplace_back(move(project_main));
	}

	bool builder::parse_projects(const core::serializable &projects, const pstring &filter, solution_reference &solution)
	{
		for(const core::serializable &child : projects) {
			const ucstring &name{child.get_name()};
			const ucstring &condition{child.get_condition()};

			if(!child.empty()) {
				if(!parse_projects(child, (filter/name), solution)) {
					return false;
				}
				continue;
			}
			
			if(!load_project(name, filter, condition, solution)) {
				return false;
			}
		}

		return true;
	}

	bool builder::parse_project(project_reference &project, solution_reference &solution)
	{
		if(!project.loaded_from_cache_) {
			ucstring proj_name{project.get_name()};
			add_variable(u8"project_name"_s, proj_name);
			to_upper(proj_name, proj_name);
			add_variable(u8"project_name_upper"_s, proj_name);
			add_variable(u8"project_filter"_s, as_string<ucstring>(project.filter()));
		}
		
		if(!project.loaded_from_cache_ && solution.loaded_from_cache_) {
			const builder_section_reference *sec{solution.builder_section()};
			if(sec) {
				generate_builder_section(*sec, builder_sec_type::solution);
			}
		}
		
		const builder_section_reference *sec{project.builder_section()};
		if(sec) {
			if(!project.loaded_from_cache_) {
				generate_builder_section(*sec, builder_sec_type::project);
			}
			
			if(!parse_builder_section(*sec, project, solution)) {
				return false;
			}
		}

		for(core::serializable &child : project) {
			tool_section_reference &tool_section_main{reinterpret_cast<tool_section_reference &>(child)};

			if(!parse_tool_section(tool_section_main, project, solution)) {
				return false;
			}
		}

		return true;
	}

	const tool_reference *builder::find_or_load_tool(const ucstring_view &name, const solution_reference *solution)
	{
		if(is_hardcoded_tool(name)) {
			return nullptr;
		}

		ucstring real_name{name};

		if(solution) {
			const core::serializable *sln_tools{solution->get_child(u8"tools"_sv)};
			if(sln_tools) {
				const core::serializable *tool{sln_tools->get_child(name)};
				if(tool) {
					const core::univalue &value{tool->get_value()};
					real_name = value.get_string();
				}
			}
		}

		for(const tool_reference &tool : tools) {
			if(tool.get_name() == real_name) {
				return &tool;
			}
		}

		if(!load_tool(real_name, solution)) {
			return nullptr;
		}

		return &tools.back();
	}

	bool builder::is_hardcoded_tool(const ucstring_view &name)
	{
		if(name == u8"plugin"_sv ||
			name == u8"builder"_sv) {
			return true;
		}

		return false;
	}

	void builder::parse_folders(const core::serializable &folders, core::serializable *target, bool remove, core::serializable *removed, core::serializable *parent)
	{
		for(const core::serializable &child : folders) {
			ucstring name{child.get_name()};
			replace_vars(name);

			const ucstring &value{child.get_value().get_string()};

			bool tmp_remove{remove};

			if(!child.empty()) {
				if(name == u8"remove"_sv) {
					parse_folders(child, target, true, removed, parent);
					continue;
				}
			} else {
				if(value == u8"remove"_sv) {
					tmp_remove = true;
				}
			}
			
			if(began_gen) {
				if(!child.passes_condition(this)) {
					continue;
				}
			}

			core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
			
			vector<pstring> files_paths{};

			pstring name_path{filesys.resolve({name}, false)};
			if(!name_path.empty()) {
				filesys.glob({name_path}, files_paths);
				
				if(files_paths.empty()) {
					files_paths.emplace_back(name_path);
				}
			} else {
				files_paths.emplace_back(as_string<pstring>(name));
			}

			const ucstring &condition{child.get_condition()};

			for(pstring &path : files_paths) {
				if(!filesys.is_directory({path})) {
					continue;
				}
				ucstring str{as_string<ucstring>(path)};
				if(tmp_remove) {
					target->erase(str);
					if(!removed) {
						removed = &parent->child(u8"removed"_sv);
					}
					removed->child(str);
				} else if(removed && removed->get_child(str)) {
					continue;
				} else {
					core::serializable &file_main{target->child(str)};
					file_main.set_condition(condition);
				}
			}
		}
	}

	void builder::parse_files(const core::serializable &files, core::serializable *target, const pstring &filter, parse_files_flags flags, core::serializable *removed, core::serializable *parent, tool_section_reference &tool_section)
	{
		for(const core::serializable &child : files) {
			ucstring name{child.get_name()};
			replace_vars(name);
			ucstring value{child.get_value().get_string()};
			replace_vars(value);

			parse_files_flags tmp_flags{flags};

			if(!child.empty()) {
				if(value == u8"folder"_sv || value == u8"filter"_sv) {
					parse_files(child, target, (filter/name), flags, removed, parent, tool_section);
					continue;
				} else if(name == u8"remove"_sv) {
					parse_files(child, target, filter, flags|parse_files_flags::remove, removed, parent, tool_section);
					continue;
				} else if(name == u8"delay"_sv) {
					parse_files(child, target, filter, flags|parse_files_flags::delay, removed, parent, tool_section);
					continue;
				} else if(name == u8"dynamic"_sv || name == u8"force"_sv) {
					parse_files(child, target, filter, flags|parse_files_flags::dynamic, removed, parent, tool_section);
					continue;
				}
			}
			
			if(began_gen) {
				if(!child.passes_condition(this)) {
					continue;
				}
			}
			
			if(value == u8"remove"_sv) {
				tmp_flags |= parse_files_flags::remove;
			} else if(value == u8"delay"_sv) {
				tmp_flags |= parse_files_flags::delay;
			} else if(value == u8"dynamic"_sv || value == u8"force"_sv) {
				tmp_flags |= parse_files_flags::dynamic;
			} else if(value == u8"folder"_sv || value == u8"filter"_sv) {
				continue;
			}

			core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};

			vector<pstring> files_paths{};

			pstring name_path{filesys.resolve({name}, false)};
			if(!name_path.empty()) {
				filesys.glob({name_path}, files_paths);
				
				if(files_paths.empty()) {
					MFW_MESSAGE("this adds globs which is not good")
					files_paths.emplace_back(name_path);
				}
			} else {
				files_paths.emplace_back(as_string<pstring>(name));
			}
			
			const ucstring &condition{child.get_condition()};

			for(pstring &path : files_paths) {
				if(filesys.is_directory({path})) {
					continue;
				}
				ucstring str{as_string<ucstring>(path)};
				if(bool_cast(tmp_flags & parse_files_flags::remove)) {
					target->erase(str);
					if(!removed) {
						removed = &parent->child(u8"removed"_sv);
					}
					removed->child(str);
				} else if(removed && removed->get_child(str)) {
					continue;
				} else {
					file_reference &file_main{reinterpret_cast<file_reference &>(target->child(str))};
					
					pstring tmp_filter{};
					
					size_t pos{name.find(u8'*')};
					if(pos != ucstring::npos) {
						pstring junk{as_string<pstring>(name.substr(0, pos))};
						junk = filesys.clean({junk});
						replace_all(str, as_string<ucstring>(junk), {});
						str.erase(str.cbegin());
						tmp_filter = filter;
						tmp_filter /= str;
						tmp_filter.remove_filename();
					}
					
					if(tmp_filter.empty()) {
						tmp_filter = filter;
					}
					
					if(bool_cast(tmp_flags & parse_files_flags::delay)) {
						file_main.add_flag(u8"delay"_sv);
					}
					if(bool_cast(tmp_flags & parse_files_flags::dynamic)) {
						file_main.add_flag(u8"dynamic"_sv);
					}
					
					file_main.set_value(as_string<core::univalue>(tmp_filter));
					file_main.set_condition(condition);
					file_main.merge(child, true, __builder_internal::merge_replace_vars);
				}
			}
		}
	}

	void builder::parse_options(core::serializable &options, tool_section_reference &tool_section)
	{
		const tool_reference *tool{tool_section.tool()};
		core::serializable newoptions{};
		base_plugin::plugin_info_t info{};
		process_options(tool, options, newoptions, info);
		options.remove_all();
		options.merge(newoptions);
	}

	bool builder::parse_tool_section(tool_section_reference &tool_section, project_reference &project, const solution_reference &solution)
	{
		const ucstring &tool_name{tool_section.get_name()};

		if(!is_hardcoded_tool(tool_name)) {
			tool_section.tool_ = find_or_load_tool(tool_name, &solution);
			if(!tool_section.tool_) {
				log_builder().error(u8"tool not found: {}"_sv, tool_name);
				return false;
			}
			
			output_tool_reference *output_tool{const_cast<output_tool_reference *>(tool_section.output_tool())};
			if(output_tool) {
				const ucstring &name{output_tool->get_value().get_string()};
				output_tool->tool_ = find_or_load_tool(name, &solution);
			}
		}

		if(!project.loaded_from_cache_) {
			const tool_reference *tool{tool_section.tool_};
			
			if(solution.tools_.empty()) {
				if(tool) {
					const ucstring &tool_name{tool->get_name()};
					add_variable(u8"tool_name"_s, tool_name);
				}
				
				const output_tool_reference *outputtool{tool_section.output_tool()};
				if(outputtool) {
					const tool_reference *tool_output{outputtool->tool()};
					if(tool_output) {
						const ucstring &output_tool_name{tool_output->get_name()};
						add_variable(u8"output_tool_name"_s, output_tool_name);
					}
				}
			}
			
			const builder_section_reference *builder_sec{tool_section.builder_section()};
			if(builder_sec) {
				generate_builder_section(*builder_sec, builder_sec_type::tool_section);
			}
			
			if(tool) {
				builder_sec = tool->builder_section();
				if(builder_sec) {
					generate_builder_section(*builder_sec, builder_sec_type::tool);
				}
			}
			
			if(solution.tools_.empty()) {
				core::serializable *options{tool_section.get_child(u8"options"_sv)};
				if(options) {
					parse_options(*options, tool_section);
				}
			}
		}

		if(tool_section.tool_) {
			core::serializable *options{tool_section.get_child(u8"options"_sv)};
			if(options) {
				const core::serializable *includes_arg{tool_section.tool_->get_child(u8"includes_paths"_sv)};
				if(includes_arg) {
					for(const core::serializable &child : *includes_arg) {
						core::serializable *includes_paths{options->get_child(child.get_name())};
						if(includes_paths) {
							for(core::serializable &child : *includes_paths) {
								pstring path{};
								if(!project.loaded_from_cache_) {
									ucstring name{child.get_name()};
									replace_vars(name);
									child.set_name(name);
									path = as_string<pstring>(name);
								} else {
									const ucstring &name{child.get_name()};
									path = as_string<pstring>(name);
								}
								timestamp_builder.add_include_dir({path});
							}
						}
					}
				}
			}
		}
		
		bool no_timestamp{
			no_stamps_ ||
			tool_section.get_value() == u8"ignore"_sv
		};
		
		core::searchpath timestamp_dir{{}, u8"builder"_sv};
		timestamp_dir.path /= u8"projects"_p;
		timestamp_dir.path /= project.get_name();
		timestamp_dir.path /= tool_section.get_name();

		if(!project.loaded_from_cache_) {
			tool_section.out_of_date_ = true;

			core::serializable *files{tool_section.get_child(u8"files"_sv)};
			if(files) {
				__builder_internal::files tmp{};

				core::serializable &flags{files->flags()};

				parse_files(*files, &tmp, {}, parse_files_flags::none, flags.get_child(u8"removed"_sv), &flags, tool_section);

				files->remove_all();

				files->merge(tmp);

				for(core::serializable &child : *files) {
					file_reference &file_main{reinterpret_cast<file_reference &>(child)};
					file_main.flags_ |= file_reference::flags::out_of_date;

					if(!child.empty() && solution.tools_.empty()) {
						pstring filter{file_main.filter()};
						add_variable(u8"file_filter"_s, as_string<ucstring>(filter));

						pstring fullpath{file_main.path()};
						pstring filename{fullpath.filename()};
						pstring basename{filename};
						basename.replace_extension();

						add_variable(u8"file_basename"_s, as_string<ucstring>(basename));
						
						parse_options(child, tool_section);
						
						remove_variable(u8"file_filter"_s);
						remove_variable(u8"file_basename"_s);
					}
				}
			}
			
			remove_variable(u8"tool_name"_s);
			remove_variable(u8"output_tool_name"_s);
		} else {
			tool_section.out_of_date_ = false;

			core::serializable *files{tool_section.get_child(u8"files"_sv)};
			if(files) {
				for(core::serializable &child : *files) {
					file_reference &file_main{reinterpret_cast<file_reference &>(child)};
					
					if(no_timestamp || regen_or_no_cache()) {
						file_main.flags_ |= file_reference::flags::out_of_date;
					}
				}
			}
		}

		return true;
	}
	
	bool builder::file_changed(const pstring &fullpath, const pstring &filter, const core::searchpath &timestamp_dir) const
	{
		core::searchpath output{timestamp_dir};
		output.path /= filter;
		output.path /= fullpath.filename();
		output.path.concat(u8".timestamp.bin"_s);

		if(debugging_) {
			log_builder().info(u8"checking if {} changed"_sv, fullpath);
		}

		return timestamp_builder.check(output);
	}

	pstring builder::get_output_path(const tool_section_reference &tool_section, const core::serializable &options, const core::serializable &file_options, bool merged) const
	{
		const tool_reference *tool{tool_section.tool()};

		pstring path{};

		if(!tool) {
			return path;
		}

		const core::serializable *args{tool->output_args()};
		if(args) {
			const core::univalue *output{nullptr};
			if(merged) {
				output = find_output_option(file_options, *args);
			} else {
				output = find_output_option(file_options, *args);
				if(!output) {
					output = find_output_option(options, *args);
				}
			}
			if(output) {
				const ucstring &str{output->get_string()};
				path = as_string<pstring>(str);
			}
		}
		
		if(path.empty()) {
			path = tool->output_default_path();
		}
		
		if(!merged) {
			replace_vars(path);
		}
		
		return path;
	}

	bool builder::output_exists(const tool_section_reference &tool_section, const core::serializable &options, const core::serializable &file_options, bool merged) const
	{
		pstring path{get_output_path(tool_section, options, file_options, merged)};
		if(path.empty()) {
			return true;
		}

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		if(filesys.exists({path})) {
			return true;
		}

		pstring workdir{filesys.get_working_dir()};
		path = (workdir/path);
		if(filesys.exists({path})) {
			return true;
		}

		return false;
	}

	void builder::replace_vars(ucstring &str) const
	{
		if(str.empty()) {
			return;
		}

		size_t replaced{0};

		for(const macros_t::value_type &it : macros) {
			replaced += replace_all(str, u8"${"_s + it.first + u8'}', it.second);
		}

		if(replaced > 0) {
			replace_vars(str);
		}
	}

	void builder::replace_vars(core::univalue &val) const
	{
		if(val.empty()) {
			return;
		}

		ucstring str{val.get_string()};
		replace_vars(str);
		val = move(str);
	}

	void builder::replace_vars(pstring &file) const
	{
		if(file.empty()) {
			return;
		}

		ucstring str{as_string<ucstring>(file)};
		replace_vars(str);
		file = move(str);
	}

	bool builder::get_variable(const ucstring_view &name, core::type_holder &var) const
	{
		macros_t::const_iterator it{macros.find(ucstring{name})};
		if(it != macros.end()) {
			const ucstring &value{it->second};
			var.deduce(value);
			return true;
		}
		var.deduce(false);
		return true;
	}

	void builder::add_variable(const ucstring &name, const ucstring &value)
	{
		macros[name] = value;
	}

	void builder::remove_variable(const ucstring &name)
	{
		macros_t::const_iterator it{macros.find(name)};
		if(it != macros.end()) {
			macros.erase(it);
		}
	}

	void builder::generate_builder_section(const builder_section_reference &sec, builder_sec_type type)
	{
		const core::serializable *macros{sec.macros()};
		if(macros) {
			for(const core::serializable &it : *macros) {
				if(began_gen) {
					if(!it.passes_condition(this)) {
						continue;
					}
				}
				const ucstring &name{it.get_name()};
				core::univalue value{it.get_value()};
				replace_vars(value);
				add_variable(name, value.get_string());
			}
		}
	}

	bool builder::remap_value(const core::serializable &option, const core::serializable *map, core::serializable &options, remap_result_t &result, const base_plugin::plugin_info_t &info)
	{
		const ucstring &condition{option.get_condition()};
		
		if(began_gen && !info.process_options_conditions) {
			if(!condition.empty()) {
				core::univalue res{};
				if(!core::parse_expression(condition, res, this)) {
					return false;
				}
				
				if(!res.get_bool()) {
					return true;
				}
			}
		}
		
		const core::serializable *opt_flags{option.get_flags()};
		if(opt_flags) {
			if(opt_flags->get_child_bool(u8"remove"_sv)) {
				return true;
			}
		}
		
		ucstring name{option.get_name()};
		replace_vars(name);

		if(map) {
			result.map = map->get_child(name);
		}
		
		const core::serializable *map_flags{nullptr};
		if(result.map) {
			map_flags = result.map->get_flags();
			if(map_flags) {
				if(map_flags->get_child_bool(u8"remove"_sv)) {
					return true;
				}
			}
		}
		
		core::univalue value{option.get_value()};
		replace_vars(value);
		
		if(!result.map || info.process_sections_args_unmaped) {
			result.child = &options.child(name);
		} else if(result.map) {
			core::univalue map_value{result.map->get_value()};
			replace_vars(map_value);
			result.child = &options.child(map_value.get_string());
			
			const core::serializable *mapped_value{result.map->get_child(value.get_string())};
			if(mapped_value) {
				value = mapped_value->get_value();
			}
		}
		
		if(map_flags) {
			result.child->flags().merge(*map_flags);
		}

		result.child->set_value(value);
		result.child->set_condition(condition);
		if(opt_flags) {
			result.child->flags().merge(*opt_flags);
		}
		
		return true;
	}

	bool builder::process_options(const tool_reference *tool, core::serializable &options, core::serializable &mapped_options, const base_plugin::plugin_info_t &info)
	{
		const core::serializable *map{nullptr};
		if(tool) {
			map = tool->options_map();
		}
		
		core::serializable::iterator option_it{options.begin()};
		while(option_it != options.end()) {
			core::serializable &option{*option_it};

			remap_result_t result{};
			if(!remap_value(option, map, mapped_options, result, info)) {
				return false;
			}

			if(result.child) {
				core::serializable &child{*result.child};
				
				bool remap_values{true};
				bool is_folders{false};
				
				core::serializable *flags{child.get_flags()};
				if(flags) {
					if(flags->get_child_bool(u8"folders"_sv)) {
						is_folders = true;
						remap_values = false;
					}
				}
				
				for(const core::serializable &it : option) {
					if(remap_values) {
						remap_result_t tmp{};
						if(!remap_value(it, result.map, child, tmp, info)) {
							return false;
						}
					} else if(!is_folders) {
						if(began_gen && !info.process_options_conditions) {
							if(!it.passes_condition(this)) {
								continue;
							}
						}
						
						child.child(it.get_name());
					}
				}
				
				if(is_folders) {
					__builder_internal::files tmp{};

					parse_folders(option, &tmp, false, flags->get_child(u8"removed"_sv), flags);

					child.merge(tmp);
				}
			}

			option_it++;
		}
		
		return true;
	}

	void builder::generate_pch(const pstring &file, const tool_reference &tool, const project_reference &project, const solution_reference &solution)
	{
		ucstring str{};
		str += u8"#include \""_sv;
		str += as_string<ucstring>(file);
		str += u8'"';

		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		pstring data_folder{filesys.resolve({{}, u8"data"_sv}, false)};
		
		MFW_DEBUGBREAK();

		pstring output{data_folder};
		output /= as_string<pstring>(solution.get_name());
		output /= project.filter();
		output /= as_string<pstring>(project.get_name());

		pstring filename{file.filename()};
		filename.replace_extension(tool.unity_build_ext());

		output /= filename;

		filesys.save_text_file({output}, str);
	}

	bool builder::execute_shell_str(const core::serializable &execute, core::log_context &log)
	{
		for(const core::serializable &child : execute) {
			if(!child.passes_condition(this)) {
				continue;
			}
			
			ucstring str{};
				
			ucstring name{child.get_name()};
			replace_vars(name);
			str += name;
			str += u8' ';
			const core::univalue &value{child.get_value()};
			if(!value.empty()) {
				ucstring value_str{value.get_string()};
				replace_vars(value_str);
				str += value_str;
				str += u8' ';
			}
			
			if(str.empty()) {
				continue;
			}

			str.pop_back();
			
			replace_all(str, u8"\""_sv, u8"\\\""_sv);

			shell_proc.set_args(str);
			
			const core::serializable *flags{child.get_flags()};
			if(flags) {
				const core::serializable *message{flags->get_child(u8"message"_sv)};
				if(message) {
					const core::univalue &value{message->get_value()};
					log.info(value.get_string());
				}
			}

			bool started{shell_proc.start(true)};
			if(!started) {
				log.error(u8"program failed to execute"_sv);
				log.add_ident();
				log.error(u8"workingdir: {}\npath: {}\nargs: {}"_sv, shell_proc.workingdir(), shell_proc.path(), shell_proc.args());
				log.remove_ident();
				return false;
			}

			const ucstring &output{shell_proc.output()};
			int32_t exit_code{shell_proc.exit_code()};

			bool success{exit_code == 0};

			if(success) {
				log.set_severity(core::log_severity::success);
			} else {
				log.set_severity(core::log_severity::error);
			}

			if(!output.empty()) {
				log.print(u8"exit_code {}:"_sv, exit_code);
				log.add_ident();
				log.print(output);
				log.remove_ident();
			} else if(!success) {
				log.print(u8"exit_code {}:"_sv, exit_code);
			}
			
			if(!success) {
				return false;
			}
		}

		return true;
	}
	
	bool builder::process_section(const core::serializable &src, core::serializable &dst)
	{
		for(const core::serializable &it : src) {
			if(began_gen) {
				if(!it.passes_condition(this)) {
					continue;
				}
			}
			
			ucstring name{it.get_name()};
			replace_vars(name);
			core::univalue value{it.get_value()};
			replace_vars(value);
			
			core::serializable &child{dst.child(name)};
			child.set_value(value);
			
			if(!process_section(it, child)) {
				return false;
			}
		}
		
		return true;
	}

	bool builder::generate_tool_section(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, base_plugin *plugin, core::log_context &log, const base_plugin::plugin_info_t &info)
	{
		const ucstring &sec_name{tool_section.get_name()};
		
		const_cast<tool_section_reference &>(tool_section).unity_build_ = false;

		const builder_section_reference *builder_sec{tool_section.builder_section()};
		if(builder_sec) {
			const core::serializable *unity_build{builder_sec->get_child(u8"unity_build"_sv)};
			if(unity_build) {
				if(unity_build->passes_condition(this)) {
					const_cast<tool_section_reference &>(tool_section).unity_build_ = unity_build->get_value_bool();
				}
			}
		}
		
		bool unity_build{tool_section.unity_build()};
		
		if(unity_build) {
			add_variable(u8"unity_build"_s, u8"true"_s);
		} else {
			add_variable(u8"unity_build"_s, u8"false"_s);
		}
		
		if(is_hardcoded_tool(sec_name)) {
			if(builder_sec) {
				generate_builder_section(*builder_sec, builder_sec_type::tool_section);
			}
			
			core::serializable section{};
			section.set_name(sec_name);
			if(!process_section(tool_section, section)) {
				return false;
			}
			
			if(!plugin->generate(solution, project, section)) {
				return false;
			}
		} else {
			const tool_reference *tool{tool_section.tool()};

			if(tool) {
				const ucstring &tool_name{tool->get_name()};
				add_variable(u8"tool_name"_s, tool_name);
			}
			
			const output_tool_reference *outputtool{tool_section.output_tool()};
			if(outputtool) {
				const tool_reference *tool_output{outputtool->tool()};
				if(tool_output) {
					const ucstring &output_tool_name{tool_output->get_name()};
					add_variable(u8"output_tool_name"_s, output_tool_name);
				}
			}
			
			if(builder_sec) {
				generate_builder_section(*builder_sec, builder_sec_type::tool_section);
			}

			if(tool) {
				builder_sec = tool->builder_section();
				if(builder_sec) {
					generate_builder_section(*builder_sec, builder_sec_type::tool);
				}
			}

			core::serializable *plugin_sec{const_cast<core::serializable *>(tool_section.plugin_section())};
			if(plugin_sec) {
				core::serializable section{};
				if(!process_section(*plugin_sec, section)) {
					return false;
				}
				plugin_sec->remove_all();
				plugin_sec->merge(section);
			}

			bool actually_do_single_input{
				(
					unity_build ||
					(tool && tool->single_input())
				) &&
				info.process_single_input
			};

			bool process_out_of_date{info.process_out_of_date};

			vector<const file_reference *> single_input_files{};
			bool single_input_out_of_date{false};

			size_t ident2{log.get_ident()};

			core::serializable *options{const_cast<tool_section_reference &>(tool_section).get_child(u8"options"_sv)};

			if(actually_do_single_input) {
				add_variable(u8"file_filter"_s, {});
				
				if(unity_build) {
					add_variable(u8"file_basename"_s, u8"unity_build"_s);
				} else {
					add_variable(u8"file_basename"_s, u8"single_input"_s);
				}
			}
			
			if(!plugin->populate_vars(solution, project, tool_section)) {
				return false;
			}

			core::serializable section_opts{};
			if(options) {
				if(!process_options(tool, *options, section_opts, info)) {
					return false;
				}
			}

			builder_sec = tool_section.builder_section();
			if(builder_sec) {
				const core::serializable *pre_generate{builder_sec->get_child(u8"pre_generate"_sv)};
				if(pre_generate) {
					if(!execute_shell_str(*pre_generate, log)) {
						return false;
					}
				}
			}

			if(!plugin->generate(solution, project, tool_section, section_opts)) {
				return false;
			}

			core::serializable *files{const_cast<tool_section_reference &>(tool_section).get_child(u8"files"_sv)};
			if(!files || files->empty()) {
				return true;
			}

			const core::serializable *patterns{tool->get_child(u8"patterns"_sv)};

			bool section_out_of_date{no_cache_ || tool_section.out_of_date()};

			core::searchpath timestamp_dir{{}, u8"builder"_sv};
			timestamp_dir.path /= u8"projects"_p;
			timestamp_dir.path /= project.get_name();
			timestamp_dir.path /= tool_section.get_name();

			bool no_timestamp{
				no_stamps_ ||
				tool_section.get_value() == u8"ignore"_sv
			};

			for(const core::serializable &child : *files) {
				const file_reference &file_main{reinterpret_cast<const file_reference &>(child)};
				
				if(!info.process_files_conditions && !file_main.passes_condition(this)) {
					continue;
				}
				
				pstring fullpath{file_main.path()};
				replace_vars(fullpath);
				const_cast<file_reference &>(file_main).set_name(as_string<ucstring>(fullpath));

				bool dynamic{file_main.get_flag_bool(u8"dynamic"_sv)};

				if((!info.process_tools_patterns && tool) && !dynamic) {
					if(patterns && !patterns->empty()) {
						bool found{false};
						for(const core::serializable &pat : *patterns) {
							const ucstring &name{pat.get_name()};
							pstring pattern{as_string<pstring>(name)};

							if(core::interfaces::filesystem::instance().matches_glob({fullpath}, pattern)) {
								found = true;
								break;
							}
						}
						if(!found) {
							continue;
						}
					}
				}
				
				pstring filter{file_main.filter()};
				add_variable(u8"file_filter"_s, as_string<ucstring>(filter));
				
				pstring filename{fullpath.filename()};
				pstring basename{filename};
				basename.replace_extension();

				add_variable(u8"file_basename"_s, as_string<ucstring>(basename));

				core::serializable file_options{};
				
				bool do_merge{info.merge_sections_files_options && !actually_do_single_input};
				
				if(!process_options(tool, const_cast<file_reference &>(file_main), file_options, info)) {
					return false;
				}
				
				if(do_merge) {
					file_options.merge(section_opts, false, __builder_internal::merge_replace_vars);
				}
				
				bool created_timestamp{false};
				
				if(!no_timestamp && !no_cache_ && !process_out_of_date) {
					core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
					
					core::searchpath output{timestamp_dir};
					output.path /= filter;
					output.path /= fullpath.filename();
					output.path.concat(u8".timestamp.bin"_s);
					
					if(!filesys.exists({output})) {
						if(debugging_) {
							log_builder().info(u8"generating {} timestamp"_sv, fullpath);
						}
						
						if(!timestamp_builder.generate({fullpath}, output)) {
							//return false;
						} else {
							created_timestamp = true;
						}
					}
				}

				bool actually_out_of_date{
					process_out_of_date ||
					(
						section_out_of_date ||
						bool_cast(file_main.flags_ & file_reference::flags::out_of_date) ||
						(!info.ignore_output && !output_exists(tool_section, section_opts, file_options, do_merge)) ||
						created_timestamp ||
						(!created_timestamp && file_changed(fullpath, filter, timestamp_dir))
					)
				};

				if(actually_do_single_input) {
					if(actually_out_of_date) {
						single_input_out_of_date = true;
					}
					single_input_files.emplace_back(&file_main);
				} else {
					if(actually_out_of_date) {
						log.set_ident(ident2);
						//log.info(u8"generating {}:"_sv, filter/filename);
						//log.add_ident();

						if(!plugin->generate(solution, project, tool_section, file_main, file_options)) {
							return false;
						}
					} else if(tool && !info.ignore_output && !dynamic) {
						pstring output_path{get_output_path(tool_section, section_opts, file_options, do_merge)};

						if(!output_path.empty()) {
							replace_vars(output_path);

							if(outputtool) {
								const tool_section_reference *out_sec{outputtool->section()};
								if(!out_sec) {
									return false;
								} else {
									out_sec->add_file(output_path, file_main.get_flags(), file_main.filter(), file_main.get_condition());
								}
							}
						}
					}
				}

				remove_variable(u8"file_filter"_s);
				remove_variable(u8"file_basename"_s);
			}

			if(actually_do_single_input && single_input_out_of_date) {
				size_t num{single_input_files.size()};
				if(num > 0) {
					log.set_ident(ident2);
					if(num > 1) {
						log.info(u8"generating {} files:"_sv, num);
					} else {
						log.info(u8"generating {}:"_sv, single_input_files[0]->path());
					}
					log.add_ident();

					if(!plugin->generate(solution, project, tool_section, single_input_files, section_opts)) {
						return false;
					}
				}
			}
			
			bool err{false};
			while(plugin->running(err)) {}
			if(err) {
				return false;
			}

			if(builder_sec) {
				const core::serializable *post_generate{builder_sec->get_child(u8"post_generate"_sv)};
				if(post_generate) {
					if(!execute_shell_str(*post_generate, log)) {
						return false;
					}
				}
			}
			
			core::serializable::iterator it{files->begin()};
			while(it != files->end()) {
				const file_reference &file_main{reinterpret_cast<const file_reference &>(*it)};
				file_reference::flags flags_{file_main.get_flags()};

				if(bool_cast(flags_ & file_reference::flags::added_by_tool)) {
					files->erase(it);
					continue;
				}

				it++;
			}
			
			remove_variable(u8"tool_name"_s);
			remove_variable(u8"output_tool_name"_s);
			
			if(actually_do_single_input) {
				remove_variable(u8"file_filter"_s);
				remove_variable(u8"file_basename"_s);
			}
		}

		return true;
	}

	tool_section_reference *builder::find_or_create_tool_section(project_reference &project, const ucstring &name, const solution_reference &solution)
	{
		tool_section_reference *tool_section{reinterpret_cast<tool_section_reference *>(project.get_child(name))};
		if(tool_section) {
			return tool_section;
		}

		tool_section = reinterpret_cast<tool_section_reference *>(&project.child(name));

		const ucstring &tool_name{tool_section->get_name()};

		if(!is_hardcoded_tool(tool_name)) {
			tool_section->tool_ = find_or_load_tool(tool_name, &solution);
			if(!tool_section->tool()) {
				return nullptr;
			}
		}

		return tool_section;
	}

	const tool_reference *builder::remap_tool(const ucstring_view &name, const solution_reference &solution)
	{
		if(is_hardcoded_tool(name)) {
			return nullptr;
		}

		for(const solution_reference::tool_info_t &info : solution.tools_) {
			if(info.name == name) {
				core::univalue result{};
				if(!core::parse_expression(info.condition, result, this)) {
					return nullptr;
				}
				if(result.get_bool()) {
					return info.tool;
				}
			}
		}

		return nullptr;
	}

	bool builder::generate_project(const solution_reference &solution, const project_reference &project, base_plugin *plugin, core::log_context &log, const base_plugin::plugin_info_t &info)
	{
		/*for(const project_reference *it : project.depends) {
			if(!generate_project(solution, *it, plugin, log, info)) {
				return false;
			}
		}
		
		if(contains(already_generated, &project)) {
			return true;
		}*/

		ucstring proj_name{project.get_name()};
		add_variable(u8"project_name"_s, proj_name);
		to_upper(proj_name, proj_name);
		add_variable(u8"project_name_upper"_s, proj_name);
		add_variable(u8"project_filter"_s, as_string<ucstring>(project.filter()));

		size_t ident{log.get_ident()};

		for(const core::serializable &child : project) {
			const tool_section_reference &tool_section_main{reinterpret_cast<const tool_section_reference &>(child)};

			const ucstring &section_name{tool_section_main.get_name()};

			const tool_reference *new_tool{remap_tool(section_name, solution)};
			if(new_tool) {
				const_cast<tool_section_reference &>(tool_section_main).tool_ = new_tool;
				add_variable(section_name, new_tool->get_name());
			}

			const tool_reference *tool{tool_section_main.tool()};
			if(tool) {
				output_tool_reference *output_tool{const_cast<output_tool_reference *>(tool_section_main.output_tool())};
				if(output_tool) {
					const ucstring &out_name{output_tool->get_value().get_string()};
					new_tool = remap_tool(out_name, solution);
					if(new_tool) {
						output_tool->tool_ = new_tool;
						add_variable(out_name, new_tool->get_name());
					}
					output_tool->section_ = find_or_create_tool_section(const_cast<project_reference &>(project), out_name, solution);
				}
			}
		}
		
		const builder_section_reference *builder_sec{project.builder_section()};
		if(builder_sec) {
			generate_builder_section(*builder_sec, builder_sec_type::project);

			const core::serializable *pre_generate{builder_sec->get_child(u8"pre_generate"_sv)};
			if(pre_generate) {
				if(!execute_shell_str(*pre_generate, log)) {
					return false;
				}
			}
		}
		
		if(!plugin->generate(solution, project)) {
			return false;
		}

		for(const core::serializable &child : project) {
			const tool_section_reference &tool_section_main{reinterpret_cast<const tool_section_reference &>(child)};
			
			const core::univalue &value{tool_section_main.get_value()};
			
			const ucstring &sec_name{tool_section_main.get_name()};
			
			if(!is_hardcoded_tool(sec_name)) {
				bool in_cmdline{false};
				bool generate_sec{true};
				
				if(!selected_sections.empty() ) {
					if(!contains(selected_sections, sec_name)) {
						generate_sec = false;
					} else {
						in_cmdline = true;
					}
				}
				
				if(!in_cmdline) {
					if(value == u8"ignore"_sv) {
						generate_sec = false;
					}
				}
				
				if(!generate_sec) {
					continue;
				}
			}

			log.set_ident(ident);
			log.info(u8"generating {}:"_sv, sec_name);
			log.add_ident();

			if(!generate_tool_section(solution, project, tool_section_main, plugin, log, info)) {
				return false;
			}
		}
		
		if(builder_sec) {
			const core::serializable *post_generate{builder_sec->get_child(u8"post_generate"_sv)};
			if(post_generate) {
				if(!execute_shell_str(*post_generate, log)) {
					return false;
				}
			}
		}
		
		//already_generated.emplace_back(&project);

		return true;
	}

	bool builder::generate_solution(const solution_reference &solution, base_plugin *plugin, core::log_context &log, const base_plugin::plugin_info_t &info)
	{
		add_variable(u8"solution_name"_s, solution.get_name());

		size_t ident{log.get_ident()};

		const ptr_vector<project_reference> &projects{solution.projects()};
		if(projects.empty()) {
			log.info(u8"no projects provided"_sv);
			return false;
		}
		
		if(!solution.loaded_from_cache_) {
			remove_variable(u8"project_name"_s);
			remove_variable(u8"project_name_upper"_s);
			remove_variable(u8"project_filter"_s);
		}

		const builder_section_reference *builder_sec{solution.builder_section()};
		if(builder_sec) {
			generate_builder_section(*builder_sec, builder_sec_type::solution);
			
			const core::serializable *pre_generate{builder_sec->get_child(u8"pre_generate"_sv)};
			if(pre_generate) {
				if(!execute_shell_str(*pre_generate, log)) {
					return false;
				}
			}
		}

		const core::serializable *build_set_list{solution.build_set()};

		gen_projects_vars_t vars{};
		vars.projects = &projects;
		vars.ident = ident;
		vars.plugin = plugin;
		vars.info = &info;
		vars.log = &log;
		vars.solution = &solution;
		
		build_set_t::build_sets_t build_sets{};

		const core::commandline &cmdline{core::commandline::instance()};
		
		if(build_set_list) {
			for(const core::serializable &child : *build_set_list) {
				const ucstring &set_name{child.get_name()};
				
				build_set_t &build_set{build_set_t::find_or_create(set_name, build_sets)};
				build_set_t::set_t &set{build_set.sets.emplace_back()};
				
				set.condition = child.get_condition();

				const vector<core::univalue> *values{cmdline.values(set_name)};
				if(values) {
					for(const core::univalue &value : *values) {
						const ucstring &value_str{value.get_string()};
						build_set_t::set_value_t &set_value{set.values.emplace_back()};
						set_value.value = value_str;
						
						if(info.process_build_set) {
							break;
						}
					}
				} else {
					for(const core::serializable &value : child) {
						const ucstring &value_str{value.get_name()};
						build_set_t::set_value_t &set_value{set.values.emplace_back()};
						set_value.value = value_str;
						
						if(info.process_build_set) {
							break;
						}
						
						set_value.condition = value.get_condition();
					}
				}
			}
		}

		size_t num{build_sets.size()};

		#define __MFW_SET_LOOP_SWITCH_EXECUTE(...) \
			{ if(!generate_projects(vars, ##__VA_ARGS__)) { return false; } break; }

		began_gen = true;

		switch(num) {
			case 0: __MFW_SET_LOOP_SWITCH_EXECUTE()
			case 1: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0])
			case 2: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1])
			case 3: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2])
			case 4: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3])
			case 5: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3],
			build_sets[4])
			case 6: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3],
			build_sets[4],
			build_sets[5])
			case 7: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3],
			build_sets[4],
			build_sets[5],
			build_sets[6])
			case 8: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3],
			build_sets[4],
			build_sets[5],
			build_sets[6],
			build_sets[7])
			case 9: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3],
			build_sets[4],
			build_sets[5],
			build_sets[6],
			build_sets[7],
			build_sets[8])
			case 10: __MFW_SET_LOOP_SWITCH_EXECUTE(
			build_sets[0],
			build_sets[1],
			build_sets[2],
			build_sets[3],
			build_sets[4],
			build_sets[5],
			build_sets[6],
			build_sets[7],
			build_sets[8],
			build_sets[9])
			default: {
				log.error(u8"more then 10 build sets is not supported"_sv);
				return false;
			}
		}

		if(!plugin->generate(solution)) {
			return false;
		}
		
		if(builder_sec) {
			const core::serializable *post_generate{builder_sec->get_child(u8"post_generate"_sv)};
			if(post_generate) {
				if(!execute_shell_str(*post_generate, log)) {
					return false;
				}
			}
		}

		return true;
	}

	bool builder::generate_solutions()
	{
		core::interfaces::filesystem &filesys{core::interfaces::filesystem::instance()};
		pstring data_folder{filesys.resolve({{}, u8"data"_sv}, false)};

		for(base_plugin *plugin : plugins) {
			const ucstring &name{plugin->name()};
			
			plugin->initialize(*this);

			pstring plugin_data_folder{data_folder/name};

			add_variable(u8"plugin_name"_s, name);
			add_variable(u8"plugin_data_folder"_s, as_string<ucstring>(plugin_data_folder));

			filesys.set_working_dir({plugin_data_folder});

			const base_plugin::plugin_info_t &info{plugin->info()};
			core::log_context &log{plugin->log()};

			size_t ident{log.get_ident()};
			for(solution_reference &solution : solutions) {
				log.set_ident(ident);
				log.info(u8"generating {}:"_sv, solution.get_name());
				log.add_ident();

				if(!generate_solution(solution, plugin, log, info)) {
					return false;
				}
				
				plugin->cleanup(base_plugin::cleanup_type_t::solution);
			}
		}

		return true;
	}

	#define __MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN_BASE(...) \
		bool builder::generate_projects(const gen_projects_vars_t &vars, ##__VA_ARGS__) \
		{ \
			const ptr_vector<project_reference> &projects{*vars.projects}; \
			base_plugin *plugin{vars.plugin}; \
			const base_plugin::plugin_info_t &info{*vars.info}; \
			core::log_context &log{*vars.log}; \
			size_t ident{vars.ident}; \
			const solution_reference &solution{*vars.solution}; \
			for(const project_reference &project_main : projects) { \
				const ucstring &name{project_main.get_name()}; \
				if(!project_main.passes_condition(this)) { \
					continue; \
				} \
				if(!selected_projects.empty()) { \
					if(!contains(selected_projects, name)) { \
						continue; \
					} \
				} \
				log.set_ident(ident); \
				log.info(u8"generating {}:"_sv, name); \
				log.add_ident();

	#define __MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(...) \
		__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN_BASE(__VA_ARGS__) \
		size_t ident2{log.get_ident()};

	#define __MFW_SET_LOOP_VAR_BEGIN(i) \
		for(const build_set_t::set_t &set_name##i : set##i.sets) { \
			const ucstring &name_condition##i{set_name##i.condition}; \
			if(!name_condition##i.empty()) { \
				core::univalue result{}; \
				if(!core::parse_expression(name_condition##i, result, this)) { \
					return false; \
				} \
				if(!result.get_bool()) { \
					continue; \
				} \
			} \
			for(const build_set_t::set_value_t &set_value##i : set_name##i.values) { \
				const ucstring &value_condition##i{set_value##i.condition}; \
				if(!value_condition##i.empty()) { \
					core::univalue result{}; \
					if(!core::parse_expression(value_condition##i, result, this)) { \
						return false; \
					} \
					if(!result.get_bool()) { \
						continue; \
					} \
				} \
				const ucstring &value##i{set_value##i.value}; \
				add_variable(set##i.name, value##i);

	#define __MFW_SET_LOOP_VAR_END(i) \
			} \
		}

	#define __MFW_SET_LOOP_PROJECT(...) \
		log.set_ident(ident2); \
		log.info(__VA_ARGS__); \
		log.add_ident(); \
		if(!generate_project(solution, project_main, plugin, log, info)) { \
			return false; \
		} \
		plugin->cleanup(base_plugin::cleanup_type_t::project);

	#define __MFW_SET_LOOP_IMPLEMENT_FUNC_END \
			} \
			return true; \
		}

	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN_BASE()
		if(!generate_project(solution, project_main, plugin, log, info)) {
			return false;
		}
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END

	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_PROJECT(u8"[{}]:"_sv, value0)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END

	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}]:"_sv,
	value0,
	value1)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END

	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}]:"_sv,
	value0,
	value1,
	value2)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END

	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END

	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3,
	const build_set_t &set4)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_VAR_BEGIN(4)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3,
	value4)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_VAR_END(4)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END
	
	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3,
	const build_set_t &set4,
	const build_set_t &set5)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_VAR_BEGIN(4)
	__MFW_SET_LOOP_VAR_BEGIN(5)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3,
	value4,
	value5)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_VAR_END(4)
	__MFW_SET_LOOP_VAR_END(5)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END
	
	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3,
	const build_set_t &set4,
	const build_set_t &set5,
	const build_set_t &set6)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_VAR_BEGIN(4)
	__MFW_SET_LOOP_VAR_BEGIN(5)
	__MFW_SET_LOOP_VAR_BEGIN(6)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}/{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3,
	value4,
	value5,
	value6)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_VAR_END(4)
	__MFW_SET_LOOP_VAR_END(5)
	__MFW_SET_LOOP_VAR_END(6)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END
	
	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3,
	const build_set_t &set4,
	const build_set_t &set5,
	const build_set_t &set6,
	const build_set_t &set7)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_VAR_BEGIN(4)
	__MFW_SET_LOOP_VAR_BEGIN(5)
	__MFW_SET_LOOP_VAR_BEGIN(6)
	__MFW_SET_LOOP_VAR_BEGIN(7)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}/{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3,
	value4,
	value5,
	value6,
	value7)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_VAR_END(4)
	__MFW_SET_LOOP_VAR_END(5)
	__MFW_SET_LOOP_VAR_END(6)
	__MFW_SET_LOOP_VAR_END(7)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END
	
	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3,
	const build_set_t &set4,
	const build_set_t &set5,
	const build_set_t &set6,
	const build_set_t &set7,
	const build_set_t &set8)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_VAR_BEGIN(4)
	__MFW_SET_LOOP_VAR_BEGIN(5)
	__MFW_SET_LOOP_VAR_BEGIN(6)
	__MFW_SET_LOOP_VAR_BEGIN(7)
	__MFW_SET_LOOP_VAR_BEGIN(8)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}/{}/{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3,
	value4,
	value5,
	value6,
	value7,
	value8)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_VAR_END(4)
	__MFW_SET_LOOP_VAR_END(5)
	__MFW_SET_LOOP_VAR_END(6)
	__MFW_SET_LOOP_VAR_END(7)
	__MFW_SET_LOOP_VAR_END(8)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END
	
	__MFW_SET_LOOP_IMPLEMENT_FUNC_BEGIN(
	const build_set_t &set0,
	const build_set_t &set1,
	const build_set_t &set2,
	const build_set_t &set3,
	const build_set_t &set4,
	const build_set_t &set5,
	const build_set_t &set6,
	const build_set_t &set7,
	const build_set_t &set8,
	const build_set_t &set9)
	__MFW_SET_LOOP_VAR_BEGIN(0)
	__MFW_SET_LOOP_VAR_BEGIN(1)
	__MFW_SET_LOOP_VAR_BEGIN(2)
	__MFW_SET_LOOP_VAR_BEGIN(3)
	__MFW_SET_LOOP_VAR_BEGIN(4)
	__MFW_SET_LOOP_VAR_BEGIN(5)
	__MFW_SET_LOOP_VAR_BEGIN(6)
	__MFW_SET_LOOP_VAR_BEGIN(7)
	__MFW_SET_LOOP_VAR_BEGIN(8)
	__MFW_SET_LOOP_VAR_BEGIN(9)
	__MFW_SET_LOOP_PROJECT(u8"[{}/{}/{}/{}/{}/{}/{}/{}/{}/{}]:"_sv,
	value0,
	value1,
	value2,
	value3,
	value4,
	value5,
	value6,
	value7,
	value8,
	value9)
	__MFW_SET_LOOP_VAR_END(0)
	__MFW_SET_LOOP_VAR_END(1)
	__MFW_SET_LOOP_VAR_END(2)
	__MFW_SET_LOOP_VAR_END(3)
	__MFW_SET_LOOP_VAR_END(4)
	__MFW_SET_LOOP_VAR_END(5)
	__MFW_SET_LOOP_VAR_END(6)
	__MFW_SET_LOOP_VAR_END(7)
	__MFW_SET_LOOP_VAR_END(8)
	__MFW_SET_LOOP_VAR_END(9)
	__MFW_SET_LOOP_IMPLEMENT_FUNC_END
}