#ifndef __MFW_PUBLIC_ENGINE_ENGINE_INTERFACE_H
#define __MFW_PUBLIC_ENGINE_ENGINE_INTERFACE_H

#pragma once

#include <public/mfw/core/application.hpp>
#include <public/mfw/engine/engine.hpp>

namespace mfw::engine
{
	namespace interfaces
	{
		class engine
		{
		protected:
			virtual ~engine() = default;

		public:
			MFW_ENGINE_API static engine & MFW_ENGINE_CALL instance();
		};
	}
}

#endif