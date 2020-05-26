#include <public/mfw/stl/typeinfo.hpp>
#include <public/mfw/stl/string.hpp>

namespace mfw::stl
{
	[[nodiscard]] MFW_STL_API __MFW_ALLOC_PRE void * MFW_STL_CALL allocate(
		size_t size,
		size_t alignment, size_t offset,
		bool is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line,
		const type_info *type
	#endif
	) noexcept __MFW_ALIGN_ALLOC_POST(1, 2);

	MFW_STL_API __MFW_ALLOC_PRE void * MFW_STL_CALL reallocate(
		void *&ptr, size_t size,
		size_t alignment, size_t offset,
		bool is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line,
		const type_info *type
	#endif
	) noexcept __MFW_ALIGN_REALLOC_POST(2, 3);

	MFW_STL_API void MFW_STL_CALL deallocate(
		void *&ptr, size_t size,
		size_t alignment, size_t offset,
		bool is_array
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		,int32_t block
		#endif
		,const char *file, int32_t line,
		const type_info *type
	#endif
	) noexcept;

	MFW_STL_API void MFW_STL_CALL copy(void *dst, [[maybe_unused]] size_t max, const void *src, size_t size);

	MFW_STL_API size_t MFW_STL_CALL get_size(const void *ptr,
	#if MFW_CONFIGURATION_IS(DEBUG) && MFW_OS_IS(WINDOWS)
		int32_t block,
	#endif
		size_t alignment, size_t offset
	) noexcept;

	MFW_STL_API bool MFW_STL_CALL is_valid(const void *ptr
	#if MFW_OS_IS(WINDOWS)
		,size_t size
	#endif
	) noexcept(false);
	MFW_STL_API bool MFW_STL_CALL is_aligned(const void *ptr) noexcept;

	template <typename T>
	bool is_valid(T *ptr) noexcept(false);

	template <typename T, typename ...Args>
	[[nodiscard]] __MFW_ALLOC_PRE T *__create(
#if MFW_CONFIGURATION_IS(DEBUG)
	const char *file, size_t line,
#endif
	Args... args
	) noexcept MFW_ATTRIBUTE(__malloc__);

	template <typename T>
	void __destroy(T *&ptr
#if MFW_CONFIGURATION_IS(DEBUG)
	,const char *file, size_t line
#endif
	) noexcept;

	template <typename D, typename S>
	constexpr D &force_cast(const S &src) noexcept;

	template <typename V, typename T>
	constexpr size_t var_offset(V T:: *var) noexcept;

	template <typename V, typename T>
	constexpr T &get_outer(V *ptr, V T:: *var) noexcept;

	template <typename V, typename T>
	constexpr T &get_outer(V *ptr, V *T:: *var) noexcept;

	template <typename T>
	void to_string(T *src, ucstring &dst);
}

#include <public/mfw/stl/impl/memory_funcs.ipp>