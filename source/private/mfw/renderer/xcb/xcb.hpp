#ifndef __MFW_PRIVATE_RENDERER_XCB_XCB_H
#define __MFW_PRIVATE_RENDERER_XCB_XCB_H

#pragma once

#include <xcb/xcb.h>
#include <xcb/xproto.h>
#include <xcb/randr.h>

#include <X11/Xlib-xcb.h>

#define __MFW_XCB_REPLY_FUNC(base, var, cargs) \
	base##_cookie_t var##_cookie{base##_unchecked cargs }; \
	base##_reply_t *var##_reply{base##_reply(xcb_disp, var##_cookie, nullptr)};

#define __MFW_XCB_ITER_FUNC(base, var, type, ...) \
	if(var##_reply) { \
		int32_t mon_len{base##_length(var##_reply)}; \
		type##_iterator_t var##_it{base##_iterator(var##_reply)}; \
		for(int32_t var##_i{0}; var##_i < var##_len; var##_i++) { \
			const type##_t &var##_data{*var##_it.data}; \
			__VA_ARGS__ \
			type##_next(&var##_it); \
		} \
		delete var##_reply; \
	}

#define __MFW_XCB_ARR_FUNC(base, var, type, ...) \
	if(var##_reply) { \
		int32_t var##_len{base##_length(var##_reply)}; \
		const type *var##_arr{base(var##_reply)}; \
		for(int32_t var##_i{0}; var##_i < var##_len; var##_i++) { \
			const type &var##_data{var##_arr[var##_i]}; \
			__VA_ARGS__ \
		} \
		delete var##_reply; \
	}

#define __MFW_XCB_MAKE_NAME(str, var) \
	str.assign(uc_str(var##_nm_ptr), static_cast<size_t>(var##_nm_len));

#define __MFW_XCB_NAME_FUNC(base, var, ...) \
	if(var##_reply) { \
		int32_t var##_nm_len{base##_name_length(var##_reply)}; \
		const char *var##_nm_ptr{base##_name(var##_reply)}; \
		__VA_ARGS__ \
		delete var##_reply; \
	}

namespace mfw::renderer
{
	void get_atom_string(xcb_connection_t *xcb_disp, xcb_atom_t atom, ucstring &str);
}

#endif