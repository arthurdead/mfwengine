#include <private/mfw/renderer/graphics_card.hpp>
#include <public/mfw/stl/vector.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>

namespace mfw::renderer
{
	namespace __graphics_card_internal
	{
		static ptr_vector<graphics_card> gpus{};
	}

	bool graphics_card::initialize()
	{
		if(!interfaces::display_api_funcs::instance().collect_gpus(__graphics_card_internal::gpus)) {
			return false;
		}

		return true;
	}

	void graphics_card::shutdown()
	{
		__graphics_card_internal::gpus.clear();
	}
}