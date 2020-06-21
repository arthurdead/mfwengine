#include <public/mfw/core/application.hpp>

bool application_load_libraries()
{
	for(::mfw::ucstring_view it : {
		u8"renderer"_sv,
		u8"engine"_sv,
		//u"scripting"_sv,
	}) {
	#ifdef __MFW_CORE_IS_DELAY_LOADED
		::mfw::stl::pstring file{u8"core/bin"_p / __MFW_TARGET_TRIPLE / it};
	#else
		::mfw::stl::pstring file{it};
	#endif
	#if MFW_OS_IS(WINDOWS)
		file.replace_extension(L".dll"_p);
	#elif MFW_OS_IS(LINUX)
		file.replace_extension(u8".so"_p);
	#else
		#error
	#endif
		if(!::mfw::core::core_load_library({file})) {
			return false;
		}
	}
	
	return true;
}

::mfw::core::exit_status application_main()
{
	::mfw::core::exit_status status{};

	while(true) {
		status += ::mfw::core::core_update();
		if(!status.succeded()) {
			break;
		}
	}

	return status;
}

::mfw::core::exit_status application_exit()
{
	return {};
}