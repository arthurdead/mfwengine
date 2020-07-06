#ifndef __MFW_PRIVATE_BUILDER_BUILDER_HPP
#define __MFW_PRIVATE_BUILDER_BUILDER_HPP

#pragma once

#include <private/mfw/builder/base_plugin.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <private/mfw/builder/file_timestamp_builder.hpp>
#include <private/mfw/builder/references/project_reference.hpp>
#include <private/mfw/builder/references/solution_reference.hpp>
#include <private/mfw/builder/references/tool_section_reference.hpp>
#include <private/mfw/builder/references/file_reference.hpp>
#include <public/mfw/core/process.hpp>

namespace mfw::builder
{
	class builder final : public interfaces::builder_funcs
	{
	public:
		builder();
		~builder();

		core::exit_status start();

		static builder &instance();

		bool parse_plugins();

		class root_file_base : public core::serializable
		{
		private:
			friend class builder;

			core::serializable *allocate_child(ssize_t depth, const ucstring_view &name_, const core::serializable *parent) const override
			{
				if(depth == 0) {
					if(name_ == which_) {
						return allocate_selected();
					}
				}
				return new serializable{};
			}

			virtual core::serializable *allocate_selected() const = 0;

			ucstring which_{};
		};

		void replace_vars(pstring &file) const override;
		void replace_vars(ucstring &str) const override;
		void replace_vars(core::univalue &val) const override;

	private:
		enum class cache_type : uchar_t
		{
			project,
			solution,
			tool,
		};
		bool open_cached_file_internal(const ucstring &name, const pstring &filter, cache_type type, root_file_base &root_file, bool &cached);
		bool save_cached_file_internal(const ucstring &name, const pstring &filter, cache_type type, const core::serializable &main_section);

		bool open_cached_file_solution(const ucstring &name, root_file_base &root_file, bool &cached);
		bool save_cached_file_solution(const ucstring &name, const core::serializable &main_section);

		bool open_cached_file_tool(const ucstring &name, root_file_base &root_file, bool &cached);
		bool save_cached_file_tool(const ucstring &name, const core::serializable &main_section);

		bool open_cached_file_project(const ucstring &name, const pstring &filter, root_file_base &root_file, bool &cached);
		bool save_cached_file_project(const ucstring &name, const pstring &filter, const core::serializable &main_section);

		pstring get_output_path(const tool_section_reference &tool_section, const core::serializable &options, const core::serializable &file_options, bool merged) const;
		bool output_exists(const tool_section_reference &tool_section, const core::serializable &options, const core::serializable &file_options, bool merged) const;

		struct remap_result_t
		{
			core::serializable *child{nullptr};
			const core::serializable *map{nullptr};
		};
		bool remap_value(const core::serializable &option, const core::serializable *map, core::serializable &options, remap_result_t &result, const base_plugin::plugin_info_t &info);
		bool process_options(const tool_reference *tool, core::serializable &options, core::serializable &mapped_options, const base_plugin::plugin_info_t &info);

		bool process_dependency(const core::serializable &src, core::serializable &dst, bool self);
		bool process_section(const core::serializable &src, core::serializable &dst);

		enum class builder_sec_type : uchar_t
		{
			project,
			tool_section,
			solution,
			tool,
		};
		void generate_builder_section(const builder_section_reference &sec, builder_sec_type type);

		bool get_variable(const ucstring_view &name, core::type_holder &var) const override;

		void add_variable(const ucstring &name, const ucstring &value) override;
		void remove_variable(const ucstring &name) override;
		
		struct scope_variable
		{
			scope_variable(const ucstring &name_, const ucstring &value) {
				builder::instance().add_variable(name_, value);
				name = name_;
			}
			~scope_variable() {
				builder::instance().remove_variable(name);
			}
			
			ucstring name{};
		};

		tool_section_reference *find_or_create_tool_section(project_reference &project, const ucstring &name, const solution_reference &solution);

		bool execute_shell_str(const core::serializable &execute, core::log_context &log);

		bool generate_solutions();
		bool generate_solution(const solution_reference &solution, base_plugin *plugin, core::log_context &log, const base_plugin::plugin_info_t &info);
		bool generate_project(const solution_reference &solution, const project_reference &project, base_plugin *plugin, core::log_context &log, const base_plugin::plugin_info_t &info);
		bool generate_tool_section(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, base_plugin *plugin, core::log_context &log, const base_plugin::plugin_info_t &info);

		project_reference *find_or_load_project(const ucstring &name, const pstring &filter, const ucstring &condition, solution_reference &solution);

		static void generate_pch(const pstring &file, const tool_reference &tool, const project_reference &project, const solution_reference &solution);

		bool parse_dependency(project_reference &project, const project_reference &other, const ucstring_view &name);
		bool parse_dependencies(project_reference &project, const project_reference &other, const ucstring_view &name);
		bool parse_builder_section(const builder_section_reference &sec, project_reference &project, solution_reference &solution);

		bool parse_plugin(const ucstring &name);
		bool parse_solutions();
		bool parse_solution(solution_reference &solution);
		bool parse_projects(const core::serializable &projects, const pstring &filter, solution_reference &solution);
		project_reference *load_project(const ucstring &name, const pstring &filter, const ucstring &condition, solution_reference &solution);
		bool parse_project(project_reference &project, solution_reference &solution);
		bool parse_tool_section(tool_section_reference &tool_section, project_reference &project, const solution_reference &solution);
		bool parse_tool(tool_reference &tool);
		enum class parse_files_flags : uchar_t
		{
			none = 0,
			remove = MFW_BIT(0),
			dynamic = MFW_BIT(1),
			delay = MFW_BIT(2),
		};
		MFW_CLASS_ENUM_FLAGS(parse_files_flags)
		void parse_files(const core::serializable &files, core::serializable *target, const pstring &filter, parse_files_flags flags, core::serializable *removed, core::serializable *parent, tool_section_reference &tool_section);
		void parse_folders(const core::serializable &folders, core::serializable *target, bool remove, core::serializable *removed, core::serializable *parent);
		void parse_options(core::serializable &options, tool_section_reference &tool_section);

		bool file_changed(const pstring &fullpath, const pstring &filter, const core::searchpath &timestamp_dir) const;

		const tool_reference *find_or_load_tool(const ucstring_view &name, const solution_reference *solution);

		const tool_reference *remap_tool(const ucstring_view &name, const solution_reference &solution);

		bool load_tool(const ucstring &name, const solution_reference *solution);

		static bool is_hardcoded_tool(const ucstring_view &name);
		
		struct build_set_t
		{
			ucstring name{};
			
			struct set_t
			{
				ucstring condition{};

				struct set_value_t
				{
					ucstring value{};
					ucstring condition{};
				};

				using values_t = vector<set_value_t>;
				values_t values{};
			};
			using set_value_t = set_t::set_value_t;
			using values_t = set_t::values_t;
			
			using sets_t = vector<set_t>;
			sets_t sets{};
			
			using build_sets_t = ptr_vector<build_set_t>;
			static build_set_t &find_or_create(const ucstring_view &name, build_sets_t &sets)
			{
				for(build_set_t &set : sets) {
					if(name == set.name) {
						return set;
					}
				}
				build_set_t &set{sets.emplace_back()};
				set.name = name;
				return set;
			}
		};
		struct gen_projects_vars_t
		{
			const ptr_vector<project_reference> *projects{nullptr};
			base_plugin *plugin{nullptr};
			const base_plugin::plugin_info_t *info{nullptr};
			core::log_context *log{nullptr};
			size_t ident{0};
			const solution_reference *solution{nullptr};
		};

		#define __MFW_SET_LOOP_DECLARE_FUNC_BEGIN(...) \
			bool generate_projects(const gen_projects_vars_t &vars, ##__VA_ARGS__);

		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN()
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2,
			const build_set_t &set3)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2,
			const build_set_t &set3,
			const build_set_t &set4)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2,
			const build_set_t &set3,
			const build_set_t &set4,
			const build_set_t &set5)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2,
			const build_set_t &set3,
			const build_set_t &set4,
			const build_set_t &set5,
			const build_set_t &set6)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2,
			const build_set_t &set3,
			const build_set_t &set4,
			const build_set_t &set5,
			const build_set_t &set6,
			const build_set_t &set7)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
			const build_set_t &set0,
			const build_set_t &set1,
			const build_set_t &set2,
			const build_set_t &set3,
			const build_set_t &set4,
			const build_set_t &set5,
			const build_set_t &set6,
			const build_set_t &set7,
			const build_set_t &set8)
		__MFW_SET_LOOP_DECLARE_FUNC_BEGIN(
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

		vector<base_plugin *> plugins{};
		ptr_vector<solution_reference> solutions{};
		ptr_vector<tool_reference> tools{};
		
		vector<ucstring> selected_sections{};
		vector<ucstring> selected_projects{};
		
		//vector<const project_reference *> already_generated{};

		file_timestamp_builder timestamp_builder{};

		bool no_stamps_{false};

		bool began_gen{false};

		using macros_t = unordered_map<ucstring, ucstring>;
		macros_t macros{};

		core::process shell_proc{};
	};
}

#endif