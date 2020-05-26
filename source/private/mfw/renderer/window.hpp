#ifndef __MFW_PRIVATE_RENDERER_WINDOW_H
#define __MFW_PRIVATE_RENDERER_WINDOW_H

#pragma once

#include <public/mfw/stl/version.hpp>
#include <public/mfw/stl/forward_list.hpp>
#include <private/mfw/renderer/helpers.hpp>

#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <xcb/xcb.h>
#endif

#if MFW_OS == MFW_OS_WINDOWS
	#define __MFW_WM_USER_UNIQUE (WM_USER+300)

	#define MFW_WM_PRE_UPDATE (__MFW_WM_USER_UNIQUE+0)
	#define MFW_WM_POST_UPDATE (__MFW_WM_USER_UNIQUE+1)

	#define MFW_WM_USER_UNIQUE (__MFW_WM_USER_UNIQUE+2)
#endif

namespace mfw::renderer
{
#if MFW_OS == MFW_OS_WINDOWS
	enum class window_classname : int8_t
	{
		button,
		combobox,
		edit,
		listbox,
		scrollbar,
		static_,
	};
#endif

	namespace agnostic
	{
		class gpu;
	};

	class window
	{
	protected:
		window();

	public:
	#if MFW_OS == MFW_OS_WINDOWS
		window(const window *parent, int32_t style, int32_t exstyle);
		window(const window *parent, ucstring_view classname, int32_t style, int32_t exstyle);
		window(const window *parent, window_classname classname, int32_t style, int32_t exstyle);
	#elif MFW_OS == MFW_OS_LINUX
		window(const window *parent, const agnostic::gpu &gpu_);
	#else
		#error
	#endif
		virtual ~window();
		
	#if MFW_OS == MFW_OS_WINDOWS
		using handle_t = HWND;
		static inline constexpr handle_t invalid_handle{nullptr};
	#elif MFW_OS == MFW_OS_LINUX
		using handle_t = xcb_window_t;
		static inline constexpr handle_t invalid_handle{0};
	#else
		#error
	#endif

		handle_t handle() const { return handle_; }

	#if MFW_OS == MFW_OS_WINDOWS
		HDC device() const { return device_; }

		int32_t get_style(bool ex) const;
		void set_style(int32_t style, bool ex);
	#endif

		static handle_t desktop_window();

		void enable(bool enabled);

		bool minimized() const;

		void set_bounds(int32_t x, int32_t y, int32_t w, int32_t h);
		void get_bounds(int32_t *const x, int32_t *const y, int32_t *const w, int32_t *const h) const;

		void set_title(const ucstring_view &title);
		ucstring get_title() const;

		const window *parent() const;
		static window *get_window(handle_t wnd);

	#if MFW_OS == MFW_OS_WINDOWS
		static HINSTANCE instance();
	#endif

		static void initialize();
		static void update();
		static void shutdown();

	protected:
	#if MFW_OS == MFW_OS_WINDOWS
		virtual int64_t window_proc(uint32_t msg, uint64_t param1, int64_t param2);
		int64_t def_window_proc(uint32_t msg, uint64_t param1, int64_t param2);
	#endif

		virtual void on_command(uint16_t code) {}

		virtual void min_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const;
		virtual void max_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const;

	#if MFW_OS == MFW_OS_WINDOWS
		void update_styles();
		void handle_messages();

		void init_window(int32_t exstyle, ucstring_view classname, int32_t style, const window *parent);
	#endif
	
		RECT rect() const;

		void clamp_bounds(int32_t &x, int32_t &y, int32_t &w, int32_t &h) const;

	#if MFW_OS == MFW_OS_WINDOWS
		void set_bounds(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t flags);
	#endif

	#if MFW_OS == MFW_OS_WINDOWS
		static int64_t global_window_proc(handle_t wnd, uint32_t msg, uint64_t param1, int64_t param2);
	#endif
		
		using window_list_t = forward_list<unique_ptr<window>>;
		static window_list_t &windows();

		handle_t handle_{invalid_handle};
	#if MFW_OS == MFW_OS_WINDOWS
		HDC device_{nullptr};
	#elif MFW_OS == MFW_OS_LINUX
		xcb_connection_t *device_{nullptr};
	#endif
	};
};

#endif