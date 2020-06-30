#include <public/mfw/stl/stl.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(float32_t __src, u32string &__dst) noexcept;

	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(float64_t __src, u32string &__dst) noexcept;

	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, wstring &__dst) noexcept;
#ifdef MFW_CPP_CHAR8_SUPPORTED
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, u8string &__dst) noexcept;
#endif
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, u16string &__dst) noexcept;
	MFW_STL_API void MFW_STL_CALL to_string(float80_t __src, u32string &__dst) noexcept;
}