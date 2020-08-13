#ifndef MFW_PUBLIC_CORE_LOGGING_INTERFACE_HPP
#define MFW_PUBLIC_CORE_LOGGING_INTERFACE_HPP

#pragma once

#include <public/mfw/core/core.hpp>
#include <public/mfw/stl/tuple.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/format.hpp>

namespace mfw::core
{
	enum class log_severity : stl::uchar_t
	{
		unknown,
		warning,
		error,
		success,
		info,
	};

	MFW_CORE_API stl::uint8_t MFW_CORE_CALL error_count() noexcept;
	MFW_CORE_API stl::uint8_t MFW_CORE_CALL warning_count() noexcept;

	#define _MFW_DECLARE_LOG_CONTEXT_IMPL(api, call, name, ...) \
		class __##name##__log_context final : public ::mfw::core::log_context \
		{ \
		public: \
			__##name##__log_context() noexcept \
				: ::mfw::core::log_context{__VA_ARGS__} {} \
		}; \
		MFW_DECLARE_GLOBAL_ALLOCATOR(name, __##name##__log_context) \
		api ::mfw::core::log_context & call name() noexcept \
		{ return __##name##_global_allocator.instance(); }

	#define _MFW_EXTERN_LOG_CONTEXT_IMPL(api, call, name) \
		extern api ::mfw::core::log_context & call name();

	#define MFW_DECLARE_LOG_CONTEXT(name, ...) _MFW_DECLARE_LOG_CONTEXT_IMPL(MFW_NOTHING, MFW_NOTHING, name, __VA_ARGS__)
	#define MFW_EXTERN_LOG_CONTEXT(name) _MFW_EXTERN_LOG_CONTEXT_IMPL(MFW_NOTHING, MFW_NOTHING, name)
}

#endif