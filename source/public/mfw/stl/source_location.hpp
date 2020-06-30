#ifndef _MFW_PUBLIC_STL_SOURCE_LOCATION_H
#define _MFW_PUBLIC_STL_SOURCE_LOCATION_H

#pragma once

#include <public/mfw/stl/version.hpp>

#if MFW_HAS_INCLUDE(<source_location>)
	#define MFW_CPP_SOURCE_LOCATION_SUPPORTED 1
#elif MFW_HAS_INCLUDE(<experimental/source_location>) || \
		defined __cpp_lib_experimental_source_location
	#define MFW_CPP_SOURCE_LOCATION_SUPPORTED 2
#endif

#if MFW_CPP_IS_EXPERIMENTAL(SOURCE_LOCATION)
	#include <experimental/source_location>
#elif MFW_CPP_IS_SUPPORTED(SOURCE_LOCATION)
	#include <source_location>
#else
	#include <public/mfw/stl/string_view.hpp>
#endif

namespace mfw::stl
{
#if MFW_CPP_IS_EXPERIMENTAL(SOURCE_LOCATION)
	using ::MFW_STD_NAMESPACE::experimental::source_location;
#elif MFW_CPP_IS_SUPPORTED(SOURCE_LOCATION)
	using ::MFW_STD_NAMESPACE::source_location;
#else
	struct source_location final
	{
		constexpr source_location() noexcept
			: _M_file{}, _M_func{}, _M_line{0}, _M_col{0}
		{}

		static constexpr source_location current(
			string_view __file = __builtin_FILE(),
			string_view __func = __builtin_FUNCTION(),
			size_t __line = __builtin_LINE(),
			size_t __col = 0) noexcept
		{
			source_location __loc{};
			__loc._M_file = __file;
			__loc._M_func = __func;
			__loc._M_line = __line;
			__loc._M_col = __col;
			return __loc;
		}

		constexpr size_t line() const noexcept { return _M_line; }
		constexpr size_t column() const noexcept { return _M_col; }
		constexpr string_view file_name() const noexcept { return _M_file; }
		constexpr string_view function_name() const noexcept { return _M_func; }

	private:
		string_view _M_file{};
		string_view _M_func{};
		size_t _M_line{0};
		size_t _M_col{0};
	};
#endif
}

#endif