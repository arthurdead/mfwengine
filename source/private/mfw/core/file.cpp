#include <private/mfw/core/file.hpp>
#include <public/mfw/stl/stdint.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <sys/stat.h>
	#include <unistd.h>
	#include <climits>
	#include <public/mfw/stl/format.hpp>
#endif

namespace mfw::core
{
	core::file::~file()
	{
		if(handle != invalid_handle) {
		#if MFW_OS == MFW_OS_WINDOWS
			CloseHandle(handle);
		#elif MFW_OS == MFW_OS_LINUX
			fclose(handle);
		#else
			#error
		#endif
		}
	}

#if MFW_OS == MFW_OS_LINUX
	size_t core::file::seek_handle(handle_t hndl, ssize_t offset_, core::seek origin)
	{
		int32_t seek{0};
		switch(origin)
		{
			case seek::begin: { seek = SEEK_SET; break; }
			case seek::current: { seek = SEEK_CUR; break; }
			case seek::end: { seek = SEEK_END; break; }
		}

		fseeko64(hndl, offset_, seek);

		return ftello64(hndl);
	}
#endif

	void core::file::seek(ssize_t offset_, core::seek origin)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		uint32_t seek{0};
		switch(origin)
		{
			case seek::begin: { seek = FILE_BEGIN; break; }
			case seek::current: { seek = FILE_CURRENT; break; }
			case seek::end: { seek = FILE_END; break; }
		}

		LARGE_INTEGER large{};
		large.QuadPart = offset_;

		LARGE_INTEGER ret{};

		SetFilePointerEx(handle, large, &ret, seek);

		offset = ret.QuadPart;
	#elif MFW_OS == MFW_OS_LINUX
		offset = seek_handle(handle, offset_, origin);
	#else
		#error
	#endif

		seek_ = origin;
	}

#if MFW_OS == MFW_OS_LINUX
	ssize_t core::file::tell_handle(handle_t hndl)
	{
		return ftello64(hndl);
	}
#endif

	ssize_t core::file::tell()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		LARGE_INTEGER large{};
		LARGE_INTEGER ret{};

		SetFilePointerEx(handle, large, &ret, FILE_CURRENT);

		return static_cast<size_t>(ret.QuadPart);
	#elif MFW_OS == MFW_OS_LINUX
		return tell_handle(handle);
	#else
		#error
	#endif
	}
	
#if MFW_OS == MFW_OS_LINUX
	namespace __file_internal
	{
		static pstring get_handle_path(file::handle_t hndl)
		{
			ucstring desc{};
			format(desc, u8"/proc/self/fd/{}"_sv, fileno(hndl));
			char filename[PATH_MAX]{'\0'};
			ssize_t len{readlink(c_str(desc), filename, stl::size(filename))};
			pstring filepath{MFW_PATH_FROM_CHARARRAY(filename, len)};
			return filepath;
		}
	}

	size_t core::file::get_handle_size(file::handle_t hndl)
	{
		pstring filepath{__file_internal::get_handle_path(hndl)};
		uintmax_t st_size{stl::filesystem::file_size(filepath)};
		if(st_size == 0) {
			ssize_t offset_{tell_handle(hndl)};
			byte data{0};
			while(true) {
				size_t read{fread(&data, sizeof(byte), 1, hndl)};
				if(read != 1 || feof(hndl) != 0 || ferror(hndl) != 0) {
					break;
				}
				st_size++;
			}
			if(offset_ != -1) {
				seek_handle(hndl, offset_, seek::begin);
			}
		}
		return st_size;
	}
#endif

	size_t core::file::size()
	{
		return stl::filesystem::file_size(path_);
	}

	size_t core::file::read(void *data, size_t elesiz, size_t size_)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		size_t total{elesiz * size_};
		#if MFW_COMPILER == MFW_COMPILER_MSVC
			#pragma warning(suppress: 6031)
		#endif
		ReadFile(handle, data, static_cast<uint32_t>(total), nullptr, nullptr);
	#elif MFW_OS == MFW_OS_LINUX
		size_t read{fread(data, elesiz, size_, handle)};
	#else
		#error
	#endif

		offset += read;

		return read;
	}

	size_t core::file::write(const void *data, size_t elesiz, size_t size_)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		size_t total{elesiz * size_};
		WriteFile(handle, data, static_cast<uint32_t>(total), nullptr, nullptr);
	#elif MFW_OS == MFW_OS_LINUX
		size_t wrote{fwrite(data, elesiz, size_, handle)};
	#else
		#error
	#endif

		offset += wrote;

		return wrote;
	}

	void core::file::rewind()
	{
		seek(-offset, seek::current);
	}

	void core::file::flush()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		FlushFileBuffers(handle);
	#elif MFW_OS == MFW_OS_LINUX
		fflush(handle);
	#else
		#error
	#endif
	}

	void core::file::resize(size_t size_)
	{
		seek(static_cast<ssize_t>(size_), seek::begin);
	#if MFW_OS == MFW_OS_WINDOWS
		SetEndOfFile(handle);
	#elif MFW_OS == MFW_OS_LINUX
		ftruncate64(fileno(handle), size_);
	#else
		#error
	#endif
		rewind();
	}
}