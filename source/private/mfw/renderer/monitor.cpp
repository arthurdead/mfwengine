#include <private/mfw/renderer/monitor.hpp>
#include <public/mfw/stl/vector.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>

namespace mfw::renderer
{
	namespace __monitor_internal
	{
		static ptr_vector<monitor> monitors{};
	}

	bool monitor::initialize()
	{
		if(!interfaces::display_api_funcs::instance().collect_monitors(__monitor_internal::monitors)) {
			return false;
		}

		return true;
	}
}