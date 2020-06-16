#ifndef __MFW_PRIVATE_BUILDER_BASE_PLUGIN_H
#define __MFW_PRIVATE_BUILDER_BASE_PLUGIN_H

#pragma once

#include <public/mfw/stl/string.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/logging_interface.hpp>
#include <private/mfw/builder/references/project_reference.hpp>
#include <private/mfw/builder/references/solution_reference.hpp>
#include <private/mfw/builder/references/tool_section_reference.hpp>
#include <private/mfw/builder/references/file_reference.hpp>

namespace mfw::builder
{
	namespace interfaces
	{
		class builder_funcs : public core::interfaces::expression_parser_callbacks
		{
		public:
			bool regen_cache() const { return regen_cache_; }
			bool no_cache() const { return no_cache_; }
			bool debugging() const { return debugging_; }
			
			bool regen_or_no_cache() const { return regen_cache_ || no_cache_; }
		
			virtual void replace_vars(pstring &file) const = 0;
			virtual void replace_vars(ucstring &str) const = 0;
			virtual void replace_vars(core::univalue &val) const = 0;
			
			virtual void add_variable(const ucstring &name, const ucstring &value) = 0;
			virtual void remove_variable(const ucstring &name) = 0;
		
		protected:
			bool regen_cache_{false};
			bool no_cache_{false};
			bool debugging_{false};
		};
	}
	
	class base_plugin
	{
	protected:
		base_plugin(const ucstring &name);

	public:
		const ucstring &name() const { return name_; }
		core::log_context &log() { return log_; }

		struct plugin_info_t
		{
			bool process_files_conditions{false};
			bool process_options_conditions{false};
			bool process_tools_patterns{false};
			bool process_sections_args_unmaped{false};
			bool process_build_set{false};
			bool process_single_input{true};
			bool merge_sections_files_options{true};
			bool process_out_of_date{false};
			bool ignore_output{false};
		};
		
		virtual void initialize(interfaces::builder_funcs &funcs) { builder_funcs_ = &funcs; }
		interfaces::builder_funcs &builder_funcs() { return *builder_funcs_; }
		
		virtual void insert_help(ucstring &help) {}

		enum cleanup_type_t : uchar_t
		{
			project,
			solution,
		};
		virtual void cleanup(cleanup_type_t type) {}

		const plugin_info_t &info() const { return info_; }

		virtual bool populate_vars(const solution_reference &solution) { return true; }
		virtual bool populate_vars(const solution_reference &solution, const project_reference &project) { return true; }
		virtual bool populate_vars(const solution_reference &solution, const project_reference &project, const core::serializable &section) { return true; }
		virtual bool populate_vars(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section) { return true; }

		virtual bool generate(const solution_reference &solution) { return true; }
		virtual bool generate(const solution_reference &solution, const project_reference &project) { return true; }
		virtual bool generate(const solution_reference &solution, const project_reference &project, const core::serializable &section) { return true; }
		virtual bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options) { return true; }
		virtual bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const file_reference &file, const core::serializable &options) { return true; }
		virtual bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const vector<const file_reference *> &files, const core::serializable &options) { return true; }

		virtual bool running(bool &err) {
			err = false;
			return false;
		}

	private:
		core::log_context log_{};
		ucstring name_{};
	protected:
		plugin_info_t info_{};
	private:
		interfaces::builder_funcs *builder_funcs_{nullptr};
	};

	#define MFW_DECLARE_BUILDER_PLUGIN(cls, name) \
		static cls __##name##_plugin{}; \
		extern "C" { \
			MFW_SHARED_EXPORT base_plugin & MFW_CALL_SHARED __##name##_get_plugin() { \
				return __##name##_plugin; \
			} \
		};
}

#endif