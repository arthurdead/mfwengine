#include <private/mfw/renderer/window.hpp>
#include <public/mfw/stl/vector.hpp>
#include <private/mfw/renderer/display_api_funcs.hpp>
#include <private/mfw/renderer/render_api_funcs.hpp>

namespace mfw::renderer
{
	namespace __window_internal
	{
		static ptr_vector<window> windows{};
		using unique_monitors_t = unordered_map<monitor *, size_t>;
		static unique_monitors_t unique_monitors{};
		static vector<window *> maked_for_delete{};
	}

	bool window::initialize()
	{
		if(!interfaces::display_api_funcs::instance().initialize()) {
			return false;
		}

		return true;
	}

	window::window(const monitor &mon, const graphics_card &gpu, size_t w, size_t h, size_t x, size_t y)
		: mon_{&mon}, gpu_{&gpu}
	{
		interfaces::display_api_funcs::instance().create_window(*this, mon, w, h, x, y);
		interfaces::render_api_funcs::instance().create_window(*this, gpu, w, h);

		monitor *mon_ptr{&const_cast<monitor &>(mon)};

		__window_internal::unique_monitors_t::iterator it{__window_internal::unique_monitors.find(mon_ptr)};
		if(it == __window_internal::unique_monitors.end()) {
			it = __window_internal::unique_monitors.emplace(__window_internal::unique_monitors_t::value_type{mon_ptr, 0}).first;
		}
		it->second++;
	}

	window::~window()
	{
		on_closed();

		interfaces::render_api_funcs::instance().destroy_window(*this);
		interfaces::display_api_funcs::instance().destroy_window(*this);

		monitor *mon_ptr{const_cast<monitor *>(mon_)};

		__window_internal::unique_monitors_t::iterator it{__window_internal::unique_monitors.find(mon_ptr)};
		if(it != __window_internal::unique_monitors.end()) {
			it->second--;
			if(it->second == 0) {
				interfaces::display_api_funcs::instance().update(*(*it).first);
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
		__window_internal::unique_monitors_t::iterator it{__window_internal::unique_monitors.begin()};
		while(it != __window_internal::unique_monitors.end()) {
			interfaces::display_api_funcs::instance().update(*(*it).first);
			it++;
		}

		for(window *win : __window_internal::maked_for_delete) {
			delete win;
		}
		__window_internal::maked_for_delete.clear();
	}

	void window::shutdown()
	{
		__window_internal::windows.clear();
		__window_internal::unique_monitors.clear();
	}

	void window::on_internal_event(internal_event_type type, opaque_data *data)
	{
		switch(type) {
			case internal_event_type::destroyed:
			case internal_event_type::closed: {
				if(type == internal_event_type::destroyed) {
					destroyed_from = destroy_from::event;
				}
				__window_internal::maked_for_delete.emplace_back(this);
				break;
			}
			case internal_event_type::render: {
				on_render();
				break;
			}
		}
	}
}