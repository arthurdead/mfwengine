#ifndef __MFW_PUBLIC_CORE_FILE_INTERFACE_H
#define __MFW_PUBLIC_CORE_FILE_INTERFACE_H

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/filesystem.hpp>

namespace mfw::core
{
	enum class seek : uchar_t
	{
		begin,
		current,
		end,
	};

	namespace interfaces
	{
		class file
		{
		public:
			virtual ~file() = default;
			virtual const pstring &path() const = 0;
			virtual void seek(ssize_t offset, seek origin) = 0;
			virtual ssize_t tell() = 0;
			virtual void rewind() = 0;
			virtual size_t read(void *data, size_t elesiz, size_t size_) = 0;
			virtual size_t write(const void *data, size_t elesiz, size_t size_) = 0;
			virtual void flush() = 0;
			virtual void resize(size_t size_) = 0;
			virtual size_t size() = 0;

			template <typename T>
			size_t read(T *data, size_t size_)
			{ return read(data, sizeof(T), size_); }

			template <typename T, size_t size_>
			size_t read(T (&data)[size_])
			{ return read(data, sizeof(T), size_); }

			template <typename T>
			size_t write(const T *data, size_t size_)
			{ return write(data, sizeof(T), size_); }

			template <typename T, size_t size_>
			size_t write(const T (&data)[size_])
			{ return write(data, sizeof(T), size_); }

			size_t write(ucstring_view str)
			{ return write(str.data(), str.length()); }
			
			template <typename T>
			size_t read(vector<T> &vec) {
				vec.resize(size());
				return read(vec.data(), sizeof(T), vec.size());
			}
		};
	}
}

#endif