#ifndef __MFW_PRIVATE_BUILDER_PLUGINS_PLUGIN_PROCESS_H
#define __MFW_PRIVATE_BUILDER_PLUGINS_PLUGIN_PROCESS_H

#pragma once

#include <private/mfw/builder/base_plugin.hpp>
#include <public/mfw/core/process.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>
#include <private/mfw/builder/formats/json.hpp>
#include <private/mfw/builder/plugins/shared.hpp>

namespace mfw::builder
{
	class plugin_process final : public base_plugin
	{
	public:
		using super = base_plugin;
	
		plugin_process();

		struct tool_info_t
		{
			tool_info_t() = default;
			tool_info_t(const tool_info_t &) = default;
			tool_info_t(tool_info_t &&) = default;

			ucstring name{};
			const tool_reference *tool{nullptr};
			const output_tool_reference *output_tool{nullptr};
			bool warning_errors{false};
			ucstring drive{};
			ucchar_t equal_char{
		#if MFW_OS_IS(LINUX)
			u8'='
		#elif MFW_IS_IS(WINDOWS)
			u8':'
		#else
			#error
		#endif
			};
			bool always_equal{false};
			ucstring base_args{};
			vector<int32_t> success_codes{};
			vector<ucstring> info_regex{};
			vector<ucstring> warning_regex{};
			vector<ucstring> error_regex{};
			vector<ucstring> ignore_regex{};
			
			bool needs_implib{false};
			
			enum class implib_arch_t : uchar_t
			{
				unknown,
				i386,
				x86_64,
				aarch64,
				arm,
			};
			struct implib_t
			{
				pstring folder{};
				pstring output1{};
				pstring output2{};
				implib_arch_t arch{implib_arch_t::unknown};
				
				struct symbol_name_t
				{
					bool mangled{true};
					ucstring name{};
					
					bool matches(const ucstring &other) const;
				};
				
				vector<symbol_name_t> symbols{};
				bool has_glob{false};
				vector<symbol_name_t> remove_symbols{};
			};
			implib_t implib{};
			
			bool is_unix_linker{false};
			
			pstring path{};
			ucstring cmd{};
			pstring workdir{};
			
			pstring output_path{};
		};
		
		struct tool_execute_info_t
		{
			const tool_section_reference *section{nullptr};
			core::serializable options{};
			core::serializable::iterator output_arg{};
			const tool_info_t *tool{nullptr};
			
			void clear();
			bool setup(const tool_info_t &tool_info, const tool_section_reference &tool_section, const core::serializable &options_);
		};
		
		struct compiler_tool_info_t : tool_execute_info_t
		{
			using super = tool_execute_info_t;
			
			bool setup(const tool_info_t &tool_info, const tool_section_reference &tool_section, const core::serializable &options_);
			
			ucstring compiler_lib_dirs{};
		};
		
		compiler_tool_info_t compiler{};

	private:
		void initialize(interfaces::builder_funcs &funcs) override;
	
		void insert_help(ucstring &help) override;
	
		void cleanup(cleanup_type_t type) override;
	
		bool populate_vars(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section) override;
	
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const file_reference &file, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const vector<const file_reference *> &files, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project) override;
		bool generate(const solution_reference &solution) override;
		bool generate(const solution_reference &solution, const project_reference &project, const core::serializable &section) override;
		
		class compile_command_t : public json::file
		{
		public:
			using super = json::file;
		
			void add_file(const core::process &proc, const file_reference &file);
			
			struct command_t
			{
				pstring file{};
				ucstring command{};
			};
			
			void save();
			
			pstring directory{};
			vector<command_t> files{};
			pstring path{};
		};
		
		ptr_vector<compile_command_t> compile_commands{};
		
		bool only_compile_commands{false};
		
		enum class sec_flags : uchar_t
		{
			none,
			warnings,
			errors,
		};
		MFW_CLASS_ENUM_FLAGS(sec_flags)
		
		void print_section(const ucstring &output, size_t start, size_t end, sec_flags &flags, const tool_info_t &info);
		void print_sections(const ucstring &output, sec_flags &flags, const tool_info_t &info);
		
		void process_option(const core::serializable &option, ucstring &str, const tool_info_t &info);
		void process_options(const core::serializable &options, ucstring &str, const tool_info_t &info);
		
		static void add_file_to_str(const pstring &path, ucstring &str, const tool_info_t &info);
		
		void add_file_to_comamnds(json::file &compile_comamnds, const core::process &proc, const file_reference &file);
		//void add_file_to_comamnds(json::file &compile_comamnds, const core::process &proc, const vector<const file_reference *> &files);

		bool execute_tool(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const tool_info_t &info, const file_reference &file, const core::serializable &options);
		bool execute_tool(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const tool_info_t &info, const vector<const file_reference *> &files, const core::serializable &options);
		
		bool compile_cpp_file(const solution_reference &solution, const project_reference &project, const pstring &file, const pstring &output);
	
		void parse_plugin(const core::serializable &section, bool replace);
	
		struct gen_lib_vars_t
		{
			const pstring *folder{nullptr};
			const pstring *output1{nullptr};
			const pstring *output2{nullptr};
			tool_info_t::implib_arch_t arch{tool_info_t::implib_arch_t::unknown};
			const vector<tool_info_t::implib_t::symbol_name_t> *symbols{nullptr};
			const vector<tool_info_t::implib_t::symbol_name_t> *remove_symbols{nullptr};
			bool has_glob{false};
		};
		bool generate_implib(const solution_reference &solution, const project_reference &project, const pstring &path, const gen_lib_vars_t &vars, bool regen);

		struct proc_vars_t
		{
			pstring output_path{};
			const file_reference *file{nullptr};
			bool unity_build{false};
			pstring unity_file_path{};
			const tool_section_reference *out_sec{nullptr};
			const tool_info_t *tool_info{nullptr};
			const solution_reference *solution{nullptr};
			const project_reference *project{nullptr};
		};

		bool process_done(core::process &proc, const proc_vars_t &vars, const tool_info_t &info);

		bool running(bool &err) override;

		struct proc_info_t
		{
			core::process proc{};
			bool started{false};
			proc_vars_t vars{};
			
			void setup(proc_vars_t &&vars_, const ucstring &str, const tool_info_t &tool_info);
		};
		
		bool start(proc_info_t &proc_info, core::log_context &log);
		
		ptr_vector<tool_info_t> toolsinfos{};
		
		#define __MFW_PROCESSES_UNLIMITED MFW_UINT64_MAX
		
		using proc_vec_t = vector<proc_info_t>;
		proc_vec_t processes{};
		
		size_t num_started{0};
		
		size_t curr_processes() const { return num_started; }
		size_t max_processes() const { return max_processes_; }
		bool hit_process_limit() const {
			if(max_processes_ == __MFW_PROCESSES_UNLIMITED) {
				return false;
			} else if(max_processes_ == 0) {
				return true;
			} else {
				return curr_processes() > max_processes_;
			}
		}
		bool multi_process() const {
			if(max_processes_ == __MFW_PROCESSES_UNLIMITED) {
				return true;
			} else {
				return max_processes_ > 1;
			}
		}
		
		size_t max_processes_{0};
		bool printcmdline{false};
	};
}

#endif