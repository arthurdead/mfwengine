#include <public/mfw/stl/utility.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/new.hpp>

namespace mfw::stl
{
	template <typename T>
	bool is_valid(T *ptr) noexcept(false)
	{
	#if MFW_OS_IS(WINDOWS)
		return is_valid(reinterpret_cast<const void *>(ptr), sizeof(T));
	#else
		return is_valid(reinterpret_cast<const void *>(ptr));
	#endif
	}

	template <typename T, typename ...Args>
	[[nodiscard]] __MFW_ALLOC_PRE T *__create(
#if MFW_CONFIGURATION_IS(DEBUG)
	const char *file, size_t line,
#endif
	Args &&... args
	) noexcept //MFW_ATTRIBUTE(__malloc__)
	{
		#pragma push_macro("new")
		#undef new

		return new(sizeof(T),
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		__MFW_MEM_BLOCK,
		#endif
		file, line
	#else
		static_cast<align_val_t>(alignof(T)), nothrow
	#endif
		) T(forward<Args>(args)...);

		#pragma pop_macro("new")
	}

#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_WARNING_PUSH()
	MFW_WARNING_DISABLE_UNIX("-Wunused-parameter")
#endif

	template <typename T>
	void __destroy(T *&ptr
#if MFW_CONFIGURATION_IS(DEBUG)
	,const char *file, size_t line
#endif
	) noexcept {
	#if 1
		ptr->~T();
		::operator delete(ptr,
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
		__MFW_MEM_BLOCK,
			#endif
		file, line
		#else
		sizeof(T), static_cast<align_val_t>(alignof(T)), nothrow
		#endif
		);
	#else
		delete ptr;
	#endif
	}

#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_WARNING_POP()
#endif

	MFW_WARNING_PUSH()
#if MFW_COMPILER_FLAGGED(MSVC)
	MFW_WARNING_DISABLE(4172) //returning address of local variable or temporary
	MFW_WARNING_DISABLE(4582) //'type': constructor is not implicitly called
	MFW_WARNING_DISABLE(4583) //'type': destructor is not implicitly called
	MFW_WARNING_DISABLE(4800) //Implicit conversion from 'type' to bool. Possible information loss
	MFW_WARNING_DISABLE(4200) //nonstandard extension used : zero-sized array in struct/union
	MFW_WARNING_DISABLE(4815) //'u': zero-sized array in stack object will have no elements (unless the object is an aggregate that has been aggregate initialized)
	MFW_WARNING_DISABLE(4946) //reinterpret_cast used between related classes: 'type' and 'type'
#endif
#if MFW_COMPILER_FLAGGED(CLANG)
	MFW_WARNING_DISABLE_UNIX("-Wreturn-stack-address")
	MFW_WARNING_DISABLE_UNIX("-Wold-style-cast")
	MFW_WARNING_DISABLE_UNIX("-Wcast-qual")
	MFW_WARNING_DISABLE_UNIX("-Wmicrosoft-flexible-array")
	MFW_WARNING_DISABLE_UNIX("-Wc99-extensions")
#elif MFW_COMPILER_IS(GCC)
	MFW_WARNING_DISABLE("-Wstrict-aliasing")
#endif
	template <typename D, typename S>
	constexpr D &force_cast(const S &src_) noexcept
	{
		using __D = remove_cvref_t<D>;
		using __S = remove_cvref_t<S>;
		MFW_IF_CONSTEXPR(is_convertible_v<__S, __D>) {
			return reinterpret_cast<D &>(const_cast<S &>(src_));
		} else {
			const union U final {
			public:
				constexpr inline U(const U &) = delete;
				inline U(const S &_src) : src{_src} {}
				inline ~U() {}
				constexpr inline operator D &() const { return type_cast(D &, dst); }
			private:
				const __S src;
				const __D *dst;
			} u{src_};
			return static_cast<D &>(u);
		}
	}
	MFW_WARNING_POP()

	template <typename V, typename T>
	constexpr size_t var_offset(V T:: *var) noexcept
	{ return (reinterpret_cast<uintptr_t>(&reinterpret_cast<unsigned char &>((reinterpret_cast<T *>(nullptr))->*var))); }

	template <typename V, typename T>
	constexpr T &get_outer(V *ptr, V T:: *var) noexcept
	{ return *reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(ptr) - var_offset(var)); }

	template <typename V, typename T>
	constexpr T &get_outer(V *ptr, V *T:: *var) noexcept
	{ return *reinterpret_cast<T *>(reinterpret_cast<unsigned char *>(ptr) - var_offset(var)); }

	template <typename T>
	inline void to_string(T *src, ucstring &dst)
	{
		dst += as_string<ucstring>(*src);
	}
}