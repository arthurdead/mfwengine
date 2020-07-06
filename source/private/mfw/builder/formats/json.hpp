#ifndef __MFW_PRIVATE_BUILDER_FORMATS_JSON_HPP
#define __MFW_PRIVATE_BUILDER_FORMATS_JSON_HPP

#pragma once

#ifndef RAPIDJSON_ERROR_STRING
#define RAPIDJSON_ERROR_STRING(x) u8##x
#endif

#if MFW_COMPILER == MFW_COMPILER_MSVC
	MFW_WARNING_PUSH()
	MFW_WARNING_DISABLE(4464 4668 4365 4061)
	MFW_WARNING_DISABLE(26451 26439 4634 6313)
	MFW_WARNING_DISABLE(5054)
#endif

#pragma push_macro("new")
#undef new

#include <rapidjson/rapidjson.h>
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>

#pragma pop_macro("new")

#if MFW_COMPILER == MFW_COMPILER_MSVC
	MFW_WARNING_POP()
#endif

#include <public/mfw/core/filesystem_interface.hpp>

namespace mfw::builder
{
	namespace json
	{
	#if defined __MFW_ENABLE_CUSTOM_ALLOCATORS && 0
		class baseallocator
		{
		public:
			static const bool kNeedFree = true;

			void *Malloc(size_t size)
			{
				return allocate(size, MFW_MEMORY_DEFAULT_ALIGNMENT, MFW_MEMORY_DEFAULT_OFFSET, false, MFW_MEMORY_DEFAULT_BLOCK, __FILE__, __LINE__, nullptr);
			}

			void *Realloc(void *originalPtr, size_t originalSize, size_t newSize)
			{
				return reallocate(originalPtr, newSize, MFW_MEMORY_DEFAULT_ALIGNMENT, MFW_MEMORY_DEFAULT_OFFSET, false, MFW_MEMORY_DEFAULT_BLOCK, __FILE__, __LINE__, nullptr);
			}

			static void Free(void *ptr)
			{
				deallocate(ptr, 0, MFW_MEMORY_DEFAULT_ALIGNMENT, MFW_MEMORY_DEFAULT_OFFSET, false, MFW_MEMORY_DEFAULT_BLOCK, __FILE__, __LINE__, nullptr);
			}
		};
	#else
		using baseallocator = ::rapidjson::CrtAllocator;
	#endif

		using char_traits = ::rapidjson::UTF8<ucchar_t>;
		using memorypoolallocator = ::rapidjson::MemoryPoolAllocator<baseallocator>;
		using document = ::rapidjson::GenericDocument<char_traits, memorypoolallocator, baseallocator>;
		using value = document::ValueType;
		using member_iterator = document::MemberIterator;
		using const_member_iterator = document::ConstMemberIterator;
		using parse_result = ::rapidjson::ParseResult;
		using ::rapidjson::GetParseError_En;
		using string_buffer = ::rapidjson::GenericStringBuffer<char_traits>;
		template <typename T>
		using pretty_writer = ::rapidjson::PrettyWriter<T, char_traits, char_traits, baseallocator, 0>;
		template <typename T>
		using writer = ::rapidjson::Writer<T, char_traits, char_traits, baseallocator, 0>;

		template <typename T>
	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		using default_writer = pretty_writer<T>;
	#else
		using default_writer = writer<T>;
	#endif

		class file : public string_buffer, public default_writer<string_buffer>
		{
		public:
			using super = default_writer<string_buffer>;

			file(const file &) = delete;
			file(file &&) = default;
			file()
				: string_buffer{}, default_writer<string_buffer>{*static_cast<string_buffer *>(this)}
			{}

			bool Key(ucstring_view name)
			{ return super::Key(name.data(), name.length(), false); }
			bool String(ucstring_view name)
			{ return super::String(name.data(), name.length(), false); }

			bool Key(const ucstring &name)
			{ return super::Key(name); }
			bool String(const ucstring &name)
			{ return super::String(name); }

			bool Key(const pstring &name)
			{ return super::Key(uc_str(name), name.native().length(), false); }
			bool String(const pstring &name)
			{ return super::String(uc_str(name), name.native().length(), false); }

			void save(const core::searchpath &search)
			{
				ucstring text{GetString(), GetLength()+1};
				core::interfaces::filesystem::instance().save_text_file(search, text);
			}
		};
	}

	inline void to_string(const json::value &val, ucstring &str)
	{
		json::string_buffer buffer{};
		json::default_writer<json::string_buffer> wri{buffer};
		val.Accept(wri);

		str = buffer.GetString();
	}
}

#endif