#ifndef MFW_PRIVATE_CORE_FILESYSTEM_HPP
#define MFW_PRIVATE_CORE_FILESYSTEM_HPP

#pragma once

#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/file_interface.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/core/application.hpp>
#include <public/mfw/core/searchpath.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::core
{
	class filesystem final : public interfaces::filesystem, interfaces::global_initializer
	{
	public:
		static filesystem &instance();

		exit_status initialize() override;
		exit_status update() override { return {}; }
		exit_status shutdown() override;

		bool add_searchpath(const searchpath &search, const searchpath &relative = {}) override;
		bool remove_searchpath(const searchpath &search) override;

		bool set_working_dir(const searchpath &search) const override;
		pstring get_working_dir() const override;

		pstring resolve(const searchpath &search, bool exists = true) const override;
		bool resolve(const searchpath &search, vector<pstring> &paths, bool exists = true) const override;
		const vector<pstring> *get_paths(const ucstring &name) const override;
		pstring clean(const searchpath &search) const override;

		uint64_t get_file_modified_time(const searchpath &search) const override;

		bool glob(const searchpath &search, vector<pstring> &files) const override;
		bool matches_glob(const searchpath &search, const pstring &pattern) const override;

		bool create_link(const searchpath &from, const searchpath &to, bool dir=false, bool hard=false) const override;
		bool remove(const searchpath &search) const override;
		bool exists(const searchpath &search) const override;
		bool is_directory(const searchpath &search) const override;
		bool create_directories(const searchpath &search) const override;
		interfaces::file *open_file(const searchpath &search, open_flags flags) const override;
		bool open_text_file(const searchpath &search, ucstring &str) const override;
		bool save_text_file(const searchpath &search, const ucstring &str) const override;

		void print_searchmap() const override;

	private:
		using search_map_t = unordered_map<ucstring, vector<pstring>>;
		search_map_t searchmap{};
	};
}

#endif