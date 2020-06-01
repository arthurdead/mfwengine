#ifndef __MFW_PUBLIC_RENDERER_RENDERER_INTERFACE_H
#define __MFW_PUBLIC_RENDERER_RENDERER_INTERFACE_H

#pragma once

#include <public/mfw/renderer/renderer.hpp>

namespace mfw::renderer::interfaces
{
	class renderer
	{
	protected:
		virtual ~renderer() = default;

	public:
		MFW_RENDERER_API static renderer & MFW_RENDERER_CALL instance();

		virtual void do_stuff() = 0;
	};
}

#endif