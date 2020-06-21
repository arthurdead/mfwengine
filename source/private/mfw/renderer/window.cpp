#include <private/mfw/renderer/window.hpp>
#include <public/mfw/stl/vector.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>

namespace mfw::renderer
{
	namespace __window_internal
	{
		static ptr_vector<window> windows{};
		using unique_monitors_t = unordered_map<monitor *, size_t>;
		static unique_monitors_t unique_monitors{};
	}

	bool window::initialize()
	{
		if(!interfaces::display_api_funcs::instance().init_windows()) {
			return false;
		}

		return true;
	}

	window::window(const monitor &mon, size_t w, size_t h, size_t x, size_t y)
		: mon_{&mon}
	{
		interfaces::display_api_funcs::instance().create_window(*this, mon, w, h, x, y);

		monitor *mon_ptr{&const_cast<monitor &>(mon)};

		__window_internal::unique_monitors_t::iterator it{__window_internal::unique_monitors.find(mon_ptr)};
		if(it == __window_internal::unique_monitors.end()) {
			it = __window_internal::unique_monitors.emplace(__window_internal::unique_monitors_t::value_type{mon_ptr, 0}).first;
		}
		it->second++;
	}

	window::~window()
	{
		interfaces::display_api_funcs::instance().destroy_window(*this);

		monitor *mon_ptr{const_cast<monitor *>(mon_)};

		__window_internal::unique_monitors_t::iterator it{__window_internal::unique_monitors.find(mon_ptr)};
		if(it != __window_internal::unique_monitors.end()) {
			it->second--;
			if(it->second == 0) {
				__window_internal::unique_monitors.erase(it);
			}
		}
	}

	void window::show(bool show_)
	{
		interfaces::display_api_funcs::instance().show_window(*this, show_);
	}

	void window::update()
	{
		for(__window_internal::unique_monitors_t::value_type &it : __window_internal::unique_monitors) {
			interfaces::display_api_funcs::instance().flush(*it.first);
		}
	}

	void window::shutdown()
	{
		__window_internal::windows.clear();
		__window_internal::unique_monitors.clear();
	}
}