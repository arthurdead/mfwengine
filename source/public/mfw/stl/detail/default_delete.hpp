namespace mfw::stl
{
#if MFW_STD_FLAGGED(API_CONFORMING)
	#ifdef __MFW_ENABLE_CUSTOM_ALLOCATORS
	template <typename T>
	class default_delete final
	{
	public:
		void operator()(T *&ptr) const noexcept;
		void operator()(T *const &ptr) const noexcept;
	};

	template <typename T>
	class default_delete<T[]> final
	{
	public:
		void operator()(T *&ptr) const noexcept;
		void operator()(T *const &ptr) const noexcept;

		template <size_t S>
		void operator()(T (&array)[S]) const noexcept;
	};
	#else
	using ::MFW_STD_NAMESPACE::default_delete;
	#endif
#else
	#error
#endif
}

#include <public/mfw/stl/impl/default_delete.ipp>