#ifndef __MFW_PRIVATE_RENDERER_OPAQUE_DATA_H
#define __MFW_PRIVATE_RENDERER_OPAQUE_DATA_H

#pragma once

namespace mfw::renderer
{
	MFW_MESSAGE("TODO move this somewhere else")

	#define __MFW_RENDERER_OPAQUE_DATA(name) \
		public: \
			template <typename T> \
			T &get_##name##_data() { return name##_data.get<T>(); } \
			template <typename T> \
			const T &get_##name##_data() const { return name##_data.get<T>(); } \
			template <typename T> \
			T &allocate_##name##_data() { return name##_data.allocate<T>(); } \
			template <typename T> \
			void destroy_##name##_data() { name##_data.destroy<T>(); } \
		private: \
			opaque_data name##_data{};

	class opaque_data
	{
	public:
		template <typename T>
		T &get() {
			return *reinterpret_cast<T *>(actual_data.get());
		}

		template <typename T>
		const T &get() const {
			return *reinterpret_cast<const T *>(actual_data.get());
		}

		template <typename T>
		T &allocate() {
			actual_data.reset(new byte[sizeof(T)]{});
			T &data{get<T>()};
			new (&data) T{};
			return data;
		}

		template <typename T>
		void destroy() {
			T &data{get<T>()};
			data.T::~T();
			actual_data.reset();
		}

	private:
		unique_ptr<byte[]> actual_data{};
	};
}

#endif