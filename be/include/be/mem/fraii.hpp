/*
//	be/mem/fraii
//	RAII wrapper for non-pointer types.
//	Similar to `std::unique_ptr<T, Deleter>`.
//
//	Elijah Shadbolt
//	2021
*/

#pragma once

#include <memory>

namespace be
{
	namespace mem
	{
		struct NullFraii {};
		constexpr NullFraii nullFraii{};

		/*
		//	RAII wrapper for a uniquely owned non-pointer resource handle.
		//	Similar to `std::unique_ptr<T, Deleter>`.
		//
		//	# Type Parameters
		//
		//	* T -
		//		Value type of the resource handle.
		//		Must have default constructor representing null.
		//		
		//	* Deleter -
		//		Callable of the form `void operator()(T resource) noexcept`.
		//		Note that `resource == T()` is a possibility (so you must check for null in the callable body).
		//		The Deleter type must also have a noexcept copy constructor and a noexcept copy assignment operator.
		//
		//	# Remarks
		//
		//	Equality and comparisons only consider the resource handle. If the deleters are different, they may still be `==`.
		//
		//
		//	TODO - use zero-sized base class optimisation for when Deleter has no fields
		//
		*/
		template<class T, class Deleter>
		class Fraii {
		private:
			Deleter m_deleter;
			T m_resource;

		public:
			~Fraii() noexcept {
				m_deleter(std::move(m_resource));
			}
			constexpr Fraii(Fraii&& b) noexcept
				: m_deleter(b.m_deleter)
				, m_resource(b.release())
			{}
			constexpr Fraii& operator=(Fraii&& b) noexcept {
				m_deleter(std::move(m_resource));
				m_deleter = b.m_deleter;
				m_resource = b.release();
				return *this;
			}
			constexpr Fraii(Fraii const& b) = delete;
			constexpr Fraii& operator=(Fraii const& b) = delete;
			constexpr void swap(Fraii& b) noexcept {
				using std::swap;
				swap(m_deleter, b.m_deleter);
				swap(m_resource, b.m_resource);
			}
			constexpr Fraii()
				: m_deleter()
				, m_resource()
			{}
			constexpr Fraii(NullFraii) : Fraii() {}
			constexpr explicit Fraii(NullFraii, Deleter&& deleter)
				: m_deleter(std::move(deleter))
				, m_resource()
			{}
			constexpr explicit Fraii(NullFraii, Deleter const& deleter)
				: m_deleter(deleter)
				, m_resource()
			{}
			constexpr explicit Fraii(T&& resource, Deleter&& deleter) noexcept
				: m_deleter(std::move(deleter))
				, m_resource(std::move(resource))
			{}
			constexpr explicit Fraii(T const& resource, Deleter&& deleter)
				: m_deleter(std::move(deleter))
				, m_resource(resource)
			{}
			constexpr explicit Fraii(T&& resource, Deleter const& deleter)
				: m_deleter(deleter)
				, m_resource(std::move(resource))
			{}
			constexpr explicit Fraii(T const& resource, Deleter const& deleter)
				: m_deleter(deleter)
				, m_resource(resource)
			{}
			constexpr explicit Fraii(T&& resource)
				: m_deleter()
				, m_resource(std::move(resource))
			{}
			constexpr explicit Fraii(T const& resource)
				: m_deleter()
				, m_resource(resource)
			{}
			constexpr Fraii& operator=(NullFraii) {
				m_deleter(release());
				return *this;
			}

			constexpr T release() {
				return std::exchange(m_resource, T());
			}

			constexpr T const& get() const noexcept {
				return m_resource;
			}
		};

		template<class T, class D>
		inline constexpr void swap(Fraii<T, D>& a, Fraii<T, D>& b) noexcept {
			a.swap(b);
		}

		template<class T, class Deleter>
		constexpr Fraii<std::decay_t<T>, std::decay_t<Deleter>>
			make_fraii(T&& resource, Deleter&& deleter)
		{
			return Fraii<std::decay_t<T>, std::decay_t<Deleter>>(
				std::forward<T>(resource),
				std::forward<Deleter>(deleter)
				);
		}

		template<class T, class Deleter>
		constexpr Fraii<T, std::decay_t<Deleter>>
			make_fraii(NullFraii nullFraii, Deleter&& deleter)
		{
			return Fraii<T, std::decay_t<Deleter>>(
				nullFraii,
				std::forward<Deleter>(deleter)
				);
		}

		template<class T, class D>
		inline constexpr bool operator==(Fraii<T, D> const& a, Fraii<T, D> const& b) noexcept {
			return a.get() == b.get();
		}
		template<class T, class D>
		inline constexpr bool operator!=(Fraii<T, D> const& a, Fraii<T, D> const& b) noexcept {
			return a.get() != b.get();
		}

		template<class T, class D>
		inline constexpr bool operator==(Fraii<T, D> const& v, NullFraii) noexcept {
			return v.get() == T();
		}
		template<class T, class D>
		inline constexpr bool operator!=(Fraii<T, D> const& v, NullFraii) noexcept {
			return v.get() != T();
		}

		template<class T, class D>
		inline constexpr bool operator==(NullFraii, Fraii<T, D> const& v) noexcept {
			return T() == v.get();
		}
		template<class T, class D>
		inline constexpr bool operator!=(NullFraii, Fraii<T, D> const& v) noexcept {
			return T() != v.get();
		}

		template<class T, class D>
		inline constexpr bool operator<(Fraii<T, D> const& a, Fraii<T, D> const& b) noexcept {
			return a.get() < b.get();
		}
		template<class T, class D>
		inline constexpr bool operator>(Fraii<T, D> const& a, Fraii<T, D> const& b) noexcept {
			return a.get() > b.get();
		}
		template<class T, class D>
		inline constexpr bool operator<=(Fraii<T, D> const& a, Fraii<T, D> const& b) noexcept {
			return a.get() <= b.get();
		}
		template<class T, class D>
		inline constexpr bool operator>=(Fraii<T, D> const& a, Fraii<T, D> const& b) noexcept {
			return a.get() >= b.get();
		}
	}
	//~ namespace
}
