#ifndef __MFW_PRIVATE_BUILDER_PLUGINS_SHARED_H
#define __MFW_PRIVATE_BUILDER_PLUGINS_SHARED_H

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>

namespace mfw::builder
{
	struct linker_info_t
	{
		enum class type_t : uchar_t
		{
			unknown,
			ld,
			link,
		};
		
		type_t type{type_t::unknown};
		
		bool operator==(type_t other) { return type == other; }
		
		enum class flags_t : uchar_t
		{
			none,
			unix_,
			gold,
			bfd,
			lld,
			musl,
		};
		MFW_CLASS_ENUM_FLAGS(flags_t)
		
		flags_t flags{flags_t::none};
		
		bool operator&(flags_t flag) { return bool_cast(flags & flag); }
		
		void clear() {
			type = type_t::unknown;
			flags = flags_t::none;
		}
	};
	
	linker_info_t get_linker_info(const pstring &path);
	linker_info_t get_linker_info(const ucstring &path);
	
	struct compiler_info_t
	{
		enum class type_t : uchar_t
		{
			unknown,
			gcc,
			clang,
			msvc,
		};
		
		type_t type{type_t::unknown};
		
		bool operator==(type_t other) { return type == other; }
		
		enum class flags_t : uchar_t
		{
			none,
			unix_,
			clang,
			musl,
			ccache,
		};
		MFW_CLASS_ENUM_FLAGS(flags_t)
		
		flags_t flags{flags_t::none};
		
		bool operator&(flags_t flag) { return bool_cast(flags & flag); }
		
		void clear() {
			type = type_t::unknown;
			flags = flags_t::none;
		}
	};
	
	compiler_info_t get_compiler_info(const pstring &path);
	compiler_info_t get_compiler_info(const ucstring &path);
}

#endif