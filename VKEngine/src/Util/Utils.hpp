#pragma once

namespace Util {
	template <typename T>
	class Singleton {
	protected:
		constexpr Singleton() noexcept = default;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		Singleton(const Singleton&&) = delete;
		Singleton&& operator=(const Singleton&&) = delete;

	public:
		static T* GetInstance() noexcept {
			static T inst{};
			return &inst;
		}
	};
}
