#define MFW_PROCESSOR_64BITS_FLAG _MFW_BIT(0)
#define MFW_PROCESSOR_32BITS_FLAG _MFW_BIT(1)
#define MFW_PROCESSOR_X86_FLAG _MFW_BIT(2)
#define MFW_PROCESSOR_ARM_FLAG _MFW_BIT(3)
#define _MFW_PROCESSOR_FLAGS_LAST_BIT 3
#define MFW_PROCESSOR_X86_64 (MFW_PROCESSOR_X86_FLAG|MFW_PROCESSOR_64BITS_FLAG|_MFW_BIT(_MFW_PROCESSOR_FLAGS_LAST_BIT+1))
#define MFW_PROCESSOR_X86 (MFW_PROCESSOR_X86_FLAG|MFW_PROCESSOR_32BITS_FLAG|_MFW_BIT(_MFW_PROCESSOR_FLAGS_LAST_BIT+2))
#define MFW_PROCESSOR_ARM (MFW_PROCESSOR_ARM_FLAG|MFW_PROCESSOR_32BITS_FLAG|_MFW_BIT(_MFW_PROCESSOR_FLAGS_LAST_BIT+3))
#define MFW_PROCESSOR_AARCH64 (MFW_PROCESSOR_ARM_FLAG|MFW_PROCESSOR_64BITS_FLAG|_MFW_BIT(_MFW_PROCESSOR_FLAGS_LAST_BIT+4))

#define MFW_PROCESSOR_FLAGGED(what) (MFW_PROCESSOR & MFW_PROCESSOR_##what##_FLAG)
#define MFW_PROCESSOR_IS(what) (MFW_PROCESSOR == MFW_PROCESSOR_##what)

#if defined _M_X64 || \
	defined _M_AMD64 || \
	defined __x86_64__ || \
	defined __x86_64 || \
	defined __amd64 || \
	defined __amd64__ || \
	defined __ia64__ || \
	defined _IA64 || \
	defined __IA64__ || \
	defined __ia64 || \
	defined _M_IA64 || \
	defined __itanium__
	#define MFW_PROCESSOR MFW_PROCESSOR_X86_64
#elif defined __aarch64__ || \
		defined __arm64__
	#define MFW_PROCESSOR MFW_PROCESSOR_AARCH64
#elif defined __arm__ || \
		defined __TARGET_ARCH_ARM || \
		defined _ARM || \
		defined _M_ARM || \
		defined __arm
	#define MFW_PROCESSOR MFW_PROCESSOR_ARM
#elif defined i386 || \
		defined __i386 || \
		defined __i386__ || \
		defined __i486__ || \
		defined __i586__ || \
		defined __i686__ || \
		defined _M_I86 || \
		defined _M_IX86 || \
		defined __X86__ || \
		defined _X86_ || \
		defined __I86__ || \
		defined __386 || \
		defined __INTEL__
	#define MFW_PROCESSOR MFW_PROCESSOR_X86
#elif defined __MFW_BROWSER_DETECTED
	_MFW_MESSAGE("not sure")
	#define MFW_PROCESSOR MFW_PROCESSOR_X86
#else
	#error
#endif