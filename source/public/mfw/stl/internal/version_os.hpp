#define MFW_OS_UNIX_FLAG _MFW_BIT(0)
#define _MFW_OS_LAST_FLAG 0
#define MFW_OS_WINDOWS (_MFW_BIT(_MFW_OS_LAST_FLAG+1))
#define MFW_OS_LINUX (MFW_OS_UNIX_FLAG|_MFW_BIT(_MFW_OS_LAST_FLAG+2))
#define MFW_OS_ANDROID (MFW_OS_UNIX_FLAG|_MFW_BIT(_MFW_OS_LAST_FLAG+3))
#define MFW_OS_WEB (MFW_OS_UNIX_FLAG|_MFW_BIT(_MFW_OS_LAST_FLAG+4))
#define MFW_OS_MACOS (MFW_OS_UNIX_FLAG|_MFW_BIT(_MFW_OS_LAST_FLAG+5))

#define MFW_OS_IS(what) (MFW_OS == MFW_OS_##what)
#define MFW_OS_FLAGGED(what) (MFW_OS & MFW_OS_##what##_FLAG)

#ifdef _MFW_WEB_DETECTED
	#define MFW_OS MFW_OS_WEB
#elif defined __ANDROID__ || \
		defined __ANDROID_API__
	#define MFW_OS MFW_OS_ANDROID
#elif defined macintosh || \
		defined Macintosh || \
		defined __APPLE__
	#define MFW_OS MFW_OS_MACOS
#elif defined __linux__ || \
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
#define MFW_PLATFORM_WEB 2

#define MFW_PLATFORM_IS(what) (MFW_PLATFORM == MFW_PLATFORM_##what)

#if MFW_OS_IS(ANDROID)
	#define MFW_PLATFORM MFW_PLATFORM_MOBILE
#elif MFW_OS_IS(WINDOWS) || \
		MFW_OS_IS(LINUX) || \
		MFW_OS_IS(MACOS)
	#define MFW_PLATFORM MFW_PLATFORM_DESKTOP
#elif MFW_OS_IS(WEB)
	#define MFW_PLATFORM MFW_PLATFORM_WEB
#else
	#error
#endif

#define MFW_CHARACTERSET_MULTIBYTE 0
#define MFW_CHARACTERSET_UNICODE 1

#define MFW_CHARACTERSET_IS(what) (MFW_CHARACTERSET == MFW_CHARACTERSET_##what)

#if MFW_OS_IS(WINDOWS)
	#if defined _UNICODE || \
		defined UNICODE
		#define MFW_CHARACTERSET MFW_CHARACTERSET_UNICODE
	#elif defined _MBCS || \
			defined MBCS
		#define MFW_CHARACTERSET MFW_CHARACTERSET_MULTIBYTE
	#else
		#error
	#endif
#else
	#define MFW_CHARACTERSET MFW_CHARACTERSET_MULTIBYTE
#endif

#ifdef UNICODE
	#undef UNICODE
#endif