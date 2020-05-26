#include <private/mfw/renderer/helpers.hpp>

namespace mfw::renderer
{
	void set_rect_bounds(RECT &rect, int32_t x, int32_t y, int32_t w, int32_t h)
	{
		rect.left = x;
		rect.top = y;
		rect.bottom = (y + h);
		rect.right = (x + w);
	}

	void get_rect_bounds(const RECT &rect, int32_t *x, int32_t *y, int32_t *w, int32_t *h)
	{
		if(x) {
			*x = rect.left;
		}
		if(y) {
			*y = rect.top;
		}
		if(w) {
			*w = (rect.right - rect.left);
		}
		if(h) {
			*h = (rect.bottom - rect.top);
		}
	}
};