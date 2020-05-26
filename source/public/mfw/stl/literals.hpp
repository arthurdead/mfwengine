#ifndef __MFW_PUBLIC_STL_LITERALS_H
#define __MFW_PUBLIC_STL_LITERALS_H

#pragma once

#include <public/mfw/stl/format.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/string_view.hpp>

namespace mfw::stl::literals {}

namespace mfw::literals
{
	using namespace stl::literals;
}

#endif