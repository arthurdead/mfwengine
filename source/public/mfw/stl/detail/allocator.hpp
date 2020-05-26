#if MFW_STD_FLAGGED(HEADERS_CONFORMING)
	#pragma push_macro("new")
	#undef new
	#include <memory>
	#pragma pop_macro("new")
#else
	#error
#endif

namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	using ::MFW_STD_NAMESPACE::allocator_traits;

	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	template <typename T>
	class allocator
	{
	public:
		using value_type = T;

		[[nodiscard]] static __MFW_ALLOC_PRE value_type *allocate(size_t size) noexcept __MFW_ALLOC_POST(1);

		static void deallocate(value_type *&ptr, size_t size) noexcept;
		static void deallocate(value_type *const &ptr, size_t size) noexcept;

		constexpr allocator() noexcept = default;

		template <typename U>
		constexpr allocator(const allocator<U> &) noexcept {}

		template <typename U>
		constexpr bool operator==(const allocator<U> &) const noexcept { return true; }

		template <typename U>
		constexpr bool operator!=(const allocator<U> &) const noexcept { return true; }

	private:
		static void __impl_deallocate(value_type *&ptr, size_t size) noexcept;
	};
	#else
	using ::MFW_STD_NAMESPACE::allocator;
	#endif
#else
	#error
#endif
}

#include <public/mfw/stl/impl/allocator.ipp>