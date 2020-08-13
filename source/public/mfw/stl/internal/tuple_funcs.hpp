#include <public/mfw/stl/vector.hpp>

namespace mfw::stl
{
	template <typename _Tp, typename... _Args>
	MFW_VISIBILITY_LOCAL bool to_vector(const tuple<_Args...> &tup, vector<_Tp> &vec) noexcept;
}

#include <public/mfw/stl/impl/tuple_funcs.tpp>