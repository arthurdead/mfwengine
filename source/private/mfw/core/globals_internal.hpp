#ifndef MFW_PRIVATE_CORE_GLOBALS_INTERNAL_HPP
#define MFW_PRIVATE_CORE_GLOBALS_INTERNAL_HPP

#pragma once

#include <public/mfw/core/application.hpp>

namespace mfw::core
{
	MFW_VISIBILITY_LOCAL void sortInitializers() noexcept;
	MFW_VISIBILITY_LOCAL ExitStatus initializeAllGlobals() noexcept;
	MFW_VISIBILITY_LOCAL ExitStatus updateAllGlobals() noexcept;
	MFW_VISIBILITY_LOCAL ExitStatus shutdownAllGlobals() noexcept;

	MFW_VISIBILITY_LOCAL void allocateAllGlobals() noexcept;
	MFW_VISIBILITY_LOCAL void deallocateAllGlobals() noexcept;
}

#endif