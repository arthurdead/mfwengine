#ifndef __MFW_PUBLIC_CORE_LOGGING_INTERFACE_HPP
#define __MFW_PUBLIC_CORE_LOGGING_INTERFACE_HPP

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
	enum class log_severity : uchar_t
	{
		unknown,
		warning,
		error,
		success,
		info,
	};

	MFW_CORE_API uint8_t MFW_CORE_CALL error_count();
	MFW_CORE_API uint8_t MFW_CORE_CALL warning_count();

	class log_context
	{
	public:
		log_context() = default;
		log_context(const ucstring &name) { print_vars_.name_ = name; }
		log_context(const pstring &name) { print_vars_.name_ = as_string<ucstring>(name); }

		using severity = log_severity;

		void set_name(const ucstring &name) { print_vars_.name_ = name; }
		const ucstring &name() const { return print_vars_.name_; }

		void clear_tag() { print_vars_.tag.clear(); }
		void set_tag(const ucstring &tag_) { print_vars_.tag = tag_; }
		void append_tag(const ucstring &tag_) {
			if(!print_vars_.tag.empty()) {
				print_vars_.tag += u8'/';
			}
			print_vars_.tag += tag_;
		}
		const ucstring &get_tag() const { return print_vars_.tag; }

		void add_ident(size_t i = 1) { print_vars_.ident += i; }
		void remove_ident(size_t i = 1) { print_vars_.ident -= i; }
		void set_ident(size_t i) { print_vars_.ident = i; }
		void clear_ident() { print_vars_.ident = 0; }
		size_t get_ident() const { return print_vars_.ident; }

		void set_severity(severity sev) { print_vars_.severity_ = sev; }
		severity get_severity() const { return print_vars_.severity_; }

		bool paused() const { return paused_; }
		void pause() { paused_ = true; }
		MFW_CORE_API void MFW_CORE_CALL resume();
		void clear_history() { history.clear(); }

		MFW_CORE_API void MFW_CORE_CALL print(const ucstring_view &str);

		template <typename ...Args>
		bool print(const ucstring_view &fmtstr, Args &&... args)
		{
			ucstring str{};
			if(!format(str, fmtstr, forward<Args>(args)...)) {
				return false;
			}
			print(str);
			return true;
		}

		void warning(const ucstring_view &str) {
			set_severity(severity::warning);
			print(str);
		}
		void error(const ucstring_view &str) {
			set_severity(severity::error);
			print(str);
		}
		void success(const ucstring_view &str) {
			set_severity(severity::success);
			print(str);
		}
		void info(const ucstring_view &str) {
			set_severity(severity::info);
			print(str);
		}

		template <typename ...Args>
		bool warning(const ucstring_view &fmtstr, Args &&... args) {
			set_severity(severity::warning);
			return print(fmtstr, forward<Args>(args)...);
		}
		template <typename ...Args>
		bool error(const ucstring_view &fmtstr, Args &&... args) {
			set_severity(severity::error);
			return print(fmtstr, forward<Args>(args)...);
		}
		template <typename ...Args>
		bool success(const ucstring_view &fmtstr, Args &&... args) {
			set_severity(severity::success);
			return print(fmtstr, forward<Args>(args)...);
		}
		template <typename ...Args>
		bool info(const ucstring_view &fmtstr, Args &&... args) {
			set_severity(severity::info);
			return print(fmtstr, forward<Args>(args)...);
		}

	private:
		struct print_vars_t
		{
			ucstring name_{};
			ucstring tag{};
			size_t ident{0};
			severity severity_{severity::unknown};
		};
		struct history_vars_t : print_vars_t
		{
			history_vars_t &operator=(const print_vars_t &other);

			ucstring print{};
		};
		print_vars_t print_vars_{};
		bool paused_{false};
		vector<history_vars_t> history{};
	};

	#define __MFW_DECLARE_LOG_CONTEXT(api, call, name, ...) \
		class __##name##__log_context : public ::mfw::core::log_context { \
		public: \
			__##name##__log_context() : ::mfw::core::log_context{__VA_ARGS__} {} \
		}; \
		MFW_DECLARE_GLOBAL_ALLOCATOR(name, __##name##__log_context) \
		api ::mfw::core::log_context & call name() { return __##name##_global_allocator.instance(); }

	#define __MFW_EXTERN_LOG_CONTEXT(api, call, name) \
		extern api ::mfw::core::log_context & call name();

	#define MFW_DECLARE_LOG_CONTEXT(name, ...) __MFW_DECLARE_LOG_CONTEXT(MFW_NOTHING, MFW_NOTHING, name, __VA_ARGS__)
	#define MFW_EXTERN_LOG_CONTEXT(name) __MFW_EXTERN_LOG_CONTEXT(MFW_NOTHING, MFW_NOTHING, name)
}

#endif