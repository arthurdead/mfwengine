#define MFW_C_94 199409L
#define MFW_C_99 199901L
#define MFW_C_11 201112L
#define MFW_C_17 201710L

#define MFW_C_REALLY_OLD (MFW_C_94/2)
#define MFW_C_REALLY_NEW (MFW_C_17*2)

#define MFW_C_COMPARE(cmp, then) (MFW_C_VERSION cmp MFW_C_##then)

#ifdef __STDC_VERSION__
	#define MFW_C_VERSION __STDC_VERSION__
#else
	#ifdef MFW_CPP
		#define MFW_C_VERSION MFW_C_REALLY_NEW
	#else
		#define MFW_C_VERSION MFW_C_REALLY_OLD
	#endif
#endif

#define MFW_LIBC_UNIX_FLAG _MFW_BIT(1)
#define _MFW_LIBC_FLAGS_LAST_BIT 1
#define MFW_LIBC_MS (_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+1))
#define MFW_LIBC_GNU (MFW_LIBC_UNIX_FLAG|_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+2))
#define MFW_LIBC_MUSL (MFW_LIBC_UNIX_FLAG|_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+3))
#define MFW_LIBC_BIONIC (MFW_LIBC_UNIX_FLAG|_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+4))
#define MFW_LIBC_WASI (MFW_LIBC_UNIX_FLAG|_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+5))
#define MFW_LIBC_CLOUDABI (MFW_LIBC_UNIX_FLAG|_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+6))
#define MFW_LIBC_UCLIBC (MFW_LIBC_UNIX_FLAG|_MFW_BIT(_MFW_LIBC_FLAGS_LAST_BIT+7))

#define MFW_LIBC_FLAGGED(what) (MFW_LIBC & MFW_LIBC_##what##_FLAG)
#define MFW_LIBC_IS(what) (MFW_LIBC == MFW_LIBC_##what)

#if defined __wasi_core_h
	#define MFW_LIBC MFW_LIBC_WASI
#elif defined __UCLIBC__ || \
		defined __UCLIBC_MAJOR__ || \
		defined __UCLIBC_MINOR__ || \
		defined __UCLIBC_SUBLEVEL__
	#define MFW_LIBC MFW_LIBC_UCLIBC
#elif defined __cloudlibc__ || \
		defined __cloudlibc_major__ || \
		defined __cloudlibc_minor__
	#define MFW_LIBC MFW_LIBC_CLOUDABI
#if defined __BIONIC__
	#define MFW_LIBC MFW_LIBC_BIONIC
#elif defined __MUSL__ || \
		defined __musl__ || \
		defined _LIBCPP_HAS_MUSL_LIBC
	#define MFW_LIBC MFW_LIBC_MUSL
#elif defined __GNU_LIBRARY__ || \
		defined __GNU_LIBRARY_MINOR__ || \
		defined __GLIBC__ || \
		defined __GLIBC_MINOR__
	#define MFW_LIBC MFW_LIBC_GNU
#elif MFW_COMPILER_FLAGGED(MSVC) || \
		defined _WIN32_C_LIB
	#define MFW_LIBC MFW_LIBC_MS
#else
	#error
#endif

#define MFW_STDC_DEFAULT 0
#define MFW_STDC_EA 1

#define MFW_STDC_IS(what) (MFW_STDC == MFW_STDC_##what)

#if MFW_STDCPP_IS(EA)
	#define MFW_STDC MFW_STDC_EA
#else
	#define MFW_STDC MFW_STDC_DEFAULT
#endif