#ifndef MFW_PUBLIC_CORE_LIBRARY_HPP
#define MFW_PUBLIC_CORE_LIBRARY_HPP

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/core/searchpath.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	MFW_MESSAGE("get rid of this eventually")
	#include <Windows.h>
#endif

namespace mfw::core
{
	namespace __library_internal { struct export_collector; }

	class library
	{
	public:
	#if MFW_OS == MFW_OS_WINDOWS
		using handle_t = HMODULE;
	#elif MFW_OS == MFW_OS_LINUX
		using handle_t = void *;
	#else
		#error
	#endif

		library(const library &other) { operator=(other); }
		MFW_CORE_API library & MFW_CORE_CALL operator=(const library &other);

		library() = default;
		library(const searchpath &search) { load(search); }
		~library() { unload(); }

		MFW_CORE_API bool MFW_CORE_CALL load(const searchpath &search);
		MFW_CORE_API bool MFW_CORE_CALL unload();

		bool operator!() const { return (module_ == nullptr); }
		operator bool() const { return (module_ != nullptr); }
		operator const pstring &() const { return fullpath; }
		operator handle_t() const { return module_; }

		bool valid() const { return (module_ != nullptr); }
		const pstring &path() const { return fullpath; }
		handle_t handle() const { return module_; }

		MFW_CORE_API ucstring MFW_CORE_CALL symbol_name(size_t index) const;
		MFW_CORE_API const void * MFW_CORE_CALL symbol(const ucstring_view &name) const;
		MFW_CORE_API const void * MFW_CORE_CALL symbol(size_t index) const;
		MFW_CORE_API size_t MFW_CORE_CALL symbol_count() const;

		template <typename T>
		T symbol(const ucstring_view &name) const
		{ return reinterpret_cast<T>(const_cast<void *>(symbol(name))); }

		template <typename T>
		T symbol(size_t index) const
		{ return reinterpret_cast<T>(const_cast<void *>(symbol(index))); }

		static MFW_CORE_API bool MFW_CORE_CALL add_directory(const searchpath &search);
		static MFW_CORE_API bool MFW_CORE_CALL remove_directory(const searchpath &search);
		static MFW_CORE_API bool MFW_CORE_CALL remove_all_directories();

		static MFW_CORE_API library * MFW_CORE_CALL load_library(const searchpath &search);
		static MFW_CORE_API bool MFW_CORE_CALL unload_library(const searchpath &search);
		static MFW_CORE_API bool MFW_CORE_CALL unload_all_libraries();

		static MFW_CORE_API library & MFW_CORE_CALL add_to_list(library &lib);
		static MFW_CORE_API bool MFW_CORE_CALL remove_from_list(library &lib);
		
	#if MFW_OS_IS(LINUX)
		enum class exports_flags_t : uchar_t
		{
			none = 0,
			functions = MFW_BIT(0),
			objects = MFW_BIT(1),
			hidden = MFW_BIT(2),
			public_ = MFW_BIT(3),
			weak = MFW_BIT(4),
			local = MFW_BIT(5),
			global = MFW_BIT(6),
			default_ = public_|global|functions,
			all = functions|objects|hidden|public_|weak|local|global,
		};
		MFW_CLASS_ENUM_FLAGS(exports_flags_t)
	#endif

		struct export_t
		{
			friend struct ::mfw::core::__library_internal::export_collector;
			
		private:
			const void *ptr{nullptr};
			ucstring name_{};
			size_t index_{0};
		#if MFW_OS_IS(LINUX)
			exports_flags_t flags{exports_flags_t::none};
			size_t value_{0};
		public:
			bool function() const { return bool_cast(flags & exports_flags_t::functions); }
			bool object() const { return bool_cast(flags & exports_flags_t::objects); }
			bool hidden() const { return bool_cast(flags & exports_flags_t::hidden); }
			bool public_() const { return bool_cast(flags & exports_flags_t::public_); }
			bool weak() const { return bool_cast(flags & exports_flags_t::weak); }
			bool local() const { return bool_cast(flags & exports_flags_t::local); }
			bool global() const { return bool_cast(flags & exports_flags_t::global); }
		#endif

		public:
			const ucstring &name() const { return name_; }
			size_t index() const { return index_; }
		};
		
		using export_vec_t = vector<export_t>;

		static MFW_CORE_API bool MFW_CORE_CALL get_library_exports(const searchpath &search, export_vec_t &exports
	#if MFW_OS_IS(LINUX)
			,exports_flags_t flags = exports_flags_t::default_
	#endif
		);

		using library_list_t = vector<library>;

	#if MFW_OS == MFW_OS_WINDOWS
		using cookie_list_t = unordered_map<pstring, DLL_DIRECTORY_COOKIE>;
	#endif

	private:
		static library_list_t::iterator find_library(const pstring &path);
		static library_list_t::iterator find_library(handle_t dll);

		handle_t module_{nullptr};
		pstring fullpath{};

		enum class flags : uchar_t
		{
			dont_unload = MFW_BIT(0),
			in_list = MFW_BIT(1),
		};
		MFW_CLASS_ENUM_FLAGS(flags)

		flags flags_{};

		export_vec_t exports{};
	};
}

#endif