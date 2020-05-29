#ifndef __MFW_PUBLIC_PCH_LITERALS_H
#define __MFW_PUBLIC_PCH_LITERALS_H

#pragma once

#ifdef MFW_CPP
	#ifdef MFW_DEPENDS_ON_STL
		#include <public/mfw/stl/literals.hpp>
	#endif

	#ifdef MFW_DEPENDS_ON_CORE
		#include <public/mfw/core/literals.hpp>
	#endif

namespace mfw::literals {}

	#if MFW_COMPILER_FLAGGED(CLANG)
		MFW_WARNING_PUSH()
		MFW_WARNING_DISABLE_UNIX("-Wheader-hygiene")
	#endif
using namespace mfw::literals;
	#if MFW_COMPILER_FLAGGED(CLANG)
		MFW_WARNING_POP()
	#endif
#endif

#endif