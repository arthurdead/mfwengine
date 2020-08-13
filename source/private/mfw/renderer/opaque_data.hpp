#ifndef MFW_PRIVATE_RENDERER_OPAQUE_DATA_HPP
#define MFW_PRIVATE_RENDERER_OPAQUE_DATA_HPP

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
			T &get_or_allocate_##name##_data() { return name##_data.get_or_allocate<T>(); } \
			void destroy_##name##_data() { name##_data.destroy(); } \
		private: \
			opaque_data name##_data{};

	class opaque_data
	{
	public:
		template <typename T>
		T &get() {
		#if MFW_CONFIGURATION_IS(DEBUG)
			if(!actual_data) {
				MFW_DEBUGBREAK();
			}
		#endif
			return *reinterpret_cast<T *>(actual_data.get());
		}

		template <typename T>
		const T &get() const {
		#if MFW_CONFIGURATION_IS(DEBUG)
			if(!actual_data) {
				MFW_DEBUGBREAK();
			}
		#endif
			return *reinterpret_cast<const T *>(actual_data.get());
		}

		template <typename T>
		T &allocate() {
		#if MFW_CONFIGURATION_IS(DEBUG)
			if(actual_data) {
				MFW_DEBUGBREAK();
			}
		#endif
			actual_data.reset(new byte[sizeof(T)]{});
			T &data{get<T>()};
			new (&data) T{};
			destroy_func = [this]() -> void {
				T &data{get<T>()};
				data.T::~T();
				actual_data.reset();
			};
			return data;
		}

		template <typename T>
		T &get_or_allocate() {
			if(actual_data) {
				return get<T>();
			} else {
				return allocate<T>();
			}
		}

		void destroy() {
			if(destroy_func) {
				destroy_func();
			}
		}

		~opaque_data() {
			destroy();
		}

	private:
		unique_ptr<byte[]> actual_data{};
		function<void()> destroy_func{};
	};
}

#endif