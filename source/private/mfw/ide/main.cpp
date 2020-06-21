#include <public/mfw/core/application.hpp>
#include <public/mfw/renderer/renderer.hpp>
#include <private/mfw/ide/ide.hpp>

bool application_load_libraries()
{
	return true;
}

::mfw::core::exit_status application_main()
{
	::mfw::core::exit_status status{};

#if MFW_OS_IS(WINDOWS)
	status = renderer_main(
	#if MFW_OS == MFW_OS_WINDOWS
		false
	#endif
	);

	if(!status.succeded()) {
		return status;
	}
#endif

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
	::mfw::core::exit_status status{};

#if MFW_OS_IS(WINDOWS)
	status = renderer_exit(
	#if MFW_OS == MFW_OS_WINDOWS
		false
	#endif
	);

	if(!status.succeded()) {
		return status;
	}
#endif

	return status;
}