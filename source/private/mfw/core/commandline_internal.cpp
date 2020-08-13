#include <private/mfw/core/commandline_internal.hpp>
#include <public/mfw/core/globals.hpp>
#include <public/mfw/stl/version.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <private/mfw/core/file.hpp>
	#include <public/mfw/stl/format.hpp>
	#include <public/mfw/pch_literals.hpp>
	#include <unistd.h>
	#include <sys/stat.h>
#endif
#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
	#include <public/mfw/core/logging_interface.hpp>
	#include <public/mfw/pch_literals.hpp>
#endif

namespace mfw::core
{
	MFW_DECLARE_GLOBAL_ALLOCATOR(commandline, commandline_internal)
#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
	MFW_DECLARE_LOG_CONTEXT(log_commandlineinternal, u8"core/commandline_internal"_p)
#endif

	commandline_internal &commandline_internal::instance() {
		return __commandline_global_allocator.instance();
	}

	namespace __commandline_internal_internal
	{
	#if MFW_OS == MFW_OS_LINUX
		static void fix_commas(ucstring &dst, size_t &off)
		{
			off++;
			if(dst[off] != u8'"') {
				dst.insert(off, 1, u8'"');
			}

			size_t off4{dst.find(u8',', off)};
			if(off4 == ucstring::npos) {
				off4 = dst.find(u8'\0', off);
				if(off4 == ucstring::npos) {
					if(*(dst.cend()-1) != u8'"') {
						dst += u8'"';
					}
				}
			} else {
				if(dst[off4] != u8'"') {
					dst.insert(off4, 1, u8'"');
				}
				off4++;
				fix_commas(dst, off4);
				off = off4;
			}
		}

		static bool fix_quote(ucstring &dst, size_t &off, char8_t sep)
		{
			size_t off3{dst.find(sep, off)};
			bool found{off3 != ucstring::npos};
			if(found) {
				MFW_MESSAGE("big think")
				if(dst[off3-1] != u8'"') {
					dst.insert(off3, 1, u8'"');
				}
				off = off3;
				off++;
				if(sep == u8',') {
					fix_commas(dst, off);
				}
			}
			return found;
		}
		
		static bool has_sep(ucstring &dst, size_t off)
		{
			while(true) {
				if(dst[off] == u8'\0') {
					return false;
				}
				
				if(dst[off] == u8'+' ||
					dst[off] == u8'-' ||
					dst[off] == u8'/') {
					return true;
				}
				
				off--;
			}
		}

		static void fix_quotes(ucstring &dst, char8_t sep)
		{
			size_t off{0};
			while(true) {
				off = dst.find(sep, off);
				if(off == ucstring::npos) {
					break;
				}

				if(dst[off+1] != u8'-' &&
					//dst[off+1] != u8'/' &&
					dst[off+1] != u8'+' &&
					dst[off+1] != u8'\0') {
					off++;
					if(dst[off] != u8'"') {
						dst.insert(off, 1, u8'"');
						if(sep != '\0') {
							if(!has_sep(dst, off)) {
								off--;
								dst.insert(off, 1, u8'"');
							}
						}
					}
					if(!fix_quote(dst, off, u8'\0') &&
						!fix_quote(dst, off, u8',')) {
						if(*(dst.cend()-1) != u8'"') {
							dst += u8'"';
						}
					}
				}

				off++;
			}
		}
		
		static bool has_value(ucstring &dst, size_t off)
		{
			while(true) {
				if(dst[off] == u8'\0') {
					return false;
				}
				
				if(dst[off] == u8'=' ||
					dst[off] == u8':') {
					return true;
				}
				
				off++;
			}
		}
		
		static void fix_quotes(ucstring &dst)
		{
			dst.insert(0, 1, u8'\0');
			
			size_t off{dst.find(u8'\0', 1)};
			if(off != ucstring::npos) {
				if(off < (dst.length()-1) &&
					dst[off+1] != u8'-' &&
					dst[off+1] != u8'+' &&
					dst[off+1] != u8'/') {
					if(has_value(dst, off+1)) {
						dst.insert(off+1, 1, u8'"');
					}
				}
			}
		}
	#endif

		static bool get_commandline_string(ucstring &dst)
		{
		#if MFW_OS == MFW_OS_WINDOWS
			const wchar_t *cmdstr{GetCommandLineW()};
			to_string(cmdstr, dst);
			size_t off{dst.find(L'"', 0)};
			off = dst.find(L'"', off + 1);
			dst.erase(0, off + 2);
		#elif MFW_OS == MFW_OS_LINUX
			FILE *cmdline{fopen("/proc/self/cmdline", "r")};
			if(!cmdline) {
				return false;
			}
			size_t len{core::file::get_handle_size(cmdline)};
			dst.resize(len);
			fread(c_str(dst), sizeof(ucchar_t), len, cmdline);
			fclose(cmdline);
			size_t off{dst.find(u8'\0', 0)};
			if(off == ucstring::npos || off == len-1) {
				dst.clear();
				return true;
			}
			dst.erase(0, off + 1);

			fix_quotes(dst);
			fix_quotes(dst, u8':');
			fix_quotes(dst, u8'=');
			fix_quotes(dst, u8'\0');

			replace_all(dst, u8'\0', u8' ');
		#else
			#error
		#endif
			return true;
		}
	}

	exit_status commandline_internal::initialize()
	{
		ucstring cmdline{};
		__commandline_internal_internal::get_commandline_string(cmdline);

	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		if(!cmdline.empty()) {
			log_commandlineinternal().info(u8"string:"_sv);
			log_commandlineinternal().add_ident();
			log_commandlineinternal().info(u8"{}"_sv, cmdline);
			log_commandlineinternal().remove_ident();
		}
	#endif

		if(!cmdline.empty() && !parse(cmdline)) {
			return exit_status::fatal;
		}

	#if MFW_CONFIGURATION == MFW_CONFIGURATION_DEBUG
		if(!empty()) {
			log_commandlineinternal().info(u8"map:"_sv);
			log_commandlineinternal().add_ident();
			const_iterator it{cbegin()};
			while(it != cend()) {
				const arg_map_t::value_type &arg{*it};

				if(!arg.second.empty()) {
					log_commandlineinternal().info(u8"{} = {}"_sv, arg.first, arg.second);
				} else {
					log_commandlineinternal().info(u8"{}"_sv, arg.first);
				}

				it++;
			}
			log_commandlineinternal().remove_ident();
		}
	#endif

		return exit_status::success;
	}
}