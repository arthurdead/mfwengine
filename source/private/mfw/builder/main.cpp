#include <public/mfw/core/application.hpp>
#include <private/mfw/builder/builder.hpp>

bool application_load_libraries()
{
	return true;
}

::mfw::core::exit_status application_main()
{
	::mfw::core::core_update();
	::mfw::builder::builder &builder{::mfw::builder::builder::instance()};
	return builder.start();
}

::mfw::core::exit_status application_exit()
{
	return {};
}