#ifndef __MFW_PRIVATE_BUILDER_PLUGINS_PLUGIN_VSCODE_H
#define __MFW_PRIVATE_BUILDER_PLUGINS_PLUGIN_VSCODE_H

#pragma once

#include <private/mfw/builder/base_plugin.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>
#include <private/mfw/builder/formats/json.hpp>
#include <private/mfw/builder/plugins/shared.hpp>

namespace mfw::builder
{
	class plugin_vscode : public base_plugin
	{
	public:
		plugin_vscode();

	private:
		bool generate(const solution_reference &solution) override;
		bool generate(const solution_reference &solution, const project_reference &project) override;
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const file_reference &file, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project, const core::serializable &section) override;
	
		void cleanup(cleanup_type_t type) override;
	
		struct json_workspace_t : json::file
		{
			using super = json::file;
			
			json_workspace_t(const json_workspace_t &) = delete;
			json_workspace_t(json_workspace_t &&) = default;
			json_workspace_t() = default;
			virtual ~json_workspace_t() = default;
			
			vector<pstring> exclude_files{};
			struct folder_t
			{
				ucstring name{};
				pstring path{};
			};
			vector<folder_t> folders{};

			pstring compiler_path{};
			vector<pstring> include_paths{};
			vector<ucstring> defines{};
			vector<pstring> force_includes{};
			core::serializable compiler_options{};
			pstring compile_commands{};
			ucstring c_standard{};
			ucstring cpp_standard{};

			pstring program{};
			pstring cwd{};
			vector<ucstring> args{};
			
			compiler_info_t compiler_type{};
			
			vector<ucstring> problem_matcher{};

			pstring command{};

			enum class debugger_type : uchar_t
			{
				unknown,
				lldb,
				gdb,
			};
			debugger_type debugger{debugger_type::unknown};
			
			ucstring name{};
			pstring path{};

			virtual void save();
			virtual void write_all();
			
			void write_cpp_properties(json::file &cpp_properties);
			void write_launch(json::file &launch);
			void write_tasks(json::file &tasks);
			
			bool solution{true};
			
			virtual void clear();
		};
		
		struct json_solution_t : json_workspace_t
		{
			using super = json_workspace_t;
			
			json_solution_t(const json_solution_t &) = delete;
			json_solution_t(json_solution_t &&) = default;
			json_solution_t()
				: super{}
			{ solution = true; }
		};
	
		struct json_project_t : json_workspace_t
		{
			using super = json_workspace_t;
			
			json_project_t(const json_project_t &) = delete;
			json_project_t(json_project_t &&) = default;
			json_project_t()
				: super{}
			{ solution = false; }
			
			json::file cpp_properties{};
			json::file launch{};
			json::file tasks{};
			
			void save() override;
			void write_all() override;
		};

		void parse_ide(const core::serializable &section, json_workspace_t &json, bool replace);

		ptr_vector<json_project_t> json_projects{};
		json_solution_t solution_json{};
	};
}

#endif