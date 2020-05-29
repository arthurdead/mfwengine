#ifndef __MFW_PRIVATE_BUILDER_PLUGINS_PLUGIN_QPC_H
#define __MFW_PRIVATE_BUILDER_PLUGINS_PLUGIN_QPC_H

#pragma once

#include <private/mfw/builder/base_plugin.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>
#include <private/mfw/builder/plugins/shared.hpp>

namespace mfw::builder
{
	/*
	class plugin_qpc : public base_plugin
	{
	public:
		plugin_qpc();

	private:
		bool generate(const solution_reference &solution) override;
		bool generate(const solution_reference &solution, const project_reference &project) override;
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project, const tool_section_reference &tool_section, const file_reference &file, const core::serializable &options) override;
		bool generate(const solution_reference &solution, const project_reference &project, const core::serializable &section) override;
	
		void cleanup(cleanup_type_t type) override;
	};
	*/
}

#endif