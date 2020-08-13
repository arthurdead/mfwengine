#include <public/mfw/stl/typeinfo.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
	[[nodiscard]] extern MFW_STL_API _MFW_ALLOC_PRE void * MFW_STL_CALL allocate(
		size_t __size,
		size_t __alignment, size_t __offset,
		bool __is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t __block
		#endif
		,const char *__file, int32_t __line,
		const type_info *__type
	#endif
	) noexcept _MFW_ALIGN_ALLOC_POST(1, 2);

	extern MFW_STL_API _MFW_ALLOC_PRE void * MFW_STL_CALL reallocate(
		void *&__ptr, size_t __size,
		size_t __alignment, size_t __offset,
		bool __is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t __block
		#endif
		,const char *__file, int32_t __line,
		const type_info *__type
	#endif
	) noexcept _MFW_ALIGN_REALLOC_POST(2, 3);

	extern MFW_STL_API void MFW_STL_CALL deallocate(
		void *&__ptr, size_t __size,
		size_t __alignment, size_t __offset,
		bool __is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t __block
		#endif
		,const char *__file, int32_t __line,
		const type_info *__type
	#endif
	) noexcept;

	extern MFW_STL_API size_t MFW_STL_CALL get_size(const void *__ptr,
	#if MFW_CONFIGURATION_IS(DEBUG) && MFW_OS_IS(WINDOWS)
		int32_t __block,
	#endif
		size_t __alignment, size_t __offset
	) noexcept;

	extern MFW_STL_API bool MFW_STL_CALL is_valid(const void *__ptr
	#if MFW_OS_IS(WINDOWS)
		,size_t __size
	#endif
	) noexcept(false);
	extern MFW_STL_API bool MFW_STL_CALL is_aligned(const void *__ptr) noexcept;

	template <typename _Dp, typename _Sp>
	constexpr inline _Dp &force_cast(const _Sp &src) noexcept;

	template <typename _Vp, typename _Tp>
	constexpr inline size_t var_offset(_Vp _Tp:: *var) noexcept;

	template <typename _Vp, typename _Tp>
	constexpr inline _Tp &get_outer(_Vp *ptr, _Vp _Tp:: *var) noexcept;

	template <typename _Vp, typename _Tp>
	constexpr inline _Tp &get_outer(_Vp *ptr, _Vp *_Tp:: *var) noexcept;

	MFW_VISIBILITY_LOCAL_PUSH()

	template <typename _Tp>
	bool is_valid(_Tp *__ptr) noexcept(false);

	template <typename _Tp, typename... _Args>
	[[nodiscard]] _MFW_ALLOC_PRE _Tp *__create(
#if MFW_CONFIGURATION_IS(DEBUG)
	const char *__file, size_t __line,
#endif
	_Args... __args
	) noexcept MFW_ATTRIBUTE(__malloc__);

	template <typename _Tp>
	void __destroy(_Tp *&__ptr
#if MFW_CONFIGURATION_IS(DEBUG)
	,const char *__file, size_t __line
#endif
	) noexcept;

	template <typename _Tp, typename _Sp>
	void to_string(_Tp *src, _Sp &dst) noexcept;

	MFW_VISIBILITY_LOCAL_POP()
}

#include <public/mfw/stl/impl/memory_funcs.tpp>