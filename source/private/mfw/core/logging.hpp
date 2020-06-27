#ifndef __MFW_PRIVATE_CORE_LOGGING_H
#define __MFW_PRIVATE_CORE_LOGGING_H

#pragma once

#include <public/mfw/core/logging_interface.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/core/expression_parser_interface.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#if MFW_OS_IS(WINDOWS)
	#include <Windows.h>
	MFW_MESSAGE("get rid of this")
#elif MFW_OS_IS(LINUX)
	#include <ncurses.h>
#endif
#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <cstdio>
#else
	#error
#endif

namespace mfw::core
{
	class logger_manager final : public interfaces::expression_parser_callbacks
	{
	private:
		friend class log_context;

	public:
		static logger_manager &instance();

		logger_manager();
		~logger_manager();

		void print(const ucstring_view &str, log_context &ctx);

		enum /*class*/ console_color : uchar_t
		{
			black,
			blue,
			green,
			aqua,
			red,
			purple,
			yellow,
			white,
			gray,
			light_blue,
			light_green,
			light_aqua,
			light_red,
			light_purple,
			light_yellow,
			bright_white,

			count,
			invalid = count,

			default_bg = black,
			default_txt = white,
		};
		MFW_CLASS_ENUM(console_color)

	private:
		struct color_rgb
		{
			byte r{0};
			byte g{0};
			byte b{0};
			
			bool valid() const {
				return (r != 0 ||
						g != 0 ||
						b != 0);
			}
		};
		color_rgb get_color_rgb(console_color conclr) const;
		void set_color_rgb(console_color conclr, const color_rgb &clr);

		console_color get_bg_color() const;
		void set_bg_color(console_color color);
		console_color get_txt_color() const;
		void set_txt_color(console_color color);
		
		enum class attributes : uint16_t
		{
			none = 0,
			bold = MFW_BIT(0),
			dim = MFW_BIT(1),
			italic = MFW_BIT(2),
			underline = MFW_BIT(3),
			slow_blink = MFW_BIT(4),
			rapid_blink = MFW_BIT(5),
			invert = MFW_BIT(6),
			hide = MFW_BIT(7),
			crossed = MFW_BIT(8),
			fraktur = MFW_BIT(9),
			font0 = MFW_BIT(10),
		};
		MFW_CLASS_ENUM_FLAGS(attributes)
		
		static void attr_to_str(attributes attr, ucstring &str);
		
		struct print_vars_t
		{
			bool sub{false};
			bool last{true};
			ssize_t ident{0};
			attributes attr{attributes::none};
			console_color txt_color{console_color::invalid};
			console_color bg_color{console_color::invalid};
		};
		
		log_context *current_ctx{nullptr};
		print_vars_t *current_vars{nullptr};
		
		void print_internal(ucstring &str, log_context &ctx, print_vars_t &vars, log_context::severity severity_, bool changed);
		
		void print_console(ucstring_view str, bool file=false);
		void print_console(ucstring_view str, log_context::severity severity_, bool file=true);
		
		using subs_t = pair<ucstring, ucstring>;
		using subs_vec_t = vector<subs_t>;
		static bool handle_vars(ucstring &str, subs_vec_t &subs);
		
		void print_sub(subs_t &sub, log_context &ctx, print_vars_t &vars, log_context::severity severity_, bool changed);
		
		static bool get_pre_spaces(ucstring &tmp, const log_context &ctx, print_vars_t &vars, bool changed);
		static void get_pre_insert(ucstring &tmp, const log_context &ctx);
		static void get_pre_insert_emoji(ucstring &tmp, log_context::severity severity_);

		bool get_variable(const ucstring_view &, type_holder &) const;

		interfaces::file *log_file{nullptr};
		
	#if MFW_OS == MFW_OS_WINDOWS
		HWND console_wnd{nullptr};
		HANDLE stdout_handle{nullptr};
		HMENU close_menu{nullptr};

		FILE *stdin_{nullptr};
		FILE *stdout_{nullptr};
		FILE *stderr_{nullptr};

		ucstring old_title{};
		uint32_t old_mode{0};

		console_color old_bgclr{console_color::black};
		console_color old_txtclr{console_color::white};

		CONSOLE_SCREEN_BUFFER_INFOEX old_screenbuffer{};
	#elif MFW_OS == MFW_OS_LINUX
		struct screenbuffer_t
		{
			color_rgb color_table[console_color::count]{};
			//attributes flags{attributes::none};
			console_color bg_color{console_color::invalid};
			console_color fg_color{console_color::invalid};
		};
		screenbuffer_t screenbuffer{};
	#endif
	};
}

#endif