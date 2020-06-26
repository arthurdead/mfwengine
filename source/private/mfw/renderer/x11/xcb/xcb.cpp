#include <private/mfw/renderer/x11/xcb/xcb.hpp>

namespace mfw::renderer
{
	void get_atom_string(xcb_connection_t *xcb_disp, xcb_atom_t atom, ucstring &str)
	{
		__MFW_XCB_REPLY_FUNC(xcb_get_atom_name, atm_name, (xcb_disp, atom))
		__MFW_XCB_NAME_FUNC(xcb_get_atom_name, atm_name, {
			__MFW_XCB_MAKE_NAME(str, atm_name)
		})
	}
}