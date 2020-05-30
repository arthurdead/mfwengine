#ifndef __MFW_PRIVATE_CORE_RTTR_H
#define __MFW_PRIVATE_CORE_RTTR_H

#pragma once

#include <public/mfw/core/rttr_interface.hpp>
#ifdef __MFW_USE_ASMJIT
	#include <private/mfw/core/asm_builder.hpp>
#endif
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>

namespace mfw::core
{
	class rttr final : public interfaces::rttr
	{
	public:
		static rttr &instance();

	public:
		bool register_class_info(const class_info &info) override;
		bool register_func_info(const func_info &info) override;

		const class_info *find_class_info(const ucstring_view &name) override;
		const func_info *find_function_info(const ucstring_view &name) const override;

		bool call_function(const func_info &info, const vector<type_holder> &args, type_holder &result) const override;

		const ucstring &clean_name(const ::std::type_info &info) const override;

		const func_info *find_internal_function(internal_func which, const class_info &info) const;
		bool call_internal_function(internal_func which, void *dst, const void *src, const class_info &info) const override;

		void *allocate(const class_info &info) const override;
		bool deallocate(void *ptr, const class_info &info) const override;

	private:
	#ifdef __MFW_USE_ASMJIT
		class func_info_jit : public func_info
		{
		public:
			func_info_jit &operator=(const func_info &other) { reinterpret_cast<func_info &>(*this) = other; return *this; };

			bool build_signature();

			asmjit::FuncSignatureBuilder signature{};
		};

		class class_info_jit : public class_info
		{
		public:
			class_info_jit &operator=(const class_info &other) { reinterpret_cast<class_info &>(*this) = other; return *this; };

			bool build_signatures();

			vector<asmjit::FuncSignatureBuilder> signatures{};
		};

		using class_info_vec_t = vector<class_info_jit>;
		class_info_vec_t class_infos{};
		using func_info_vec_t = vector<func_info_jit>;
		func_info_vec_t function_infos{};
	#else
		using class_info_vec_t = vector<class_info>;
		class_info_vec_t class_infos{};
		using func_info_vec_t = vector<func_info>;
		func_info_vec_t function_infos{};
	#endif
	};
}

#endif