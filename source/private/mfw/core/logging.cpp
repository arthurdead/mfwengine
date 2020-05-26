#include <private/mfw/core/logging.hpp>
#include <public/mfw/core/core.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/defines.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/pch_literals.hpp>
#include <private/mfw/core/expression_parser.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <ncurses.h>
#endif
#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#include <cstdio>
#else
	#error
#endif

namespace mfw::core
{
	MFW_DECLARE_LOG_CONTEXT(log_logging, u8"core/logging"_p)

	MFW_DECLARE_GLOBAL_ALLOCATOR(loggermanager, logger_manager)

	logger_manager &logger_manager::instance() {
		return __loggermanager_global_allocator.instance();
	}

	bool logger_manager::handle_vars(ucstring &str, subs_vec_t &subs)
	{
		ucstring::const_iterator it{str.cend()};
		ucstring::const_iterator last{it};
		while(it != str.cbegin()) {
			bool at_start{false};
			if(it == str.cbegin()) {
				at_start = true;
			}
			bool isvar{false};
			if(*it == u8'}') {
				isvar = true;
				if(!at_start) {
					if(*(it - 1) == u8'\\' ||
						*(it - 1) == u8'{' ||
						*(it - 1) == u8'\n') {
						isvar = false;
					}
				}
				if(it != str.cend()) {
					if(*(it + 1) == u8'\n') {
						isvar = false;
					}
				}
			}
			if(isvar) {
				ucstring::const_iterator end{it+1};
				ucstring::const_iterator start{it};
				it--;
				ucstring value{};
				while(true) {
					if(*it == u8'{') {
						if(*(it - 1) == u8'\\' ||
							*(it - 1) != u8'#') {
							isvar = false;
							if(!at_start) {
								it = start-1;
							} else {
								it = start;
							}
							break;
						}
						start = it-1;
						if(!at_start) {
							it-=2;
						}
						break;
					}
					value.insert(0, 1, *it);
					if(!at_start) {
						it--;
					}
				}
				if(!isvar) {
					continue;
				}
				ssize_t last_i{distance(str.cbegin(), last)};
				ssize_t end_i{distance(str.cbegin(), end)};
				ssize_t len{last_i-end_i};
				ucstring sub{str.substr(end_i, len)};
				str.erase(start, last);
				subs.emplace_back(subs_t{value, sub});
				last = start;
			} else {
				it--;
			}
		}
		
		if(!subs.empty()) {
			reverse(subs.begin(), subs.end());
			return true;
		} else {
			return false;
		}
	}
	
	bool logger_manager::get_variable(const ucstring_view &name, type_holder &var) const
	{
		if(name == u8"ident"_sv) {
			var.deduce(current_ctx->get_ident());
			return true;
		}
		
		MFW_DEBUGBREAK();
		return false;
	}
	
	void logger_manager::print_sub(subs_t &sub, log_context &ctx, print_vars_t &vars, log_context::severity severity_)
	{
		const ucstring &var{sub.first};
		if(var == u8"black"_sv) {
			vars.txt_color = console_color::black;
		} else if(var == u8"blue"_sv) {
			vars.txt_color = console_color::blue;
		} else if(var == u8"green"_sv) {
			vars.txt_color = console_color::green;
		} else if(var == u8"aqua"_sv) {
			vars.txt_color = console_color::aqua;
		} else if(var == u8"red"_sv) {
			vars.txt_color = console_color::red;
		} else if(var == u8"purple"_sv) {
			vars.txt_color = console_color::purple;
		} else if(var == u8"yellow"_sv) {
			vars.txt_color = console_color::yellow;
		} else if(var == u8"white"_sv) {
			vars.txt_color = console_color::white;
		} else if(var == u8"gray"_sv) {
			vars.txt_color = console_color::gray;
		} else if(var == u8"light_blue"_sv) {
			vars.txt_color = console_color::light_blue;
		} else if(var == u8"light_green"_sv) {
			vars.txt_color = console_color::light_green;
		} else if(var == u8"light_aqua"_sv) {
			vars.txt_color = console_color::light_aqua;
		} else if(var == u8"light_red"_sv) {
			vars.txt_color = console_color::light_red;
		} else if(var == u8"light_purple"_sv) {
			vars.txt_color = console_color::light_purple;
		} else if(var == u8"light_yellow"_sv) {
			vars.txt_color = console_color::light_yellow;
		} else if(var == u8"bright_white"_sv) {
			vars.txt_color = console_color::bright_white;
		} else if(var == u8"reset"_sv) {
			vars.txt_color = console_color::invalid;
		} else {
			univalue res{};
			if(expression_parser::instance().parse(var, res, this)) {
				MFW_DEBUGBREAK();
			} else {
				return;
			}
		}
		print_internal(sub.second, ctx, vars, severity_);
	}
	
	namespace __logging_internal
	{
		static uint8_t errors{0};
		static uint8_t warnings{0};
	}
	
	MFW_CORE_API uint8_t MFW_CORE_CALL error_count()
	{
		return __logging_internal::errors;
	}
	
	MFW_CORE_API uint8_t MFW_CORE_CALL warning_count()
	{
		return __logging_internal::warnings;
	}
	
	void logger_manager::print(const ucstring_view &str, log_context &ctx)
	{
		current_ctx = &ctx;
		
		if(str.empty()) {
			return;
		}
		
		log_context::severity severity_{ctx.get_severity()};
		
		static log_context::severity last_severity{log_context::severity::unknown};
		if(last_severity != severity_) {
			if(severity_ == log_context::severity::warning) {
				__logging_internal::warnings++;
			} else if(severity_ == log_context::severity::error) {
				__logging_internal::errors++;
			}
		}
		last_severity = severity_;
		
		ucstring fixedstr{str};
		
		print_vars_t vars{};
		
		subs_vec_t subs{};
		if(handle_vars(fixedstr, subs)) {
			vars.last = false;
			subs_vec_t::iterator it{subs.begin()};
			if(!fixedstr.empty()) {
				print_internal(fixedstr, ctx, vars, severity_);
			} else {
				subs_vec_t::iterator tmp{it};
				it++;
				bool end{it == subs.end()};
				if(end) {
					vars.last = true;
				}
				print_sub(*tmp, ctx, vars, severity_);
				if(end) {
					return;
				}
			}
			vars.sub = true;
			subs_vec_t::iterator end{subs.end()-1};
			while(it != end) {
				print_sub(*it, ctx, vars, severity_);
				it++;
			}
			it = end;
			vars.last = true;
			print_sub(*it, ctx, vars, severity_);
			return;
		} else {
			print_internal(fixedstr, ctx, vars, severity_);
		}
	}

	void logger_manager::get_pre_insert(ucstring &tmp, const log_context &ctx)
	{
		tmp.clear();

		tmp += u8"["_sv;
		tmp += ctx.name();
		tmp += u8"] "_sv;

		const ucstring &tag{ctx.get_tag()};
		if(!tag.empty()) {
			tmp += u8"- "_sv;
			tmp += tag;
			tmp += u8": "_sv;
		}
	}

	void logger_manager::get_pre_insert_emoji(ucstring &tmp, log_context::severity severity_)
	{
		tmp.clear();

		switch(severity_) {
			case log_context::severity::warning: { tmp += u8" ⚠️"_sv; break; }
			case log_context::severity::error: { tmp += u8"❌"_sv; break; }
			case log_context::severity::success: { tmp += u8" ✔️"_sv; break; }
			case log_context::severity::info: { tmp += u8"❕"_sv; break; }
			case log_context::severity::unknown: { tmp += u8"❔"_sv; break; }
		}

		tmp += u8' ';
	}

	bool logger_manager::get_pre_spaces(ucstring &tmp, const log_context &ctx, print_vars_t &vars)
	{
		tmp.clear();

		ssize_t ident{0};
		if(!vars.sub || (!vars.sub && vars.last)) {
			ident = static_cast<ssize_t>(ctx.get_ident());
		}

		if(vars.ident != 0) {
			ident += vars.ident;
		}

		if(ident > 0) {
			tmp.insert(tmp.cbegin(), ident * 2, u8' ');
			return true;
		}

		return false;
	}
	
	void logger_manager::attr_to_str(attributes attr, ucstring &str)
	{
		vector<uint16_t> ids{};
		
		bool bold{bool_cast(attr & attributes::bold)};
		if(bold) {
			ids.emplace_back(1);
		} else {
			ids.emplace_back(21);
		}
		
		bool dim{bool_cast(attr & attributes::dim)};
		if(dim) {
			ids.emplace_back(2);
		}
		
		bool fraktur{bool_cast(attr & attributes::fraktur)};
		if(fraktur) {
			ids.emplace_back(20);
		}
		
		bool italic{bool_cast(attr & attributes::italic)};
		if(italic) {
			ids.emplace_back(3);
		}
		
		if(bool_cast(attr & attributes::underline)) {
			ids.emplace_back(4);
		} else {
			ids.emplace_back(24);
		}
		
		bool slow_blink{bool_cast(attr & attributes::slow_blink)};
		if(slow_blink) {
			ids.emplace_back(5);
		}
		
		bool rapid_blink{bool_cast(attr & attributes::rapid_blink)};
		if(rapid_blink) {
			ids.emplace_back(6);
		}
		
		if(bool_cast(attr & attributes::invert)) {
			ids.emplace_back(7);
		} else {
			ids.emplace_back(27);
		}
		
		if(bool_cast(attr & attributes::hide)) {
			ids.emplace_back(8);
		} else {
			ids.emplace_back(28);
		}
		
		if(bool_cast(attr & attributes::crossed)) {
			ids.emplace_back(9);
		} else {
			ids.emplace_back(29);
		}
		
		if(!slow_blink && !rapid_blink) {
			ids.emplace_back(25);
		}
		
		if(!italic && !fraktur) {
			ids.emplace_back(23);
		}
		
		if(!bold && !dim) {
			ids.emplace_back(22);
		}
		
		if(!ids.empty()) {
			str += u8"\x1b["_sv;
			ucstring tmp{};
			for(const uint16_t &i : ids) {
				stl::to_string(i, tmp);
				str += tmp;
				str += u8';';
			}
			str.pop_back();
			str += u8'm';
		}
	}

	void logger_manager::print_internal(ucstring &fixedstr, log_context &ctx, print_vars_t &vars, log_context::severity severity_)
	{
		current_vars = &vars;
		
		if(fixedstr.empty()) {
			return;
		}

		static bool had_ident_change{false};
		bool ident_change{(vars.ident != 0 && vars.sub) || (vars.ident == 0 && vars.sub && had_ident_change)};
		had_ident_change = ident_change;

		ucstring pre_insert{};
		bool did_pre_insert{!vars.sub || ident_change};
		if(did_pre_insert) {
			get_pre_insert(pre_insert, ctx);
		}

		ucstring tmp{};
		get_pre_spaces(tmp, ctx, vars);
		pre_insert += tmp;

		if(ident_change) {
			print_console(u8"\n\r"_sv, true);
		}
		
		if(did_pre_insert) {
			get_pre_insert_emoji(tmp, severity_);
			print_console(tmp, severity_, false);
		}
		print_console(pre_insert, severity_);

		bool endsinnewline{false};
		if(*(fixedstr.cend()-1) == u8'\n') {
			endsinnewline = true;
		}

		bool newline{false};

		size_t pos{0};
		while(true) {
			pos = fixedstr.find(u8'\n', 1);
			if(pos == ucstring::npos || pos >= fixedstr.length()) {
				break;
			}

			bool ignore_insert{false};
			if(endsinnewline) {
				if(pos == fixedstr.length()-1) {
					ignore_insert = true;
				}
			}
			
			ucstring sub{fixedstr.substr(0, pos)};
			fixedstr.erase(0, pos+1);
			
			bool was_sub{vars.sub};
			vars.sub = true;
			print_internal(sub, ctx, vars, severity_);
			vars.sub = was_sub;
			newline = true;
			
			if(fixedstr.empty()) {
				return;
			} else if(fixedstr.size() == 1 && fixedstr[0] == u8'\n') {
				print_console(u8"\n"_sv, true);
				return;
			}

			if(!ignore_insert) {
				if(!did_pre_insert) {
					get_pre_insert(tmp, ctx);
					pre_insert.insert(0, tmp);
					did_pre_insert = true;
				}
				get_pre_insert_emoji(tmp, severity_);
				print_console(tmp, severity_, false);
				print_console(pre_insert, severity_);
			}
		}

		/*if(newline) {
			get_pre_spaces(tmp, ctx, vars);
			//print_console(tmp, true);
		}*/

	#if MFW_OS == MFW_OS_LINUX
		attributes attr{vars.attr};
		if(severity_ == log_context::severity::warning || severity_ == log_context::severity::error) {
			attr |= attributes::bold;
			//attr |= attributes::underline;
		} else if(severity_ == log_context::severity::info) {
			attr |= attributes::italic;
		}
	
		pre_insert.clear();

		attr_to_str(attr, pre_insert);

		print_console(pre_insert);
	#endif
	
		if(vars.last) {
			fixedstr.insert(fixedstr.cend(), 1, u8'\n');
		}
		
		if(vars.txt_color != console_color::invalid) {
			set_txt_color(vars.txt_color);
		}

		print_console(fixedstr, true);
	}
	
	void logger_manager::print_console(ucstring_view str, log_context::severity severity_, bool file)
	{
		if(str.empty()) {
			return;
		}
		
		console_color txt_color{console_color::purple};
		switch(severity_) {
			case log_context::severity::warning: { txt_color = console_color::yellow; break; }
			case log_context::severity::error: { txt_color = console_color::red; break; }
			case log_context::severity::success: { txt_color = console_color::green; break; }
			case log_context::severity::info: { txt_color = console_color::aqua; break; }
			case log_context::severity::unknown: { txt_color = console_color::purple; break; }
		}
		set_txt_color(txt_color);
		
	#if MFW_OS == MFW_OS_LINUX
		print_console(u8"\x1b[21;22;23;24;25;27;28;29m"_sv);
	#endif
		print_console(str, file);
	}
	
	void logger_manager::print_console(ucstring_view str, bool file)
	{
		if(str.empty()) {
			return;
		}

		if(file && log_file) {
			log_file->write(str);
		}
		
	#if MFW_OS == MFW_OS_WINDOWS
		WriteConsoleW(stdout_handle, c_str(str), static_cast<uint32_t>(str.length()), nullptr, nullptr);
		#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		OutputDebugStringW(c_str(str));
		#endif
	#elif MFW_OS == MFW_OS_LINUX
		#if MFW_STD_FLAGGED(API_CONFORMING)
		::MFW_STD_NAMESPACE::printf("%s", c_str(str));
		#else
			#error
		#endif
	#else
		#error
	#endif
	}

	MFW_CORE_API void MFW_CORE_CALL log_context::resume()
	{
		if(!paused_ || history.empty()) {
			return;
		}

		paused_ = false;
		print_vars_t saved{print_vars_};
		for(history_vars_t &vars : history) {
			print_vars_ = vars;
			print(vars.print);
		}
		print_vars_ = saved;
		history.clear();
	}

	log_context::history_vars_t &log_context::history_vars_t::operator=(const print_vars_t &other)
	{
		name_ = other.name_;
		tag = other.tag;
		ident = other.ident;
		severity_ = other.severity_;
		return *this;
	}

	MFW_CORE_API void MFW_CORE_CALL log_context::print(const ucstring_view &str)
	{
		if(paused_) {
			history_vars_t &vars{history.emplace_back()};
			vars = print_vars_;
			vars.print = str;
			return;
		}

		logger_manager::instance().print(str, *this);
	}

	logger_manager::logger_manager()
	{
		::MFW_STD_NAMESPACE::setlocale(LC_ALL, "");
		::MFW_STD_NAMESPACE::locale::global(::MFW_STD_NAMESPACE::locale{""});

		::MFW_STD_NAMESPACE::setbuf(stdout, nullptr);

		log_file = interfaces::filesystem::instance().open_file({u8"console.log"_sv}, open_flags::all);

	#if MFW_OS == MFW_OS_WINDOWS
		if(!AttachConsole(ATTACH_PARENT_PROCESS)) {
			AllocConsole();
		}

		console_wnd = GetConsoleWindow();

		ShowWindow(console_wnd, SW_SHOW);

		close_menu = GetSystemMenu(console_wnd, FALSE);
		EnableMenuItem(close_menu, SC_CLOSE, MF_GRAYED);

		old_title.resize(128);
		uint32_t len{GetConsoleTitleW(reinterpret_cast<wchar_t *>(old_title.data()), static_cast<uint32_t>(old_title.length()))};
		old_title.resize(static_cast<size_t>(len));

		SetConsoleTitleW(L"Console");

		_wfreopen_s(&stdin_, L"CONIN$", L"r", stdin);
		_wfreopen_s(&stdout_, L"CONOUT$", L"w", stdout);
		_wfreopen_s(&stderr_, L"CONOUT$", L"w", stderr);

		stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);

		old_screenbuffer.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &old_screenbuffer);
		GetConsoleMode(stdout_handle, reinterpret_cast<unsigned long *>(&old_mode));

		old_bgclr = get_bg_color();
		old_txtclr = get_txt_color();

		uint32_t newmode{old_mode};
		newmode |= ENABLE_PROCESSED_OUTPUT;
		newmode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		newmode |= ENABLE_LVB_GRID_WORLDWIDE;

		SetConsoleMode(stdout_handle, newmode);
	#endif

		/*
		set_color_rgb(console_color::white, {byte{255}, byte{255}, byte{255}});
		set_color_rgb(console_color::red, {byte{255}, {}, {}});
		set_color_rgb(console_color::green, {{}, byte{255}, {}});
		set_color_rgb(console_color::blue, {{}, {}, byte{255}});
		set_color_rgb(console_color::aqua, {{}, byte{255}, byte{255}});
		set_color_rgb(console_color::yellow, {byte{255}, byte{255}, {}});
		*/
	}

	logger_manager::~logger_manager()
	{
		if(log_file) {
			delete log_file;
		}

	#if MFW_OS == MFW_OS_WINDOWS
		SetConsoleTitleW(c_str(old_title));

		set_bg_color(old_bgclr);
		set_txt_color(old_txtclr);

		SetConsoleMode(stdout_handle, old_mode);
		SetConsoleScreenBufferInfoEx(stdout_handle, &old_screenbuffer);

		EnableMenuItem(close_menu, SC_CLOSE, MF_ENABLED);

		fclose(stdin_);
		fclose(stdout_);
		fclose(stderr_);

		#if MFW_BUILD != MFW_BUILD_EXECUTABLE
		FreeConsole();
		#endif
	#elif MFW_OS == MFW_OS_LINUX
		print_console(u8"\x1b[0m"_sv);
	#endif
	}

	namespace __logging_internal
	{
		static size_t get_color_index(logger_manager::console_color color)
		{
			switch(color) {
				case logger_manager::console_color::black: { return 0; }
				case logger_manager::console_color::blue: { return 1; }
				case logger_manager::console_color::green: { return 2; }
				case logger_manager::console_color::aqua: { return 3; }
				case logger_manager::console_color::red: { return 4; }
				case logger_manager::console_color::purple: { return 5; }
				case logger_manager::console_color::yellow: { return 6; }
				case logger_manager::console_color::white: { return 7; }
				case logger_manager::console_color::gray: { return 8; }
				case logger_manager::console_color::light_blue: { return 9; }
				case logger_manager::console_color::light_green: { return 10; }
				case logger_manager::console_color::light_aqua: { return 11; }
				case logger_manager::console_color::light_red: { return 12; }
				case logger_manager::console_color::light_purple: { return 13; }
				case logger_manager::console_color::light_yellow: { return 14; }
				case logger_manager::console_color::bright_white: { return 15; }
				default: { return 0; }
			}
		}

	#if MFW_OS == MFW_OS_WINDOWS
		#define __MFW_GET_COLOR_FLAG(background, color) \
			static_cast<uint32_t>(background ? BACKGROUND_##color : FOREGROUND_##color)

		static uint32_t get_color_flags(logger_manager::console_color color, bool background)
		{
			switch(color) {
				case logger_manager::console_color::black: { return 0; }
				case logger_manager::console_color::blue: { return __MFW_GET_COLOR_FLAG(background, BLUE); }
				case logger_manager::console_color::green: { return __MFW_GET_COLOR_FLAG(background, GREEN); }
				case logger_manager::console_color::aqua: { return __MFW_GET_COLOR_FLAG(background, BLUE) | __MFW_GET_COLOR_FLAG(background, GREEN); }
				case logger_manager::console_color::red: { return __MFW_GET_COLOR_FLAG(background, RED); }
				case logger_manager::console_color::purple: { return __MFW_GET_COLOR_FLAG(background, BLUE) | __MFW_GET_COLOR_FLAG(background, RED); }
				case logger_manager::console_color::yellow: { return __MFW_GET_COLOR_FLAG(background, RED) | __MFW_GET_COLOR_FLAG(background, GREEN); }
				case logger_manager::console_color::white: { return __MFW_GET_COLOR_FLAG(background, RED) | __MFW_GET_COLOR_FLAG(background, GREEN) | __MFW_GET_COLOR_FLAG(background, BLUE); }
				case logger_manager::console_color::gray: { return __get_color_flags(logger_manager::console_color::black, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::light_blue: { return get_color_flags(logger_manager::console_color::blue, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::light_green: { return get_color_flags(logger_manager::console_color::green, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::light_aqua: { return get_color_flags(logger_manager::console_color::aqua, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::light_red: { return get_color_flags(logger_manager::console_color::red, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::light_purple: { return get_color_flags(logger_manager::console_color::purple, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::light_yellow: { return get_color_flags(logger_manager::console_color::yellow, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				case logger_manager::console_color::bright_white: { return get_color_flags(logger_manager::console_color::white, background) | __MFW_GET_COLOR_FLAG(background, INTENSITY); }
				default: { return 0; }
			}
		}
	#elif MFW_OS == MFW_OS_LINUX
		static uint32_t get_color_code(logger_manager::console_color color)
		{
			switch(color) {
				case logger_manager::console_color::black: { return 0; }
				case logger_manager::console_color::blue: { return 4; }
				case logger_manager::console_color::green: { return 2; }
				case logger_manager::console_color::aqua: { return 6; }
				case logger_manager::console_color::red: { return 1; }
				case logger_manager::console_color::purple: { return 5; }
				case logger_manager::console_color::yellow: { return 3; }
				case logger_manager::console_color::white: { return 255; }
				case logger_manager::console_color::gray: { return 7; }
				case logger_manager::console_color::light_blue: { return 12; }
				case logger_manager::console_color::light_green: { return 10; }
				case logger_manager::console_color::light_aqua: { return 14; }
				case logger_manager::console_color::light_red: { return 9; }
				case logger_manager::console_color::light_purple: { return 13; }
				case logger_manager::console_color::light_yellow: { return 11; }
				case logger_manager::console_color::bright_white: { return 15; }
				default: { return 0; }
			}
		}
	#endif
	}

	void logger_manager::set_bg_color(console_color color)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFOEX info{};
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &info);

		info.wAttributes &= ~(BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY);
		info.wPopupAttributes &= ~(BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE|BACKGROUND_INTENSITY);
	
		info.wAttributes |= __logging_internal::get_color_flags(color, true);
		info.wPopupAttributes |= __logging_internal::get_color_flags(color, true);

		SetConsoleTextAttribute(stdout_handle, info.wAttributes);
		//SetConsoleScreenBufferInfoEx(stdout_handle, &info);
	#elif MFW_OS == MFW_OS_LINUX
		screenbuffer.bg_color = color;
		const color_rgb &bg_rgb{screenbuffer.color_table[__logging_internal::get_color_index(screenbuffer.bg_color)]};
		ucstring str{};
		if(bg_rgb.valid()) {
			str = u8"\x1b[48;2;{};{};{}m"_fmt(bg_rgb.r, bg_rgb.g, bg_rgb.b);
		} else {
			str = u8"\x1b[48;5;{}m"_fmt(__logging_internal::get_color_code(color));
		}
		print_console(str);
	#else
		#error
	#endif
	}

	void logger_manager::set_txt_color(console_color color)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFOEX info{};
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &info);

		info.wAttributes &= ~(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
		info.wPopupAttributes &= ~(FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY);
	
		info.wAttributes |= __logging_internal::get_color_flags(color, false);
		info.wPopupAttributes |= __logging_internal::get_color_flags(color, false);

		SetConsoleTextAttribute(stdout_handle, info.wAttributes);
		//SetConsoleScreenBufferInfoEx(stdout_handle, &info);
	#elif MFW_OS == MFW_OS_LINUX
		screenbuffer.fg_color = color;
		const color_rgb &fg_rgb{screenbuffer.color_table[__logging_internal::get_color_index(screenbuffer.fg_color)]};
		ucstring str{};
		if(fg_rgb.valid()) {
			str = u8"\x1b[38;2;{};{};{}m"_fmt(fg_rgb.r, fg_rgb.g, fg_rgb.b);
		} else {
			str = u8"\x1b[38;5;{}m"_fmt(__logging_internal::get_color_code(color));
		}
		print_console(str);
	#else
		#error
	#endif
	}

	logger_manager::console_color logger_manager::get_txt_color() const
	{
	#if MFW_OS == MFW_OS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFOEX info{};
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &info);

		for(size_t i{0}; i < static_cast<size_t>(console_color::bright_white); i++) {
			if(bool_cast(info.wAttributes & __logging_internal::get_color_flags(static_cast<console_color>(i), false))) {
				return static_cast<console_color>(i);
			}
		}

		return console_color::invalid;
	#elif MFW_OS == MFW_OS_LINUX
		return screenbuffer.fg_color;
	#else
		#error
	#endif
	}

	logger_manager::console_color logger_manager::get_bg_color() const
	{
	#if MFW_OS == MFW_OS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFOEX info{};
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &info);

		for(size_t i{0}; i < static_cast<size_t>(console_color::bright_white); i++) {
			if(bool_cast(info.wAttributes & __logging_internal::get_color_flags(static_cast<console_color>(i), true))) {
				return static_cast<console_color>(i);
			}
		}

		return console_color::invalid;
	#elif MFW_OS == MFW_OS_LINUX
		return screenbuffer.bg_color;
	#else
		#error
	#endif
	}

	#if MFW_COMPILER == MFW_COMPILER_MSVC
		#pragma runtime_checks("", off)
	#endif
	logger_manager::color_rgb logger_manager::get_color_rgb(console_color conclr) const
	{
	#if MFW_OS == MFW_OS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFOEX info{};
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &info);

		const COLORREF &clrref{info.ColorTable[__logging_internal::get_color_index(conclr)]};

		color_rgb clr{};
		clr.r = GetRValue(clrref);
		clr.g = GetGValue(clrref);
		clr.b = GetBValue(clrref);

		return clr;
	#elif MFW_OS == MFW_OS_LINUX
		return screenbuffer.color_table[__logging_internal::get_color_index(conclr)];
	#else
		#error
	#endif
	}
	#if MFW_COMPILER == MFW_COMPILER_MSVC
		#pragma runtime_checks("", restore)
	#endif

	void logger_manager::set_color_rgb(console_color conclr, const color_rgb &clr)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		CONSOLE_SCREEN_BUFFER_INFOEX info{};
		info.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
		GetConsoleScreenBufferInfoEx(stdout_handle, &info);

		info.ColorTable[__logging_internal::get_color_index(conclr)] = RGB(clr.r, clr.g, clr.b);

		SetConsoleScreenBufferInfoEx(stdout_handle, &info);
	#elif MFW_OS == MFW_OS_LINUX
		screenbuffer.color_table[__logging_internal::get_color_index(conclr)] = clr;
	#else
		#error
	#endif
	}
}