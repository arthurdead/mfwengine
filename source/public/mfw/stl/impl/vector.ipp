namespace mfw::stl
{
	template <typename _Tp, typename _Alloc>
	typename ptr_vector<_Tp, _Alloc>::reference ptr_vector<_Tp, _Alloc>::operator[](size_type __pos) noexcept
	{ return *super::operator[](__pos).get(); }

	template <typename _Tp, typename _Alloc>
	typename ptr_vector<_Tp, _Alloc>::const_reference ptr_vector<_Tp, _Alloc>::operator[](size_type __pos) const noexcept
	{ return *super::operator[](__pos).get(); }

	template <typename _Tp, typename _Alloc>
	typename ptr_vector<_Tp, _Alloc>::reference ptr_vector<_Tp, _Alloc>::at(size_type __pos) noexcept
	{ return *super::at(__pos).get(); }

	template <typename _Tp, typename _Alloc>
	typename ptr_vector<_Tp, _Alloc>::const_reference ptr_vector<_Tp, _Alloc>::at(size_type __pos) const noexcept
	{ return *super::at(__pos).get(); }
}