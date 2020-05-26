#include <cassert>
#include <unistd.h>
#include <cstdio>
#include <dirent.h>
#include <glob.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <public/mfw/core/application.hpp>
#include <public/mfw/stl/iterator.hpp>
#include <public/mfw/stl/unordered_map.hpp>
#include <public/mfw/stl/string.hpp>
#include <public/mfw/stl/vector.hpp>
#include <public/mfw/stl/filesystem.hpp>
#include <public/mfw/core/filesystem_interface.hpp>
#include <public/mfw/core/commandline.hpp>
#include <public/mfw/core/serializable.hpp>
#include <public/mfw/core/logging_interface.hpp>

#include <xcb/xcb.h>
#include <xcb/randr.h>

using namespace mfw;
using namespace mfw::stl;

MFW_DECLARE_LOG_CONTEXT(log_codetest, u8"codetest"_p);

bool application_load_libraries()
{
	return true;
}

mfw::core::exit_status application_exit()
{
	return {};
}

mfw::core::exit_status application_main()
{
	/*
	DIR *x11dir{opendir("/tmp/.X11-unix")};
	if(x11dir) {
		dirent *entry{nullptr};
		while(true) {
			entry = readdir(x11dir);
			if(!entry) {
				break;
			}
			
			if(entry->d_name[0] != 'X') {
				continue;
			}
			
			char display_name[64]{":"};
			strcat(display_name, entry->d_name + 1);
			
			xcb_connection_t *xdisp{xcb_connect(display_name, nullptr)};
			if(!xcb_connection_has_error(xdisp)) {
				const xcb_setup_t *setup{xcb_get_setup(xdisp)};
				
				char *vend = xcb_setup_vendor(setup);
				
				xcb_screen_iterator_t it{xcb_setup_roots_iterator(setup)};
				
				int32_t numscreen{xcb_setup_roots_length(setup)};
				for(int32_t i{0}; i < numscreen; i++) {
					xcb_screen_t *scrn{it.data};
					
					xcb_screen_next(&it);
				}
				
				xcb_screen_end(it);
			}
		}
		closedir(x11dir);
	}
	
	while(true) {}
	*/
	
	::mfw::core::exit_status status{::mfw::core::core_update()};
	
	mfw::core::serializable test{};
	if(!test.from_file(u8"input.sr"_p)) {
		log_codetest().error(u8"failed to parse"_sv);
		status.set_failed();
		return status;
	}
	
	ucstring output{};
	test.to_string(output);
	test.to_file({u8"output.sr"_p});
	
	log_codetest().info(output);
	
	return status;
}