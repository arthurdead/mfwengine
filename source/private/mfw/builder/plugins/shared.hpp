#ifndef __MFW_PRIVATE_BUILDER_PLUGINS_SHARED_H
#define __MFW_PRIVATE_BUILDER_PLUGINS_SHARED_H

#pragma once

#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/core/serializable.hpp>
#include <private/mfw/builder/references/tool_reference.hpp>

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
	
	const core::univalue *find_output_option(const core::serializable &options, const core::serializable &names);
	pstring get_output_path(const tool_reference &tool, const core::serializable &options);
	
	#define __MFW_QUOTE_STR_BEGIN(var, quote_var, check) \
		bool quote_var{var.find(u8' ', 0) != ucstring::npos}; \
		if(check) { \
			str += u8'"'; \
		}
			
	#define __MFW_QUOTE_STR_END(var, quote_var, check) \
		if(check) { \
			str += u8'"'; \
		} \
		
	#define __MFW_QUOTE_STR_NAME(var, quote_var) \
		__MFW_QUOTE_STR_BEGIN(var, quote_var, quote_var) \
		str += var; \
		__MFW_QUOTE_STR_END(var, quote_var, quote_var)
		
	#define __MFW_QUOTE_STR_VALUE(var, quote_var) \
		__MFW_QUOTE_STR_BEGIN(var, quote_var, quote_var || is_file) \
		if(is_file && !drive.empty()) { \
			str += drive; \
		} \
		str += var; \
		__MFW_QUOTE_STR_END(var, quote_var, quote_var || is_file)

	#define __MFW_APPEND_VALUE(sep) \
		if(!value.empty()) { \
			if(sep != u8'\0') { \
				str += sep; \
			} \
			const ucstring &val_str{value.get_string()}; \
			__MFW_QUOTE_STR_VALUE(val_str, quote_value) \
		}
}

#endif