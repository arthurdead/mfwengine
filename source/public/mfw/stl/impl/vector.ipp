#if MFW_STD_FLAGGED(API_CONFORMING)
namespace mfw::stl
{
	template <typename T, typename A>
	typename ptr_vector<T, A>::reference ptr_vector<T, A>::operator[](size_type pos)
	{
		unique_ptr<value_type> &ptr{super::operator[](pos)};
		return *ptr.get();
	}

	template <typename T, typename A>
	typename ptr_vector<T, A>::const_reference ptr_vector<T, A>::operator[](size_type pos) const
	{
		const unique_ptr<value_type> &ptr{super::operator[](pos)};
		return *ptr.get();
	}

	template <typename T, typename A>
	typename ptr_vector<T, A>::reference ptr_vector<T, A>::at(size_type pos)
	{
		unique_ptr<value_type> &ptr{super::at(pos)};
		return *ptr.get();
	}

	template <typename T, typename A>
	typename ptr_vector<T, A>::const_reference ptr_vector<T, A>::at(size_type pos) const
	{
		const unique_ptr<value_type> &ptr{super::at(pos)};
		return *ptr.get();
	}
}
#else
	#error
#endif