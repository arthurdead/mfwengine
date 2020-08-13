#ifndef MFW_PUBLIC_STL_WINDOWS_WINDOWS_HPP
#define MFW_PUBLIC_STL_WINDOWS_WINDOWS_HPP

#pragma once

#ifdef MFW_PUBLIC_STL_VERSION_HPP
	#error please include windows before version
#endif

#include <public/mfw/stl/internal/version_config.hpp>
#include <public/mfw/stl/internal/version_os.hpp>
#include <public/mfw/stl/internal/pre_std.hpp>

#define NOMINMAX
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
#define NOWINMESSAGES
#define NOWINSTYLES
#define NOSYSMETRICS
#define NOICONS
#define NOKEYSTATES
#define NORASTEROPS
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOGDI
#define NOKERNEL
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOMENUS
#define NOSHOWWINDOW
#define NOUSER
#define NOSYSCOMMANDS
#define NORESOURCE
#define NODESKTOP
#define NOWINDOWSTATION
#define NOWINRES
#define NOCRYPT
#define NOIME
#define NOTOOLBAR
#define NOUPDOWN
#define NOSTATUSBAR
#define NOMENUHELP
#define NOTRACKBAR
#define NODRAGLIST
#define NOPROGRESS
#define NOHOTKEY
#define NOHEADER
#define NOIMAGEAPIS
#define NOLISTVIEW
#define NOTREEVIEW
#define NOTABCONTROL
#define NOANIMATE
#define NOBUTTON
#define NOSTATIC
#define NOEDIT
#define NOLISTBOX
#define NOCOMBOBOX
#define NOSCROLLBAR
#define NOTASKDIALOG
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#define ENABLE_RTL_NUMBER_OF_V2
#define WINDOWS_ENABLE_CPLUSPLUS
#define ENABLE_RESTRICTED
#define _STDCALL_SUPPORTED
#define DEPRECATE_DDK_FUNCTIONS
#define RPC_NO_WINDOWS_H
#define DOSWIN32
#define _WIN32NLS
#define _WIN32REG
#define INC_OLE2
#define STRICT

#if MFW_CHARACTERSET_IS(UNICODE)
	#define DBGHELP_TRANSLATE_TCHAR
	#define RPC_USE_NATIVE_WCHAR
#endif

#if MFW_CONFIGURATION_IS(DEBUG)
	#define STRICT_GS_ENABLED
	#define COM_STDMETHOD_CAN_THROW
#endif

#include <windows.h>

#define _MAX_PATH MFW_MACRO_EXPAND(MAX_PATH)
#undef MAX_PATH
#define MAX_PATH please avoid this

#endif