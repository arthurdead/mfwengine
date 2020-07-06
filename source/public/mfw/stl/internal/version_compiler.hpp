#define MFW_COMPILER_UNIX_FLAG _MFW_BIT(0)
#define MFW_COMPILER_CLANG_FLAG _MFW_BIT(1)
#define MFW_COMPILER_MSVC_FLAG _MFW_BIT(2)
#define _MFW_COMPILER_FLAGS_LAST_BIT 2
#define _MFW_CLANG_FLAGS (MFW_COMPILER_UNIX_FLAG|MFW_COMPILER_CLANG_FLAG)
#define MFW_COMPILER_MSVC (MFW_COMPILER_MSVC_FLAG|_MFW_BIT(_MFW_COMPILER_FLAGS_LAST_BIT+1))
#define MFW_COMPILER_MSVC_CLANG (MFW_COMPILER_MSVC_FLAG|_MFW_CLANG_FLAGS|_MFW_BIT(_MFW_COMPILER_FLAGS_LAST_BIT+2))
#define MFW_COMPILER_CLANG (_MFW_CLANG_FLAGS|_MFW_BIT(_MFW_COMPILER_FLAGS_LAST_BIT+3))
#define MFW_COMPILER_GCC (MFW_COMPILER_UNIX_FLAG|_MFW_BIT(_MFW_COMPILER_FLAGS_LAST_BIT+4))

#define MFW_COMPILER_FLAGGED(what) (MFW_COMPILER & MFW_COMPILER_##what##_FLAG)
#define MFW_COMPILER_IS(what) (MFW_COMPILER == MFW_COMPILER_##what)

#if defined __clang__ || \
	defined __clang_major__ || \
	defined __clang_minor__ || \
	defined __clang_patchlevel__ || \
	defined __clang_version__
	#define _MFW_DETECTED_CLANG
#endif

#if defined _MSC_FULL_VER || \
	defined _MSC_VER || \
	defined _MSVC_LANG || \
	defined _MSC_BUILD
	#ifdef _MFW_DETECTED_CLANG
		#define MFW_COMPILER MFW_COMPILER_MSVC_CLANG
	#else
		#define MFW_COMPILER MFW_COMPILER_MSVC
	#endif
#elif defined _MFW_DETECTED_CLANG
	#define MFW_COMPILER MFW_COMPILER_CLANG
#elif defined __GNUC__ || \
		defined __GNUC_MINOR__ || \
		defined __GNUC_PATCHLEVEL__ || \
		defined __GNUG__ || \
		defined __GNUC_VERSION__
	#define MFW_COMPILER MFW_COMPILER_GCC
#elif defined __cpp_check
	#define MFW_COMPILER MFW_COMPILER_GCC
#else
	#error
#endif

#if defined __EMSCRIPTEN__ || \
	defined EMSCRIPTEN || \
	defined __EMSCRIPTEN_major__ || \
	defined __EMSCRIPTEN_minor__ || \
	defined __EMSCRIPTEN_tiny__ || \
	defined __native_client__ || \
	MFW_HAS_FEATURE(pnacl) || \
	defined __pnacl__ || \
	defined __asmjs__
	#define _MFW_WEB_DETECTED
#endif