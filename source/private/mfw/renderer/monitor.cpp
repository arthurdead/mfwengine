#include <private/mfw/renderer/monitor.hpp>

namespace mfw::renderer
{
	void monitor::bounds(bool work, int32_t *x, int32_t *y, int32_t *w, int32_t *h) const
	{
		const RECT &rect_{rect(work)};

		get_rect_bounds(rect_, x, y, w, h);
	}
};