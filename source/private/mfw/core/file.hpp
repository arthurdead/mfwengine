#ifndef MFW_PRIVATE_CORE_FILE_HPP
#define MFW_PRIVATE_CORE_FILE_HPP

#pragma once

#include <public/mfw/core/file_interface.hpp>
#include <private/mfw/core/filesystem.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>

#if MFW_OS == MFW_OS_WINDOWS
	MFW_MESSAGE("remove this eventually")
	#include <Windows.h>
#endif

namespace mfw::core
{
	class file final : public interfaces::file
	{
	public:
		friend core::filesystem;

	#if MFW_OS == MFW_OS_WINDOWS
		using handle_t = HANDLE;
		static inline constexpr handle_t invalid_handle{INVALID_HANDLE_VALUE};
	#elif MFW_OS == MFW_OS_LINUX
		using handle_t = FILE *;
		static inline constexpr handle_t invalid_handle{nullptr};
	#else
		#error
	#endif

		file(handle_t hndl, const pstring &path)
			: handle{hndl}, path_{path} {}
		~file() override;

		const pstring &path() const override { return path_; }
		void seek(ssize_t offset, core::seek origin) override;
		ssize_t tell() override;
		void rewind() override;
		size_t read(void *data, size_t elesiz, size_t size_) override;
		size_t write(const void *data, size_t elesiz, size_t size_) override;
		void flush() override;
		void resize(size_t size_) override;
		size_t size() override;
		
	#if MFW_OS == MFW_OS_LINUX
		static ssize_t tell_handle(handle_t hndl);
		static size_t seek_handle(handle_t hndl, ssize_t offset, core::seek origin);
		static size_t get_handle_size(handle_t hndl);
	#endif

	private:
		handle_t handle{invalid_handle};
		pstring path_{};
		core::seek seek_{};
		ssize_t offset{};
	};
}

#endif