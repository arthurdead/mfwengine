#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/stdint.hpp>
#include <public/mfw/stl/typeinfo.hpp>

#include <public/mfw/core/internal/typeinfo.hpp>

namespace mfw::core
{
	bool TypeInfo::isInt8() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			isRelaxed<__int8>() ||
		#endif
			isRelaxed<char>() ||
			isExact<bool>()
		#ifdef MFW_CPP_CHAR8_SUPPORTED
			|| isExact<char8_t>()
		#endif
		);
	}

	bool TypeInfo::isInt16() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			isRelaxed<__int16>() ||
		#endif
			isRelaxed<short>() ||
			isExact<char16_t>()
		#if MFW_WCHAR_SIZE == 16
			|| isExact<wchar_t>()
		#endif
		);
	}

	bool TypeInfo::isInt32() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			isRelaxed<__int32>() ||
		#endif
			isRelaxed<int>() ||
		#if MFW_LONG_SIZE == 32
			isExact<long>() ||
		#endif
		#if MFW_WCHAR_SIZE == 32
			isExact<wchar_t>() ||
		#endif
			isExact<char32_t>()
		);
	}

	bool TypeInfo::isInt64() const noexcept
	{
		return (
		#if MFW_COMPILER_FLAGGED(MSVC)
			isRelaxed<__int64>() ||
		#endif
		#if MFW_LONG_SIZE == 64
			isExact<long>() ||
		#endif
			isExact<long long>()
		);
	}

	bool TypeInfo::isFloat16() const noexcept
	{
	#ifdef MFW_FLOAT16_SUPPORTED
		return isExact<_Float16>();
	#else
		return isFloat32();
	#endif
	}

	bool TypeInfo::isFloat32() const noexcept
	{ return isExact<float>(); }

	bool TypeInfo::isFloat64() const noexcept
	{ return isExact<double>(); }

	bool TypeInfo::isFloat80() const noexcept
	{
		return (
		#if MFW_COMPILER_IS(GCC)
			isExact<__float80>() ||
		#endif
			isExact<long double>()
		);
	}

	bool TypeInfo::isFloat128() const noexcept
	{
	#ifdef MFW_FLOAT128_SUPPORTED
		return isExact<_Float128>();
	#else
		return isFloat80();
	#endif
	}
}