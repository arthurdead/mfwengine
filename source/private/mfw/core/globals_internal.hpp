#ifndef __MFW_PRIVATE_CORE_GLOBALS_INTERNAL_H
#define __MFW_PRIVATE_CORE_GLOBALS_INTERNAL_H

#pragma once

#include <public/mfw/core/application.hpp>

namespace mfw::core
{
	extern exit_status initialize_all_globals();
	extern exit_status update_all_globals();
	extern exit_status shutdown_all_globals();

	extern void allocate_all_globals();
	extern void deallocate_all_globals();
}

#endif