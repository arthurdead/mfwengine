#define MFW_OS_WINDOWS 0
#define MFW_OS_LINUX 1

#define MFW_OS_IS(what) (MFW_OS == MFW_OS_##what)

#if defined __linux__ || \
	defined linux || \
	defined __linux || \
	defined __gnu_linux__ || \
	defined __GNU__ || \
	defined unix || \
	defined __unix__ || \
	defined __unix
	#define MFW_OS MFW_OS_LINUX
#elif defined _WIN64 || \
		defined WIN64 || \
		defined _WIN32 || \
		defined WIN32 || \
		defined _WINDOWS || \
		defined WINDOWS || \
		defined __WIN32__ || \
		defined __WINDOWS__
	#define MFW_OS MFW_OS_WINDOWS
#else
	#error
#endif

#ifdef WINDOWS
	#undef WINDOWS
#endif

#define MFW_PLATFORM_DESKTOP 0
#define MFW_PLATFORM_MOBILE 1

#define MFW_PLATFORM_IS(what) (MFW_PLATFORM == MFW_PLATFORM_##what)

#if MFW_OS_IS(WINDOWS)
	#define MFW_PLATFORM MFW_PLATFORM_DESKTOP
#elif MFW_OS_IS(LINUX)
	#if defined __ANDROID__ || \
		defined __ANDROID_API__
		#define MFW_PLATFORM MFW_PLATFORM_MOBILE
	#else
		#define MFW_PLATFORM MFW_PLATFORM_DESKTOP
	#endif
#else
	#error
#endif

#if MFW_OS_IS(WINDOWS)
	#define MFW_CHARACTERSET_MULTIBYTE 0
	#define MFW_CHARACTERSET_UNICODE 1

	#define MFW_CHARACTERSET_IS(what) (MFW_CHARACTERSET == MFW_CHARACTERSET_##what)

	#if defined _UNICODE || \
		defined UNICODE
		#define MFW_CHARACTERSET MFW_CHARACTERSET_UNICODE
	#elif defined _MBCS || \
			defined MBCS
		#define MFW_CHARACTERSET MFW_CHARACTERSET_MULTIBYTE
	#else
		#define MFW_CHARACTERSET MFW_CHARACTERSET_MULTIBYTE
	#endif

	#ifdef UNICODE
		#undef UNICODE
	#endif
#endif