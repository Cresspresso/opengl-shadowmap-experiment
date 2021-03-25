
#pragma once

#include <memory>

namespace be
{
	struct default_constructed_need_t {};
	inline constexpr default_constructed_need_t const default_constructed_need = {};

	template<class T>
	class need
	{
	private:
		T m_value;
	public:
		using value_type = T;

		~need() noexcept = default;
		need() = delete;
		constexpr need(need&&) noexcept = default;
		constexpr need& operator=(need&&) noexcept = default;
		constexpr need(need const&) = default;
		constexpr need& operator=(need const&) = default;

		constexpr need(T&& value) noexcept : m_value(std::move(value)) {}
		constexpr need& operator=(T&& value) noexcept { m_value = std::move(value); return *this; }

		constexpr need(T const& value) : m_value(value) {}
		constexpr need& operator=(T const& value) { m_value = value; return *this; }

		constexpr need(default_constructed_need_t) : m_value() {}
		constexpr need& operator=(default_constructed_need_t) { m_value = T(); return *this; }


		constexpr T const& cget() const noexcept { return m_value; }
		constexpr T const& get() const noexcept { return m_value; }
		constexpr T& get() noexcept { return m_value; }

		constexpr operator T const& () const noexcept { return m_value; }
		constexpr operator T& () noexcept { return m_value; }
	};

	template<class T> using need_ref = std::reference_wrapper<T>;
}
