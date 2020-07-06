#if MFW_COMPILER_FLAGGED(MSVC)
	#define MFW_DECLSPEC(x) __declspec(x)
	#define MFW_ATTRIBUTE(x)
	#define MFW_SHARED_EXPORT __declspec(dllexport)
	#define MFW_SHARED_IMPORT __declspec(dllimport)
	#define MFW_NOVTABLE __declspec(novtable)
	#define MFW_NODEBUG 
	#define MFW_INHERITANCE(x) __##x##_inheritance
	#define MFW_VISIBILITY_PUSH(x) 
	#define MFW_VISIBILITY_POP() 
	#define MFW_VISIBILITY_PUBLIC 
	#define MFW_VISIBILITY_LOCAL 
	#define MFW_CALL_STDCALL __stdcall
	#define MFW_CALL_CDECL __cdecl
	#define MFW_OPTIMIZE_PRAGMA(x) MFW_PRAGMA(optimize(x))
	#define MFW_OPTIMIZE_ATTRIBUTE(x) 
	#define MFW_RESET_OPTIONS() 
	#define MFW_PUSH_OPTIONS() 
	#define MFW_POP_OPTIONS() 
	#define MFW_WARNING_DISABLE(...) MFW_PRAGMA(warning(disable: __VA_ARGS__))
	#define MFW_WARNING_SUPPRESS(...) MFW_PRAGMA(warning(suppress: __VA_ARGS__))
	#define _MFW_WARNING_PUSH_MSVC() MFW_PRAGMA(warning(push))
	#define _MFW_WARNING_POP_MSVC() MFW_PRAGMA(warning(pop))
	#if MFW_COMPILER_FLAGGED(CLANG)
		#define MFW_WARNING_DISABLE_UNIX(x) MFW_PRAGMA_UNIX(clang diagnostic ignored x)
		#define _MFW_WARNING_PUSH_UNIX() MFW_PRAGMA_UNIX(clang diagnostic push)
		#define _MFW_WARNING_POP_UNIX() MFW_PRAGMA_UNIX(clang diagnostic pop)
		#define MFW_WARNING_PUSH() _MFW_WARNING_PUSH_UNIX() _MFW_WARNING_PUSH_MSVC()
		#define MFW_WARNING_POP() _MFW_WARNING_POP_UNIX() _MFW_WARNING_POP_MSVC()
	#else
		#define MFW_WARNING_PUSH _MFW_WARNING_PUSH_MSVC
		#define MFW_WARNING_POP _MFW_WARNING_POP_MSVC
	#endif
	#define MFW_EXTENSION
	#define MFW_NOTHROW __declspec(nothrow)
	#define MFW_FLAG_ENUM 
	#define MFW_DEBUGBREAK() __debugbreak()
	#define MFW_ABSTRACT_CLASS __declspec(novtable) __single_inheritance
#elif MFW_COMPILER_FLAGGED(UNIX)
	#define MFW_DECLSPEC(x) 
	#define MFW_NOVTABLE 
	#define MFW_INHERITANCE(x) 
	#ifdef MFW_VA_MACROS_SUPPORTED
		#define MFW_ATTRIBUTE(...) __attribute__((__VA_ARGS__))
	#else
		#define MFW_ATTRIBUTE(x) __attribute__((x))
	#endif
	#define MFW_VISIBILITY_PUBLIC __attribute__((__visibility__("default"), __lto_visibility_public__))
	#if MFW_OS_IS(WINDOWS)
		#define MFW_SHARED_EXPORT __attribute__((__dllexport__)) MFW_VISIBILITY_PUBLIC
		#define MFW_SHARED_IMPORT __attribute__((__dllimport__)) MFW_VISIBILITY_PUBLIC
		#define _MFW_SYSTEM_ABI __attribute__((__ms_abi__))
	#elif MFW_OS_IS(LINUX)
		#define MFW_SHARED_EXPORT MFW_VISIBILITY_PUBLIC
		#define MFW_SHARED_IMPORT MFW_VISIBILITY_PUBLIC
		#define _MFW_SYSTEM_ABI __attribute__((__sysv_abi__))
	#else
		#error
	#endif
	#if MFW_PROCESSOR_FLAGGED(X86)
		#define MFW_CALL_STDCALL _MFW_SYSTEM_ABI __attribute__((__stdcall__))
		#define MFW_CALL_CDECL _MFW_SYSTEM_ABI __attribute__((__cdecl__))
	#elif MFW_PROCESSOR_FLAGGED(ARM)
		#define MFW_CALL_STDCALL __attribute__((__pcs__("aapcs")))
		#define MFW_CALL_CDECL 
	#else
		#error
	#endif
	#if MFW_COMPILER_FLAGGED(CLANG)
		#define _MFW_PRAGMA_ID clang
		#define MFW_OPTIMIZE_PRAGMA(x) 
		#define MFW_OPTIMIZE_ATTRIBUTE(x) 
		#define MFW_RESET_OPTIONS() 
		#define MFW_PUSH_OPTIONS() 
		#define MFW_POP_OPTIONS() 
		#define _MFW_INTERNAL_LINKAGE __attribute__((__internal_linkage__))
		#define MFW_NODEBUG __attribute__((__nodebug__))
		#define MFW_FLAG_ENUM __attribute__((__flag_enum__))
		#define MFW_DEBUGBREAK() __builtin_debugtrap()
	#elif MFW_COMPILER_IS(GCC)
		#define _MFW_PRAGMA_ID GCC
		#ifdef MFW_VA_MACROS_SUPPORTED
			#define MFW_OPTIMIZE_PRAGMA(...) MFW_PRAGMA(_MFW_PRAGMA_ID optimize(__VA_ARGS__))
			#define MFW_OPTIMIZE_ATTRIBUTE(...) __attribute__((__optimize__(__VA_ARGS__)))
		#else
			#define MFW_OPTIMIZE_PRAGMA(x) MFW_PRAGMA(_MFW_PRAGMA_ID optimize(x))
			#define MFW_OPTIMIZE_ATTRIBUTE(x) __attribute__((__optimize__(x)))
		#endif
		#define MFW_RESET_OPTIONS() MFW_PRAGMA(_MFW_PRAGMA_ID reset_options)
		#define MFW_PUSH_OPTIONS() MFW_PRAGMA(_MFW_PRAGMA_ID push_options)
		#define MFW_POP_OPTIONS() MFW_PRAGMA(_MFW_PRAGMA_ID pop_options)
		#define _MFW_INTERNAL_LINKAGE 
		#define MFW_NODEBUG 
		#define MFW_FLAG_ENUM 
		#if MFW_PROCESSOR_FLAGGED(X86)
			#define MFW_DEBUGBREAK() __asm__("int3")
		#else
			#define MFW_DEBUGBREAK() __builtin_trap()
		#endif
	#else
		#error
	#endif
	#define MFW_VISIBILITY_LOCAL __attribute__((__visibility__("internal"))) _MFW_INTERNAL_LINKAGE
	#define MFW_VISIBILITY_PUSH(x) MFW_PRAGMA(_MFW_PRAGMA_ID visibility push(x))
	#define MFW_VISIBILITY_POP() MFW_PRAGMA(_MFW_PRAGMA_ID visibility pop)
	#define MFW_WARNING_PUSH() MFW_PRAGMA(_MFW_PRAGMA_ID diagnostic push)
	#define MFW_WARNING_POP() MFW_PRAGMA(_MFW_PRAGMA_ID diagnostic pop)
	#define MFW_WARNING_DISABLE(x) MFW_PRAGMA(_MFW_PRAGMA_ID diagnostic ignored x)
	#define MFW_WARNING_SUPPRESS MFW_WARNING_DISABLE
	#define MFW_WARNING_DISABLE_UNIX MFW_WARNING_DISABLE
	#define MFW_EXTENSION __extension__
	#define MFW_NOTHROW __attribute__((__nothrow__))
	#define MFW_ABSTRACT_CLASS 
#else
	#error
#endif