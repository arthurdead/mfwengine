#ifndef __MFW_PRIVATE_RENDERER_X11_X11_DISPLAY_API_FUNCS_HPP
#define __MFW_PRIVATE_RENDERER_X11_X11_DISPLAY_API_FUNCS_HPP

#pragma once

#include <private/mfw/renderer/display_api_funcs.hpp>

namespace mfw::renderer
{
	class display_api_funcs_x11 : public interfaces::display_api_funcs
	{
	public:
		bool collect_gpus(ptr_vector<graphics_card> &gpus) override;
	};
}

#endif