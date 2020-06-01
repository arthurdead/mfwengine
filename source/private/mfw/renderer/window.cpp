#include <private/mfw/renderer/window.hpp>
#include <private/mfw/renderer/helpers.hpp>
#include <public/mfw/stl/algorithm.hpp>
#if MFW_OS == MFW_OS_WINDOWS
	#include <Windows.h>
	#include <CommCtrl.h>
#elif MFW_OS == MFW_OS_LINUX
	#include <public/mfw/stl/unordered_set.hpp>
	#include <private/mfw/renderer/gpu.hpp>
	#include <X11/Xlib.h>
#endif

namespace mfw::renderer
{
	window::window_list_t &window::windows() {
		static window_list_t _windows{};
		return _windows;
	}

#if MFW_OS == MFW_OS_WINDOWS
	int64_t window::global_window_proc(handle_t wnd, uint32_t msg, uint64_t param1, int64_t param2)
	{
		window *window_{get_window(wnd)};
		if(window_) {
			int64_t res{window_->window_proc(msg, param1, param2)};
			return res;
		} else {
			if(msg == WM_NCCREATE) {
				const CREATESTRUCTW *create{reinterpret_cast<const CREATESTRUCTW *>(param2)};
				const window *created{reinterpret_cast<const window *>(create->lpCreateParams)};
				SetWindowLongPtrW(wnd, GWLP_USERDATA, reinterpret_cast<int64_t>(created));
				return 1;
			}
		}

		return DefWindowProcW(wnd, msg, param1, param2);
	}

	int64_t window::window_proc(uint32_t msg, uint64_t param1, int64_t param2)
	{
		if(msg == WM_MOVING) {
			RECT *rect{reinterpret_cast<RECT *>(param2)};

			int32_t tw{0}; int32_t th{0};
			int32_t tx{0}; int32_t ty{0};
			get_rect_bounds(*rect, &tx, &ty, &tw, &th);

			clamp_bounds(tx, ty, tw, th);

			set_rect_bounds(*rect, tx, ty, tw, th);

			return 1;
		} else if(msg == WM_GETMINMAXINFO) {
			MINMAXINFO *info{reinterpret_cast<MINMAXINFO *>(param2)};

			int32_t mw{0}; int32_t mh{0};
			max_bounds(nullptr, nullptr, &mw, &mh);

			int32_t iw{0}; int32_t ih{0};
			min_bounds(nullptr, nullptr, &iw, &ih);

			info->ptMaxSize.x = mw;
			info->ptMaxSize.y = mh;

			info->ptMaxPosition.x = 0;
			info->ptMaxPosition.y = 0;

			info->ptMinTrackSize.x = iw;
			info->ptMinTrackSize.y = ih;

			info->ptMaxTrackSize.x = info->ptMaxSize.x;
			info->ptMaxTrackSize.y = info->ptMaxSize.y;
			return 0;
		} else if(msg == WM_SIZE) {
			/*int32_t width{static_cast<const int32_t>(LOWORD(param2))};
			int32_t height{static_cast<const int32_t>(HIWORD(param2))};

			int32_t x{0}; int32_t y{0};
			int32_t w{0}; int32_t h{0};
			GetBounds(&x, &y, &w, &h);

			ClampBounds(x, y, width, height);

			if(width != w || height != h) {
				SetBounds(x, y, width, height);
			}*/
			return 0;
		} else if(msg == WM_COMMAND) {
			//window *child{reinterpret_cast<window *>(LOWORD(param2))};
			window *child{reinterpret_cast<window *>(param1)};
			uint16_t code{HIWORD(param2)};

			child->on_command(code);

			return 0;
		} else if(msg == WM_DESTROY) {
			delete this;
			return 0;
		}

		return def_window_proc(msg, param1, param2);
	}
#endif

	void window::clamp_bounds(int32_t &x, int32_t &y, int32_t &w, int32_t &h) const
	{
		int32_t mw{0}; int32_t mh{0};
		int32_t mx{0}; int32_t my{0};
		max_bounds(&mx, &my, &mw, &mh);

		int32_t iw{0}; int32_t ih{0};
		//int32_t ix{0}; int32_t iy{0};
		//min_bounds(&ix, &iy, &iw, &ih);
		min_bounds(nullptr, nullptr, &iw, &ih);

		//x = clamp(x, ix, mx);
		//y = clamp(y, iy, my);

		w = clamp(w, iw, mw);
		h = clamp(h, ih, mh);

		int32_t rw{mw-w};
		int32_t rh{mh-h};

		if(x == -1) {
			x = (rw / 2);
		}
		if(y == -1) {
			y = (rh / 2);
		}

		x = clamp(x, mx, (mx+rw));
		y = clamp(y, my, (my+rh));
	}

#if MFW_OS == MFW_OS_WINDOWS
	int64_t window::def_window_proc(uint32_t msg, uint64_t param1, int64_t param2)
	{
		if(msg >= MFW_WM_USER_UNIQUE) {
			return 0;
		}

		return DefWindowProcW(handle_, msg, param1, param2);
	}

	HINSTANCE window::instance()
	{
		return __instance;
	}
#endif

	void window::shutdown()
	{
		windows().clear();
	#if MFW_OS == MFW_OS_WINDOWS
		UnregisterClassW(__windowclass.lpszClassName, __windowclass.hInstance);
	#endif
	}

	window::handle_t window::desktop_window()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		return ::GetDesktopWindow();
	#elif MFW_OS == MFW_OS_LINUX
		MFW_MESSAGE("TODO!!!")
		return invalid_handle;
	#else
		#error
	#endif
	}

#if MFW_OS == MFW_OS_WINDOWS
	static HINSTANCE __instance{nullptr};
	static WNDCLASSEXW __windowclass{};
#endif
	void window::initialize()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		__instance = GetModuleHandleW(nullptr);

		INITCOMMONCONTROLSEX cmmctrls{};
		cmmctrls.dwSize = sizeof(INITCOMMONCONTROLSEX);
		InitCommonControlsEx(&cmmctrls);

		__windowclass.cbSize = sizeof(WNDCLASSEXW);
		__windowclass.style = CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS | CS_CLASSDC;
		__windowclass.lpfnWndProc = reinterpret_cast<WNDPROC>(global_window_proc);
		__windowclass.hInstance = __instance;
		__windowclass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
		__windowclass.hCursor = LoadCursorW(__windowclass.hInstance, IDC_ARROW);
		__windowclass.lpszClassName = L"window";
		__windowclass.hIcon = static_cast<HICON>(LoadImageW(__windowclass.hInstance, L"", IMAGE_ICON, 64, 64, LR_LOADFROMFILE | LR_SHARED | LR_DEFAULTCOLOR | LR_DEFAULTSIZE));
		__windowclass.hIconSm = __windowclass.hIcon;
		RegisterClassExW(&__windowclass);
	#endif
	}

	window::window()
	{
		windows().emplace_front(this);
	}

#if MFW_OS == MFW_OS_WINDOWS
	window::window(const window *parent, int32_t style, int32_t exstyle)
		: window{}
	{
		#if MFW_COMPILER == MFW_COMPILER_MSVC
			#pragma warning(suppress: 6387)
		#endif
		init_window(exstyle, uc_str(__windowclass.lpszClassName), style, parent);
	}

	window::window(const window *parent, const u16string_view &classname, int32_t style, int32_t exstyle)
		: window{}
	{
		#if MFW_COMPILER == MFW_COMPILER_MSVC
			#pragma warning(suppress: 6387)
		#endif
		init_window(exstyle, classname, style, parent);
	}

	window::window(const window *parent, window_classname classname, int32_t style, int32_t exstyle)
		: window{}
	{
		const wchar_t *strclass{nullptr};
		switch(classname)
		{
			case window_classname::button: { strclass = WC_BUTTONW; break; }
			case window_classname::combobox: { strclass = WC_COMBOBOXW; break; }
			case window_classname::edit: { strclass = WC_EDITW; break; }
			case window_classname::listbox: { strclass = WC_LISTBOXW; break; }
			case window_classname::scrollbar: { strclass = WC_SCROLLBARW; break; }
			case window_classname::static_: { strclass = WC_STATICW; break; }
		}

		#if MFW_COMPILER == MFW_COMPILER_MSVC
			#pragma warning(suppress: 6387)
		#endif
		init_window(exstyle, uc_str(strclass), style, parent);
	}

	void window::init_window(int32_t exstyle, ucstring_view classname, int32_t style, const window *parent)
	{
		handle_t parwnd{invalid_handle};
		if(parent) {
			style |= WS_CHILD;
			parwnd = parent->handle_;
		}

		handle_ = CreateWindowExW(static_cast<unsigned long>(exstyle), c_str(classname), L"", static_cast<unsigned long>(style), 0, 0, 0, 0, parwnd, nullptr, __instance, this);
		device_ = GetWindowDC(handle_);
	}

	void window::set_style(int32_t style, bool ex)
	{
		bool changed{get_style(ex) != style};
		SetWindowLongPtrW(handle_, ex ? GWL_EXSTYLE : GWL_STYLE, style);
		if(changed) {
			update_styles();
		}
	}

	void window::update_styles()
	{
		int32_t x{0}; int32_t y{0};
		int32_t w{0}; int32_t h{0}; 
		get_bounds(&x, &y, &w, &h);

		set_bounds(x, y, w, h, SWP_FRAMECHANGED);

		WINDOWPLACEMENT place{};
		place.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(handle_, &place);

		ShowWindow(handle_, SW_SHOW);

		ShowWindow(handle_, static_cast<int32_t>(place.showCmd));
	}

	int32_t window::get_style(bool ex) const
	{
		return static_cast<int32_t>(GetWindowLongPtrW(handle_, ex ? GWL_EXSTYLE : GWL_STYLE));
	}
#elif MFW_OS == MFW_OS_LINUX
	window::window(const window *parent, const agnostic::gpu &gpu_)
		: window{}
	{
		device_ = gpu_.display_;
		
		//handle_t parwnd{invalid_handle};
		if(parent) {
			//parwnd = parent->handle_;
		} else {
			MFW_MESSAGE("port to xcb")
			//parwnd = DefaultRootWindow(device_);
		}
		
		device_ = gpu_.display_;
		MFW_MESSAGE("port to xcb")
		//handle_ = XCreateWindow(device_, parwnd, 0, 0, 1, 1, 0, 0, 0, nullptr, 0, nullptr);
		
		MFW_MESSAGE("port to xcb")
		//XSelectInput(device_, handle_, NoEventMask);
	}
#endif

	bool window::minimized() const
	{
	#if MFW_OS == MFW_OS_WINDOWS
		WINDOWPLACEMENT place{};
		place.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(handle_, &place);

		return ((place.showCmd == SW_SHOWMINIMIZED) /*|| IsIconic(handle_)*/);
	#else
		MFW_MESSAGE("TODO!!!")
		return false;
	#endif
	}

	window::~window()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		SetWindowLongPtrW(handle_, GWLP_USERDATA, reinterpret_cast<int64_t>(nullptr));

		DestroyWindow(handle_);
	#elif MFW_OS == MFW_OS_LINUX
		MFW_MESSAGE("port to xcb")
		//XDestroyWindow(device_, handle_);
	#endif
	}

	void window::update()
	{
	#if MFW_OS == MFW_OS_WINDOWS
		window_list_t &list{windows()}; 
		window_list_t::const_iterator it{list.begin()};
		while(it != list.end()) {
			window *win{(*it).get()};
			it++;
			win->handle_messages();
		}
	#elif MFW_OS == MFW_OS_LINUX
		using display_set_t = unordered_set<xcb_connection_t *>;
		display_set_t displays{};
	
		window_list_t &list{windows()}; 
		window_list_t::const_iterator wit{list.begin()};
		while(wit != list.end()) {
			window *win{(*wit).get()};
			wit++;
			displays.emplace(win->device_);
		}
		
		display_set_t::const_iterator dit{displays.begin()};
		while(dit != displays.end()) {
			xcb_connection_t *xdisp{*dit};
			MFW_MESSAGE("port to xcb")
			/*int32_t queued{XEventsQueued(xdisp, QueuedAfterFlush)};
			if(queued) {
				XEvent event{};
				XNextEvent(xdisp, &event);
			}*/
			displays.erase(dit);
		}
	#endif
	}

#if MFW_OS == MFW_OS_WINDOWS
	void window::handle_messages()
	{
		window_proc(MFW_WM_PRE_UPDATE, 0, 0);

		MSG msg{};
		if(bool_cast(PeekMessageW(&msg, handle_, 0, 0, PM_REMOVE))) {
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}

		if(msg.message != WM_NCLBUTTONDOWN) {
			window_proc(MFW_WM_POST_UPDATE, 0, 0);
		}
	}
#endif

	void window::enable(bool enabled)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		ShowWindow(handle_, (enabled ? SW_SHOW : SW_HIDE));
	#elif MFW_OS == MFW_OS_LINUX
		MFW_MESSAGE("port to xcb")
		/*if(enabled) {
			XMapWindow(device_, handle_);
		} else {
			XUnmapWindow(device_, handle_);
		}*/
	#endif
	}

#if MFW_OS == MFW_OS_WINDOWS
	void window::set_bounds(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t flags)
	{
		clamp_bounds(x, y, w, h);

		SetWindowPos(handle_, nullptr, x, y, w, h, flags);
	}
#endif

	void window::set_bounds(int32_t x, int32_t y, int32_t w, int32_t h)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		set_bounds(x, y, w, h, SWP_ASYNCWINDOWPOS);
	#elif MFW_OS == MFW_OS_LINUX
		clamp_bounds(x, y, w, h);
		
		MFW_MESSAGE("port to xcb")
		//XMoveResizeWindow(device_, handle_, x, y, w, h);
	#endif
	}

	RECT window::rect() const
	{
		RECT rect{};
	#if MFW_OS == MFW_OS_WINDOWS
		GetWindowRect(handle_, &rect);
	#else
		int32_t x{0}; int32_t y{0};
		uint32_t w{0}; uint32_t h{0};
		MFW_MESSAGE("port to xcb")
		//XGetGeometry(device_, handle_, nullptr, &x, &y, &w, &h, nullptr, nullptr);
		
		set_rect_bounds(rect, x, y, w, h);
	#endif
		return rect;
	}

	void window::get_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		RECT rect_{rect()};

		get_rect_bounds(rect_, x, y, w, h);
	}

	void window::set_title(const ucstring_view &title)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		SetWindowTextW(handle_, rcast<const wchar_t *>(title.data()));
	#else
		MFW_MESSAGE("TODO!!!")
	#endif
	}

	ucstring window::get_title() const
	{
	#if MFW_OS == MFW_OS_WINDOWS
		int32_t size = GetWindowTextLengthW(handle_);
		u16string title{};
		title.resize(scast<size_t>(size), u'\0');
		GetWindowTextW(handle_, reinterpret_cast<wchar_t *>(title.data()), scast<int32_t>(title.size()));
		return title;
	#else
		MFW_MESSAGE("TODO!!!")
		return {};
	#endif
	}

	window *window::get_window(handle_t wnd)
	{
	#if MFW_OS == MFW_OS_WINDOWS
		return reinterpret_cast<window *>(GetWindowLongPtrW(wnd, GWLP_USERDATA));
	#else
		MFW_MESSAGE("TODO!!!")
		return nullptr;
	#endif
	}

	const window *window::parent() const
	{
	#if MFW_OS == MFW_OS_WINDOWS
		handle_t wndpar{::GetParent(handle_)};
	#else
		handle_t wndpar{invalid_handle};
		MFW_MESSAGE("TODO!!!")
	#endif
		return get_window(wndpar);
	}

	void window::max_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		const window *parent_{parent()};
		if(!parent_) {
			return;
		}

		if(x) {
			*x = 0;
		}
		if(y) {
			*y = 0;
		}

		parent_->get_bounds(nullptr, nullptr, w, h);
	}

	void window::min_bounds(int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		if(x) {
			*x = 0;
		}
		if(y) {
			*y = 0;
		}
		if(w) {
			*w = 0;
		}
		if(h) {
			*h = 0;
		}
	}
}