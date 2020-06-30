namespace mfw::stl
{
#ifdef _MFW_ENABLE_CUSTOM_ALLOCATORS
	template <typename _Tp>
	class default_delete final
	{
	public:
		void operator()(T *&__ptr) const noexcept;
		void operator()(T *const &__ptr) const noexcept;
	};

	template <typename _Tp>
	class default_delete<_Tp[]> final
	{
	public:
		void operator()(_Tp *&__ptr) const noexcept;
		void operator()(_Tp *const &__ptr) const noexcept;

		template <size_t _Sp>
		void operator()(_Tp (&__array)[S]) const noexcept;
	};
#else
	using ::MFW_STD_NAMESPACE::default_delete;
#endif
}

#include <public/mfw/stl/impl/default_delete.ipp>