#pragma push_macro("new")
#undef new
#if MFW_STDCPP_IS(DEFAULT)
	#include <memory>
#elif MFW_STDCPP_IS(EA)
	#include <EASTL/memory.h>
#else
	#error
#endif
#pragma pop_macro("new")

namespace mfw::stl
{
	using ::MFW_STD_NAMESPACE::allocator_traits;

#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	template <typename _Tp>
	class allocator
	{
	public:
		using value_type = _Tp;

		[[nodiscard]] static _MFW_ALLOC_PRE value_type *allocate(size_t __size) noexcept _MFW_ALLOC_POST(1);

		static void deallocate(value_type *&__ptr, size_t __size) noexcept;
		static void deallocate(value_type *const &__ptr, size_t __size) noexcept;

		constexpr allocator() noexcept = default;

		template <typename _Up>
		constexpr allocator(const allocator<_Up> &) noexcept
			{}

		template <typename _Up>
		constexpr bool operator==(const allocator<_Up> &) const noexcept
			{ return true; }

		template <typename _Up>
		constexpr bool operator!=(const allocator<_Up> &) const noexcept
			{ return true; }

	private:
		static MFW_VISIBILITY_LOCAL void _deallocate_impl(value_type *&__ptr, size_t __size) noexcept;
	};
#else
	using ::MFW_STD_NAMESPACE::allocator;
#endif
}

#include <public/mfw/stl/impl/allocator.ipp>