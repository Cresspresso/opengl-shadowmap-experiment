/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/grid.hpp
//
//	Summary		:	Multi-dimensional dynamically allocated array/matrix.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	01/11/2019
*/

#pragma once

#include <array>
#include <vector>

#ifndef CRESS_MOO_NO_CASSERT
#include <cassert>
#endif



namespace cress
{
	namespace moo
	{
		/*
		//	Thrown when a Grid receives an out-of-range indexing argument.
		*/
		struct OutOfRangeGridIndexerException : public std::exception
		{
			char const* what() const noexcept override
			{
				return "cress::moo::OutOfRangeGridIndexerException";
			}
		};



		struct DefaultGridRequireTraits
		{
			/*
			//	This is called in the event of out of range coordinates or linear index that does not throw an exception.
			//	It must be considered noexcept.
			//	It can terminate or return normally.
			*/
			[[noreturn]] static void onBadAccess() noexcept
			{
#ifndef CRESS_MOO_NO_CASSERT
				assert(false);
#endif
				std::terminate();
			}

			/*
			//	This is called in the event of out of range coordinates or linear index.
			//	It must be noreturn.
			//	It can throw an exception or terminate.
			*/
			[[noreturn]] static void onBadAccessThrow() noexcept(false)
			{
				throw OutOfRangeGridIndexerException();
			}
		};



		template<size_t numDimensions_>
		struct DimensionsUtils
		{
			using dimensions_type = std::array<size_t, numDimensions_>;

			[[nodiscard]] static constexpr size_t numDimensions() noexcept { return numDimensions_; }

			[[nodiscard]] static constexpr size_t calcSize(dimensions_type const& dimensions) noexcept
			{
				size_t total = 1U;
				for (size_t const dimension : dimensions)
				{
					total *= dimension;
				}
				return total;
			}

			[[nodiscard]] static constexpr size_t calcLinearIndex(
				dimensions_type const& dimensions,
				dimensions_type const& coordinates
			) noexcept
			{
				size_t linearIndex = coordinates[0];
				for (size_t dimension = 1;
					dimension < dimensions.size();
					++dimension)
				{
					size_t localOffset = coordinates[dimension];
					for (size_t i = 0; i < dimension; ++i)
					{
						localOffset *= dimensions[i];
					}
					linearIndex += localOffset;
				}
				return linearIndex;
				/// i0
				/// i0 + i1 * n0
				/// i0 + i1 * n0 + i2 * n0 * n1
				/// i0 + i1 * n0 + i2 * n0 * n1 + i3 * n0 * n1 * n2
			}

			[[nodiscard]] static constexpr bool isInRange(
				dimensions_type const& dimensions,
				dimensions_type const& coordinates
			) noexcept
			{
				for (size_t dimension = 0; dimension < numDimensions_; ++dimension)
				{
					size_t const ordinate = coordinates[dimension];
					if (ordinate >= dimensions[dimension] /* || ordinate < 0 */)
					{
						return false;
					}
				}
				return true;
			}
		};

		template<class...I>
		[[nodiscard]]
		constexpr std::enable_if_t<(std::is_convertible_v<I, size_t> && ...),
		std::array<size_t, sizeof...(I)>>
			makeDimensions(I const&...i)
		{
			return { static_cast<size_t>(i)... };
		}



		namespace detail
		{
			struct InternalGicTag {};
		}

		/*
		//	IndexedCoordinates
		//	Multi-dimensional coordinates with an associated linear index.
		//	Can be used as a key in a map or set, because comparison is done against the linear index.
		*/
		template<size_t numDimensions_>
		class IndexedCoordinates
		{
		private:
			using Utils = DimensionsUtils<numDimensions_>;

		public:
			[[nodiscard]] static constexpr size_t numDimensions() noexcept { return numDimensions_; }

			using dimensions_type = typename Utils::dimensions_type;

		private:
			size_t m_linearIndex = 0;
			dimensions_type m_coordinates = dimensions_type();

		public:
			IndexedCoordinates() = default;
			IndexedCoordinates(IndexedCoordinates const&) = default;
			IndexedCoordinates& operator=(IndexedCoordinates const&) = default;
			IndexedCoordinates(IndexedCoordinates&&) = default;
			IndexedCoordinates& operator=(IndexedCoordinates&&) = default;

			[[nodiscard]] size_t linearIndex() const noexcept { return m_linearIndex; }
			[[nodiscard]] dimensions_type const& coordinates() const noexcept { return m_coordinates; }

			/*
			//	For internal use.
			*/
			explicit IndexedCoordinates(
				detail::InternalGicTag,
				size_t const linearIndex,
				dimensions_type const& coordinates
			)
				: m_linearIndex(linearIndex)
				, m_coordinates(coordinates)
			{}
		};

		template<size_t numDimensions_>
		[[nodiscard]] bool operator<(IndexedCoordinates<numDimensions_> const& a, IndexedCoordinates<numDimensions_> const& b) noexcept
		{
			return a.linearIndex() < b.linearIndex();
		}

		template<size_t numDimensions_>
		[[nodiscard]] bool operator>(IndexedCoordinates<numDimensions_> const& a, IndexedCoordinates<numDimensions_> const& b) noexcept
		{
			return a.linearIndex() > b.linearIndex();
		}

		template<size_t numDimensions_>
		[[nodiscard]] bool operator<=(IndexedCoordinates<numDimensions_> const& a, IndexedCoordinates<numDimensions_> const& b) noexcept
		{
			return a.linearIndex() <= b.linearIndex();
		}

		template<size_t numDimensions_>
		[[nodiscard]] bool operator>=(IndexedCoordinates<numDimensions_> const& a, IndexedCoordinates<numDimensions_> const& b) noexcept
		{
			return a.linearIndex() >= b.linearIndex();
		}

		template<size_t numDimensions_>
		[[nodiscard]] bool operator==(IndexedCoordinates<numDimensions_> const& a, IndexedCoordinates<numDimensions_> const& b) noexcept
		{
			return a.linearIndex() == b.linearIndex();
		}

		template<size_t numDimensions_>
		bool operator!=(IndexedCoordinates<numDimensions_> const& a, IndexedCoordinates<numDimensions_> const& b)
		{
			return a.linearIndex() != b.linearIndex();
		}



		/*
		//	Grid
		//	Multi-dimensional dynamically allocated array/matrix.
		//
		//	The |Require_traits_| type parameter must have the following members:
		//
		//		static void onBadAccess() noexcept;
		//			This is called in the event of an invalid dereference that does not throw an exception.
		//			It must be considered noexcept.
		//			It can terminate or return normally.
		//
		//		[[noreturn]] static void onBadUnwrap(bool isLeft) noexcept(false);
		//			This is called in the event of an invalid unwrap.
		//			It must be noreturn.
		//			It can throw an exception or terminate.
		//
		//	Example:

			cress::moo::Grid2<std::string> grid({ 3, 2 },
			{
				"0,0", "1,0", "2,0",
				"0,1", "1,1", "2,1",
			});
			assert(grid.at({ 0, 0 }) == "0,0");
			assert(grid.at({ 2, 1 }) == "2,1");
			assert(!grid.isInRange({ 3, 0 }));

		*/
		template<size_t numDimensions_,
			class Value_type_,
			class Alloc_ = std::allocator<Value_type_>,
			class Require_traits_ = DefaultGridRequireTraits
		>
		class Grid
		{
		private:
			using Utils = DimensionsUtils<numDimensions_>;

		public:
			static constexpr size_t s_numDimensions() noexcept { return numDimensions_; }
			constexpr size_t numDimensions() noexcept { return numDimensions_; }

			using This = Grid;
			using require_traits = Require_traits_;
			using value_type = Value_type_;

			using dimensions_type = typename Utils::dimensions_type;
			using indexed_coordinates_type = IndexedCoordinates<numDimensions_>;

			using linear_type = std::vector<Value_type_, Alloc_>;
			using size_type = typename linear_type::size_type;
			using iterator = typename linear_type::iterator;
			using const_iterator = typename linear_type::const_iterator;



		private:

			linear_type m_linearArray;
			dimensions_type m_dimensions = dimensions_type();



		public:

			~Grid() noexcept = default;
			Grid() = default;
			Grid(Grid const&) = default;
			Grid& operator=(Grid const&) = default;

			template<class RequireTraits2>
			Grid(Grid<numDimensions_, Value_type_, Alloc_, RequireTraits2>&& that) noexcept
				: m_linearArray(std::move(that.m_linearArray))
				, m_dimensions(that.m_dimensions)
			{
				that.m_dimensions.fill(0U);
			}

			template<class RequireTraits2>
			Grid& operator=(Grid<numDimensions_, Value_type_, Alloc_, RequireTraits2>&& that) noexcept
			{
				m_linearArray = std::move(that.m_linearArray);
				m_dimensions = that.m_dimensions;
				if (that.m_linearArray.empty())
				{
					that.m_dimensions.fill(0U);
				}
				return *this;
			}

			void swap(Grid& that) noexcept
			{
				m_linearArray.swap(that.m_linearArray);
				m_dimensions.swap(that.m_dimensions);
			}

			friend void swap(Grid& a, Grid& b) noexcept { a.swap(b); }

			explicit Grid(Alloc_ const& al) noexcept : m_linearArray(al) {}

			Grid(dimensions_type const& dimensions, Alloc_ const& al = Alloc_())
				: m_linearArray(Utils::calcSize(dimensions), al)
				, m_dimensions(dimensions)
			{
				// if at least one dimension is zero
				if (m_linearArray.empty())
				{
					m_dimensions.fill(0);
				}
			}

			Grid(dimensions_type const& dimensions, Value_type_ const& value, Alloc_ const& al = Alloc_())
				: m_linearArray(Utils::calcSize(dimensions), value, al)
				, m_dimensions(dimensions)
			{
				// if at least one dimension is zero
				if (m_linearArray.empty())
				{
					m_dimensions.fill(0);
				}
			}

			Grid(
				dimensions_type const& dimensions,
				std::initializer_list<Value_type_> list,
				Alloc_ const& al = Alloc_()
			)
				: m_linearArray(list, al)
				, m_dimensions(dimensions)
			{
				// ensure size is correct.
				m_linearArray.resize(Utils::calcSize(dimensions));

				// if at least one dimension is zero
				if (m_linearArray.empty())
				{
					m_dimensions.fill(0);
				}
			}

			template<class Iter_>
			Grid(
				dimensions_type const& dimensions,
				Iter_ first,
				Iter_ last,
				Alloc_ const& al = Alloc_()
			)
				: m_linearArray(first, last, al)
				, m_dimensions(dimensions)
			{
				// ensure size is correct.
				m_linearArray.resize(Utils::calcSize(dimensions));

				// if at least one dimension is zero
				if (m_linearArray.empty())
				{
					m_dimensions.fill(0);
				}
			}

			[[nodiscard]] bool empty() const noexcept { return m_linearArray.empty(); }
			[[nodiscard]] size_type size() const noexcept { return m_linearArray.size(); }
			[[nodiscard]] dimensions_type const& dimensions() const noexcept { return m_dimensions; }

			[[nodiscard]] auto* data() noexcept { return m_linearArray.data(); }
			[[nodiscard]] auto const* data() const noexcept { return m_linearArray.data(); }

			[[nodiscard]] auto cbegin() const noexcept { return m_linearArray.cbegin(); }
			[[nodiscard]] auto cend() const noexcept { return m_linearArray.cend(); }
			[[nodiscard]] auto begin() const noexcept { return m_linearArray.begin(); }
			[[nodiscard]] auto end() const noexcept { return m_linearArray.end(); }
			[[nodiscard]] auto begin() noexcept { return m_linearArray.begin(); }
			[[nodiscard]] auto end() noexcept { return m_linearArray.end(); }

			[[nodiscard]] auto crbegin() const noexcept { return m_linearArray.crbegin(); }
			[[nodiscard]] auto crend() const noexcept { return m_linearArray.crend(); }
			[[nodiscard]] auto rbegin() const noexcept { return m_linearArray.rbegin(); }
			[[nodiscard]] auto rend() const noexcept { return m_linearArray.rend(); }
			[[nodiscard]] auto rbegin() noexcept { return m_linearArray.rbegin(); }
			[[nodiscard]] auto rend() noexcept { return m_linearArray.rend(); }

			/*
			//	Returns true if linear index is valid.
			*/
			[[nodiscard]] bool isInRange(size_type const linearIndex) const noexcept
			{
				return linearIndex < size() /* && linearIndex >= 0 */;
			}

		private:
			void requireAccess(bool condition) const noexcept
			{
				if (!condition) { Require_traits_::onBadAccess(); }
			}

			void requireAccessElseThrow(bool condition) const noexcept(false)
			{
				if (!condition) { Require_traits_::onBadAccessThrow(); }
			}

		public:
			[[nodiscard]] decltype(auto) operator[](size_type const linearIndex) noexcept
			{
				requireAccess(isInRange(linearIndex));
				return m_linearArray[linearIndex];
			}

			[[nodiscard]] decltype(auto) operator[](size_type const linearIndex) const noexcept
			{
				requireAccess(isInRange(linearIndex));
				return m_linearArray[linearIndex];
			}

			[[nodiscard]] decltype(auto) at(size_type const linearIndex) noexcept(false)
			{
				requireAccessElseThrow(isInRange(linearIndex));
				return m_linearArray[linearIndex];
			}

			[[nodiscard]] decltype(auto) at(size_type const linearIndex) const noexcept(false)
			{
				requireAccessElseThrow(isInRange(linearIndex));
				return m_linearArray[linearIndex];
			}

			/*
			//	Returns true if coordinates are valid.
			*/
			[[nodiscard]] bool isInRange(dimensions_type const& coordinates) const noexcept
			{
				return Utils::isInRange(m_dimensions, coordinates);
			}

			/*
			//	Given coordinates, returns the index into the underlying single-dimensional array.
			//	The returned value is invalidated if this grid's dimensions are changed.
			*/
			[[nodiscard]] size_type linearIndexUnchecked(dimensions_type const& coordinates) const noexcept
			{
				requireAccess(isInRange(coordinates));
				return Utils::calcLinearIndex(m_dimensions, coordinates);
			}

			/*
			//	Given coordinates, returns the index into the underlying single-dimensional array.
			//	The returned value is invalidated if this grid's dimensions are changed.
			*/
			[[nodiscard]] size_type linearIndex(dimensions_type const& coordinates) const noexcept(false)
			{
				requireAccessElseThrow(isInRange(coordinates));
				return Utils::calcLinearIndex(m_dimensions, coordinates);
			}

			[[nodiscard]] decltype(auto) operator[](dimensions_type const& coordinates) noexcept
			{
				return operator[](linearIndexUnchecked(coordinates));
			}

			[[nodiscard]] decltype(auto) operator[](dimensions_type const& coordinates) const noexcept
			{
				return operator[](linearIndexUnchecked(coordinates));
			}

			[[nodiscard]] decltype(auto) at(dimensions_type const& coordinates) noexcept(false)
			{
				requireAccessElseThrow(isInRange(coordinates));
				return operator[](coordinates);
			}

			[[nodiscard]] decltype(auto) at(dimensions_type const& coordinates) const noexcept(false)
			{
				requireAccessElseThrow(isInRange(coordinates));
				return operator[](coordinates);
			}

			/*
			//	Given coordinates, returns something that can be used as a value in a set.
			//	The returned value is invalidated if this grid's dimensions are changed.
			*/
			[[nodiscard]] indexed_coordinates_type
				indexCoordinates(dimensions_type const& coordinates) const noexcept(false)
			{
				requireAccessElseThrow(isInRange(coordinates));
				return indexed_coordinates_type(detail::InternalGicTag(),
					linearIndexUnchecked(coordinates), coordinates);
			}

			/*
			//	Returns true if coordinates and linear index are valid.
			*/
			[[nodiscard]] bool isValid(indexed_coordinates_type const& ic) const noexcept
			{
				return isInRange(ic.coordinates())
					&& ic.linearIndex() == linearIndexUnchecked(ic.coordinates());
			}

			/*
			//	Precondition: The indexed coordinates must be in range for this grid,
			//		and this grid's dimensions are the same as the dimensions used when the
			//		indexed coordinates were constructed.
			*/
			[[nodiscard]] decltype(auto) operator[](indexed_coordinates_type const& ic) noexcept
			{
				requireAccess(isValid(ic));
				return m_linearArray[ic.linearIndex()];
			}

			/*
			//	Precondition: The indexed coordinates must be in range for this grid,
			//		and this grid's dimensions are the same as the dimensions used when the
			//		indexed coordinates were constructed.
			*/
			[[nodiscard]] decltype(auto) operator[](indexed_coordinates_type const& ic) const noexcept
			{
				requireAccess(isValid(ic));
				return m_linearArray[ic.linearIndex()];
			}

			[[nodiscard]] decltype(auto) at(indexed_coordinates_type const& ic) noexcept(false)
			{
				requireAccessElseThrow(isValid(ic));
				return m_linearArray[ic.linearIndex()];
			}

			[[nodiscard]] decltype(auto) at(indexed_coordinates_type const& ic) const noexcept(false)
			{
				requireAccessElseThrow(isValid(ic));
				return m_linearArray[ic.linearIndex()];
			}
		};

		/*
		//	Two-dimensional dynamically allocated array/matrix.
		*/
		template<class Value_type_, class Alloc_ = std::allocator<Value_type_>, class Require_traits_ = DefaultGridRequireTraits>
		using Grid2 = Grid<2U, Value_type_, Alloc_, Require_traits_>;

		/*
		//	Three-dimensional dynamically allocated array/matrix.
		*/
		template<class Value_type_, class Alloc_ = std::allocator<Value_type_>, class Require_traits_ = DefaultGridRequireTraits>
		using Grid3 = Grid<3U, Value_type_, Alloc_, Require_traits_>;
	}
}
