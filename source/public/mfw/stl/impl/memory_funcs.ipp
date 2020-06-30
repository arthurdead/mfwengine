#include <public/mfw/stl/utility.hpp>
#include <public/mfw/stl/type_traits.hpp>
#include <public/mfw/stl/new.hpp>

namespace mfw::stl
{
	template <typename _Tp>
	bool is_valid(_Tp *__ptr) noexcept(false)
	{
	#if MFW_OS_IS(WINDOWS)
		return is_valid(reinterpret_cast<const void *>(__ptr), sizeof(_Tp));
	#else
		return is_valid(reinterpret_cast<const void *>(__ptr));
	#endif
	}

	template <typename _Tp, typename... _Args>
	[[nodiscard]] MFW_VISIBILITY_LOCAL _MFW_ALLOC_PRE _Tp *__create(
#if MFW_CONFIGURATION_IS(DEBUG)
	const char *__file, size_t __line,
#endif
	_Args &&... __args
	) noexcept //MFW_ATTRIBUTE(__malloc__)
	{
		#pragma push_macro("new")
		#undef new

		return new(sizeof(_Tp),
	#if MFW_CONFIGURATION_IS(DEBUG)
		#if MFW_OS_IS(WINDOWS)
		_MFW_MEM_BLOCK,
		#endif
		__file, __line
	#else
		static_cast<align_val_t>(alignof(_Tp)), nothrow
	#endif
		) _Tp(forward<_Args>(__args)...);

		#pragma pop_macro("new")
	}

#if MFW_COMPILER_FLAGGED(UNIX)
	MFW_WARNING_PUSH()
	MFW_WARNING_DISABLE_UNIX("-Wunused-parameter")
#endif

	template <typename _Tp>
	MFW_VISIBILITY_LOCAL void __destroy(_Tp *&__ptr
#if MFW_CONFIGURATION_IS(DEBUG)
	,const char *__file, size_t __line
#endif
	) noexcept {
	#if 1
		__ptr->~_Tp();
		::operator delete(__ptr,
		#if MFW_CONFIGURATION_IS(DEBUG)
			#if MFW_OS_IS(WINDOWS)
		_MFW_MEM_BLOCK,
			#endif
		__file, __line
		#else
		sizeof(_Tp), static_cast<align_val_t>(alignof(_Tp)), nothrow
		#endif
		);
	#else
		delete __ptr;
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
	template <typename _Dp, typename _Sp>
	constexpr inline _Dp &force_cast(const _Sp &__src) noexcept
	{
		using __D = remove_cvref_t<_Dp>;
		using __S = remove_cvref_t<_Sp>;
		MFW_IF_CONSTEXPR(is_convertible_v<__S, __D>) {
			return reinterpret_cast<_Dp &>(const_cast<_Sp &>(__src));
		} else {
			const union __U final {
			public:
				constexpr inline __U(const __U &) = delete;
				constexpr inline __U(const _Sp &__src)
					: _M_src{__src} {}
				constexpr inline ~__U() {}
				constexpr inline operator _Dp &() const
				{ return type_cast(_Dp &, _M_dst); }
			private:
				const __S _M_src;
				const __D *_M_dst;
			} __u{__src};
			return static_cast<_Dp &>(__u);
		}
	}
	MFW_WARNING_POP()

	template <typename _Vp, typename _Tp>
	constexpr inline size_t var_offset(_Vp _Tp:: *__var) noexcept
	{ return (reinterpret_cast<uintptr_t>(&reinterpret_cast<unsigned char &>((reinterpret_cast<_Tp *>(nullptr))->*__var))); }

	template <typename _Vp, typename _Tp>
	constexpr inline _Tp &get_outer(_Vp *__ptr, _Vp _Tp:: *__var) noexcept
	{ return *reinterpret_cast<_Tp *>(reinterpret_cast<unsigned char *>(__ptr) - var_offset(__var)); }

	template <typename _Vp, typename _Tp>
	constexpr inline _Tp &get_outer(_Vp *__ptr, _Vp *_Tp:: *__var) noexcept
	{ return *reinterpret_cast<_Tp *>(reinterpret_cast<unsigned char *>(__ptr) - var_offset(__var)); }

	template <typename _Tp, typename _Sp>
	void to_string(_Tp *__src, _Sp &__dst) noexcept;
	//{ _MFW_TO_STRING_HELPER(__src, _Tp, __dst, _Sp) }
}