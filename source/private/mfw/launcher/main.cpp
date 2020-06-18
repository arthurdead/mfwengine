#include <public/mfw/core/application.hpp>

bool application_load_libraries()
{
	for(::mfw::ucstring_view it : {
		u8"renderer"_sv,
		u8"engine"_sv,
		//u"scripting"_sv,
	}) {
		if(!::mfw::core::core_load_library(it)) {
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