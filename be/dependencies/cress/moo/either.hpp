/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/either.hpp
//
//	Summary		:	Either a left value or a right value.
//					Useful when returning result|error.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	23/10/2019
*/

#pragma once

#include <type_traits>
#include <exception>

#ifndef CRESS_MOO_NO_CASSERT
#include <cassert>
#endif



namespace cress
{
	namespace moo
	{
		/*
		//	Thrown when a Either is unwrapped while in the other state.
		*/
		struct BadEitherException : public std::exception
		{
			char const* what() const override
			{
				return "cress::moo::BadEitherException";
			}
		};



		struct DefaultEitherRequireTraits
		{
			/*
			//	This is called in the event of an invalid dereference that does not throw an exception.
			//	It must be considered noexcept.
			//	It can terminate or return normally.
			*/
			[[noreturn]] static void onBadAccess(bool isLeft) noexcept
			{
#ifndef CRESS_MOO_NO_CASSERT
				assert(false);
#endif
				std::terminate();
			}

			/*
			//	This is called in the event of an invalid unwrap.
			//	It must be noreturn.
			//	It can throw an exception or terminate.
			*/
			[[noreturn]] static void onBadUnwrap(bool isLeft) noexcept(false)
			{
				throw BadEitherException();
			}
		};



		namespace EitherTags
		{
			struct EmplaceLeft {};
			constexpr EmplaceLeft const emplaceLeft{};

			struct EmplaceRight {};
			constexpr EmplaceRight const emplaceRight{};
		}



		/*
		//	Either
		//
		//	Either a left value or a right value.
		//	Useful when returning result|error.
		//
		//	The |RequireTraits| type parameter must have the following members:
		//
		//		static void onBadAccess(bool isLeft) noexcept;
		//			This is called in the event of an invalid dereference that does not throw an exception.
		//			It must be considered noexcept.
		//			It can terminate or return normally.
		//
		//		[[noreturn]] static void onBadUnwrap(bool isLeft) noexcept(false);
		//			This is called in the event of an invalid unwrap.
		//			It must be noreturn.
		//			It can throw an exception or terminate.
		//
		//
		//	Example:

			cress::moo::Either<int, std::string> tryDivide(int n, int d)
			{
				if (0 == d) { return "divide by zero error"; }
				return n / d;
			}

			int main()
			{
				auto result = tryDivide(8, 4);
				assert(result.hasLeft());
				assert(result.accessLeft() == 2);

				result = tryDivide(3, 0);
				assert(result.hasRight());
				assert(result.accessRight() == std::string("divide by zero error"));

				int x = tryDivide(4, 3).joinLeft([]{ return 13; });
				assert(x == 1);

				x = tryDivide(4, 0).joinLeft([]{ return 13; });
				assert(x == 13);
			}

		*/
		template<class Left_, class Right_, class RequireTraits_ = DefaultEitherRequireTraits>
		class [[nodiscard]] Either
		{
		public:
			using This = Either;
			using Left = Left_;
			using Right = Right_;
			using RequireTraits = RequireTraits_;
		private:
			bool m_hasLeft;

			struct UninitalisedStorageTag {};

			union Storage {
				UninitalisedStorageTag u_uninitialised;
				Left_ u_left;
				Right_ u_right;

				~Storage() noexcept {}

				explicit Storage(UninitalisedStorageTag) noexcept : u_uninitialised() {}

				template<class...A>
				explicit Storage(EitherTags::EmplaceLeft, A&& ...a)
					noexcept(std::is_nothrow_constructible_v<Left_, A&&...>)
					: u_left(std::forward<A>(a)...) {}

				template<class...A>
				explicit Storage(EitherTags::EmplaceRight, A&& ...a)
					noexcept(std::is_nothrow_constructible_v<Right_, A&&...>)
					: u_right(std::forward<A>(a)...) {}

			} m_storage;



			template<class R>
			using EnableOverloadRight_t = std::enable_if_t<std::is_convertible_v<R, Right_> && !std::is_same_v<Right_, Left_>>;

			void checkAccessLeft() const noexcept
			{
				if (!m_hasLeft) { RequireTraits_::onBadAccess(true); }
			}

			void checkAccessRight() const noexcept
			{
				if (m_hasLeft) { RequireTraits_::onBadAccess(false); }
			}

			void requireUnwrapLeft() const noexcept(false)
			{
				if (!m_hasLeft) { RequireTraits_::onBadUnwrap(true); }
			}

			void requireUnwrapRight() const noexcept(false)
			{
				if (m_hasLeft) { RequireTraits_::onBadUnwrap(false); }
			}

		public:
			~Either() noexcept
			{
				if (m_hasLeft)
				{
					m_storage.u_left.Left_::~Left_();
				}
				else
				{
					m_storage.u_right.Right_::~Right_();
				}
			}

			Either(Either&& that) noexcept
				: m_hasLeft(that.m_hasLeft), m_storage(UninitalisedStorageTag{})
			{
				if (that.m_hasLeft)
				{
					new (&m_storage.u_left) Left_(std::move(that.m_storage.u_left));
				}
				else
				{
					new (&m_storage.u_right) Right_(std::move(that.m_storage.u_right));
				}
			}

			Either& operator=(Either&& that) noexcept
			{
				if (m_hasLeft)
				{
					if (that.m_hasLeft)
					{
						m_storage.u_left = std::move(that.m_storage.u_left);
					}
					else
					{
						m_storage.u_left.Left_::~Left_();
						new (&m_storage.u_right) Right_(std::move(that.m_storage.u_right));
					}
				}
				else
				{
					if (that.m_hasLeft)
					{
						m_storage.u_right.Right_::~Right_();
						new (&m_storage.u_left) Left_(std::move(that.m_storage.u_left));
					}
					else
					{
						m_storage.u_right = std::move(that.m_storage.u_right);
					}
				}
				m_hasLeft = that.m_hasLeft;
				return *this;
			}

			void swap(Either& that) noexcept
			{
				using std::swap;
				if (m_hasLeft)
				{
					if (that.m_hasLeft)
					{
						swap(m_storage.u_left, that.m_storage.u_left);
					}
					else
					{
						Left_ temp = std::move(m_storage.u_left);
						m_storage.u_left.Left_::~Left_();
						new (&m_storage.u_right) Right_(std::move(that.m_storage.u_right));
						that.m_storage.u_right.Right_::~Right_();
						new (&that.m_storage.u_left) Left_(std::move(temp));
					}
				}
				else
				{
					if (that.m_hasLeft)
					{
						Left_ temp = std::move(that.m_storage.u_left);
						that.m_storage.u_left.Left_::~Left_();
						new (&that.m_storage.u_right) Right_(std::move(m_storage.u_right));
						m_storage.u_right.Right_::~Right_();
						new (&m_storage.u_left) Left_(std::move(temp));
					}
					else
					{
						swap(m_storage.u_right, that.m_storage.u_right);
					}
				}
				swap(m_hasLeft, that.m_hasLeft);
			}

			Either(Either const& that)
				noexcept(std::is_nothrow_copy_constructible_v<Left_>
					&& std::is_nothrow_copy_constructible_v<Right_>)
				: m_hasLeft(that.m_hasLeft), m_storage(UninitalisedStorageTag{})
			{
				if (that.m_hasLeft)
				{
					new (&m_storage.u_left) Left_(that.m_storage.u_left);
				}
				else
				{
					new (&m_storage.u_right) Right_(that.m_storage.u_right);
				}
			}

			Either& operator=(Either const& that)
				noexcept(std::is_nothrow_copy_constructible_v<Left_>
					&& std::is_nothrow_copy_assignable_v<Left_>
					&& std::is_nothrow_copy_constructible_v<Right_>
					&& std::is_nothrow_copy_assignable_v<Right_>)
			{
				if (m_hasLeft)
				{
					if (that.m_hasLeft)
					{
						m_storage.u_left = that.m_storage.u_left;
					}
					else
					{
						m_storage.u_left.Left_::~Left_();
						new (&m_storage.u_right) Right_(that.m_storage.u_right);
					}
				}
				else
				{
					if (that.m_hasLeft)
					{
						m_storage.u_right.Right_::~Right_();
						new (&m_storage.u_left) Left_(that.m_storage.u_left);
					}
					else
					{
						m_storage.u_right = that.m_storage.u_right;
					}
				}
				m_hasLeft = that.m_hasLeft;
				return *this;
			}

			//Either() = delete;
			Either() noexcept(std::is_nothrow_default_constructible_v<Right_>)
				: m_hasLeft(false), m_storage(EitherTags::emplaceRight) {}

			Either(Left_ const& left) noexcept(std::is_nothrow_copy_constructible_v<Left_>)
				: m_hasLeft(true), m_storage(EitherTags::emplaceLeft, left) {}

			Either(Left_&& left) noexcept
				: m_hasLeft(true), m_storage(EitherTags::emplaceLeft, std::move(left)) {}

			template<class R, class = EnableOverloadRight_t<R>>
			Either(R const& right) noexcept(std::is_nothrow_copy_constructible_v<Right_>)
				: m_hasLeft(false), m_storage(EitherTags::emplaceRight, right) {}

			template<class R, class = EnableOverloadRight_t<R>>
			Either(R&& right) noexcept
				: m_hasLeft(false), m_storage(EitherTags::emplaceRight, std::move(right)) {}

			template<class...A>
			explicit Either(EitherTags::EmplaceLeft em, A&& ...a)
				noexcept(std::is_nothrow_constructible_v<Left_, A&&...>)
				: m_hasLeft(true), m_storage(em, std::forward<A>(a)...) {}

			template<class...A>
			explicit Either(EitherTags::EmplaceRight em, A&& ...a)
				noexcept(std::is_nothrow_constructible_v<Right_, A&&...>)
				: m_hasLeft(false), m_storage(em, std::forward<A>(a)...) {}

			template<class...A>
			Left_& emplaceLeft(A&& ...a)
				noexcept(std::is_nothrow_constructible_v<Left_, A&&...>)
			{
				if (m_hasLeft)
				{
					m_storage.u_left = Left_(std::forward<A>(a)...);
				}
				else
				{
					m_storage.u_right.Right_::~Right_();
					new (&m_storage.u_left) Left_(std::forward<A>(a)...);
					m_hasLeft = true;
				}
				return m_storage.u_left;
			}

			template<class...A>
			Right_& emplaceRight(A&& ...a)
				noexcept(std::is_nothrow_constructible_v<Right_, A&& ...>)
			{
				if (m_hasLeft)
				{
					m_storage.u_left.Left_::~Left_();
					new (&m_storage.u_right) Right_(std::forward<A>(a)...);
					m_hasLeft = false;
				}
				else
				{
					m_storage.u_right = Right_(std::forward<A>(a)...);
				}
				return m_storage.u_right;
			}

			Either& operator=(Left_&& left) noexcept
			{
				emplaceLeft(std::move(left));
				return *this;
			}

			Either& operator=(Left_ const& left)
				noexcept(std::is_nothrow_copy_constructible_v<Left_>
					&& std::is_nothrow_copy_assignable_v<Left_>)
			{
				emplaceLeft(left);
				return *this;
			}

			template<class R, class = EnableOverloadRight_t<R>>
			Either& operator=(R&& right) noexcept
			{
				emplaceRight(std::move(right));
				return *this;
			}

			template<class R, class = EnableOverloadRight_t<R>>
			Either& operator=(R const& right)
				noexcept(std::is_nothrow_copy_constructible_v<Right_>
					&& std::is_nothrow_copy_assignable_v<Right_>)
			{
				emplaceRight(right);
				return *this;
			}



			/*
			//	Returns true if left value exists.
			*/
			[[nodiscard]]
			bool hasLeft() const noexcept { return m_hasLeft; }

			/*
			//	Returns reference to left value.
			//	Precondition: (hasLeft()) the left value must exist.
			*/
			[[nodiscard]]
			Left_& accessLeft() noexcept
			{
				checkAccessLeft();
				return (m_storage.u_left);
			}

			/*
			//	Returns reference to left value.
			//	Precondition: (hasLeft()) the left value must exist.
			*/
			[[nodiscard]]
			Left_ const& accessLeft() const noexcept
			{
				checkAccessLeft();
				return (m_storage.u_left);
			}

			/*
			//	Returns true if right value exists.
			*/
			[[nodiscard]]
			bool hasRight() const noexcept { return !m_hasLeft; }

			/*
			//	Returns reference to right value.
			//	Precondition: (hasRight()) the right value must exist.
			*/
			[[nodiscard]]
			Right_& accessRight() noexcept
			{
				checkAccessRight();
				return (m_storage.u_right);
			}

			/*
			//	Returns reference to right value.
			//	Precondition: (hasRight()) the right value must exist.
			*/
			[[nodiscard]]
			Right_ const& accessRight() const noexcept
			{
				checkAccessRight();
				return (m_storage.u_right);
			}



			/*
			//	Returns true if left value exists.
			//	Dereference semantics consider Left is a Value and Right is No Value.
			*/
			[[nodiscard]]
			explicit operator bool() const noexcept { return m_hasLeft; }

			/*
			//	Returns reference to left value.
			//	Precondition: (hasLeft()) the left value must exist.
			//	Dereference semantics consider Left is a Value and Right is No Value.
			*/
			[[nodiscard]]
			Left_& operator*() noexcept { return accessLeft(); }

			/*
			//	Returns reference to left value.
			//	Precondition: (hasLeft()) the left value must exist.
			//	Dereference semantics consider Left is a Value and Right is No Value.
			*/
			[[nodiscard]]
			Left_ const& operator*() const noexcept { return accessLeft(); }

			/*
			//	Returns reference to left value.
			//	Precondition: (hasLeft()) the left value must exist.
			//	Dereference semantics consider Left is a Value and Right is No Value.
			*/
			[[nodiscard]]
			Left_* operator->() noexcept { return std::addressof(accessLeft()); }

			/*
			//	Returns reference to left value.
			//	Precondition: (hasLeft()) the left value must exist.
			//	Dereference semantics consider Left is a Value and Right is No Value.
			*/
			[[nodiscard]]
			Left_ const* operator->() const noexcept { return std::addressof(accessLeft()); }



			/*
			//	Returns a copy of left or throws.
			*/
			[[nodiscard]]
			Left_ unwrapLeft() const& noexcept(false)
			{
				requireUnwrapLeft();
				return m_storage.u_left;
			}

			/*
			//	Returns left or throws.
			*/
			[[nodiscard]]
			Left_ unwrapLeft() && noexcept(false)
			{
				requireUnwrapLeft();
				return std::move(m_storage.u_left);
			}

			/*
			//	Returns a copy of right or throws.
			*/
			[[nodiscard]]
			Right_ unwrapRight() const& noexcept(false)
			{
				requireUnwrapRight();
				return m_storage.u_right;
			}

			/*
			//	Returns right or throws.
			*/
			[[nodiscard]]
			Right_ unwrapRight() && noexcept(false)
			{
				requireUnwrapRight();
				return std::move(m_storage.u_right);
			}



			/*
			//	Converts  Either<Left,Right>  ->  Left
			//	Callable Parameter <() -> Left>  is only invoked if left value does not exist.
			*/
			template<class F>
			[[nodiscard]]
			Left_ joinLeft(F&& alternativeCreator) const&
				noexcept(std::is_nothrow_copy_constructible_v<Left_>
					&& std::is_nothrow_invocable_v<F&&>
					&& std::is_nothrow_constructible_v<Left_, std::invoke_result_t<F&&>>)
			{
				return m_hasLeft ? m_storage.u_left : std::forward<F>(alternativeCreator)();
			}

			/*
			//	Converts  Either<Left,Right>  ->  Left
			//	Callable Parameter <() -> Left>  is only invoked if left value does not exist.
			*/
			template<class F>
			[[nodiscard]]
			Left_ joinLeft(F&& alternativeCreator) &&
				noexcept(std::is_nothrow_invocable_v<F&&>
					&& std::is_nothrow_constructible_v<Left_, std::invoke_result_t<F&&>>)
			{
				return m_hasLeft ? std::move(m_storage.u_left) : std::forward<F>(alternativeCreator)();
			}



			/*
			//	Converts  Either<Left,Right>  ->  Right
			//	Callable Parameter <() -> Right>  is only invoked if right value does not exist.
			*/
			template<class F>
			[[nodiscard]]
			Right_ joinRight(F&& alternativeCreator) const&
				noexcept(std::is_nothrow_copy_constructible_v<Right_>
					&& std::is_nothrow_invocable_v<F&&>
					&& std::is_nothrow_constructible_v<Right_, std::invoke_result_t<F&&>>)
			{
				return m_hasLeft ? std::forward<F>(alternativeCreator)() : m_storage.u_right;
			}

			/*
			//	Converts  Either<Left,Right>  ->  Right
			//	Callable Parameter <() -> Right>  is only invoked if right value does not exist.
			*/
			template<class F>
			[[nodiscard]]
			Right_ joinRight(F&& alternativeCreator) &&
				noexcept(std::is_nothrow_invocable_v<F&&>
					&& std::is_nothrow_constructible_v<Right_, std::invoke_result_t<F&&>>)
			{
				return m_hasLeft ? std::forward<F>(alternativeCreator)() : std::move(m_storage.u_right);
			}



			/*
			//	Converts  Either<Left,Right>  ->  Either<U,Right>
			//	Callable Parameter <(Left) -> U>  is invoked with left value.
			*/
			template<class F, class U = std::invoke_result_t<F&&, Left_ const&>>
			[[nodiscard]]
			Either<U, Right_, RequireTraits_> mapLeft(F&& selector) const&
				noexcept(std::is_nothrow_invocable_v<F&&, Left_ const&>
					&& std::is_nothrow_copy_constructible_v<Right_>)
			{
				return m_hasLeft
					? Either<U, Right_, RequireTraits_>(EitherTags::emplaceLeft, std::forward<F>(selector)(m_storage.u_left))
					: Either<U, Right_, RequireTraits_>(EitherTags::emplaceRight, m_storage.u_right);
			}

			/*
			//	Converts  Either<Left,Right>  ->  Either<U,Right>
			//	Callable Parameter <(Left) -> U>  is invoked with left value.
			*/
			template<class F, class U = std::invoke_result_t<F&&, Left_&&>>
			[[nodiscard]]
			Either<U, Right_, RequireTraits_> mapLeft(F&& selector)&&
				noexcept(std::is_nothrow_invocable_v<F&&, Left_&&>)
			{
				return m_hasLeft
					? Either<U, Right_, RequireTraits_>(EitherTags::emplaceLeft, std::forward<F>(selector)(std::move(m_storage.u_left)))
					: Either<U, Right_, RequireTraits_>(EitherTags::emplaceRight, std::move(m_storage.u_right));
			}



			/*
			//	Converts  Either<Left,Right>  ->  Either<Left,U>
			//	Callable Parameter <(Right) -> U>  is invoked with right value.
			*/
			template<class F, class U = std::invoke_result_t<F&&, Right_ const&>>
			[[nodiscard]]
			Either<Left_, U, RequireTraits_> mapRight(F&& selector) const&
				noexcept(std::is_nothrow_invocable_v<F&&, Right_ const&>
					&& std::is_nothrow_copy_constructible_v<Left_>)
			{
				return m_hasLeft
					? Either<Left_, U, RequireTraits_>(EitherTags::emplaceLeft, m_storage.u_left)
					: Either<Left_, U, RequireTraits_>(EitherTags::emplaceRight, std::forward<F>(selector)(m_storage.u_right));
			}

			/*
			//	Converts  Either<Left,Right>  ->  Either<Left,U>
			//	Callable Parameter <(Right) -> U>  is invoked with right value.
			*/
			template<class F, class U = std::invoke_result_t<F&&, Right_&&>>
			[[nodiscard]]
			Either<Left_, U, RequireTraits_> mapRight(F&& selector) &&
				noexcept(std::is_nothrow_invocable_v<F&&, Right_&&>)
			{
				return m_hasLeft
					? Either<Left_, U, RequireTraits_>(EitherTags::emplaceLeft, std::move(m_storage.u_left))
					: Either<Left_, U, RequireTraits_>(EitherTags::emplaceRight, std::forward<F>(selector)(std::move(m_storage.u_right)));
			}



			/*
			//	Converts  Either<Left,Right>  ->  Either<L2,R2>
			//	First Callable Parameter <(Left) -> L2>
			//	Second Callable Parameter <(Right) -> R2>
			*/
			template<class F, class G,
				class L2 = std::invoke_result_t<F&&, Left_ const&>,
				class R2 = std::invoke_result_t<G&&, Right_ const&>>
			[[nodiscard]]
			Either<L2, R2, RequireTraits_> mapMatch(F&& leftSelector, G&& rightSelector) const&
				noexcept(std::is_nothrow_invocable_v<F&&, Left_ const&>
					&& std::is_nothrow_copy_constructible_v<Left_>
					&& std::is_nothrow_invocable_v<G&&, Right_ const&>
					&& std::is_nothrow_copy_constructible_v<Right_>)
			{
				return m_hasLeft
					? Either<L2, R2, RequireTraits_>(EitherTags::emplaceLeft, std::forward<F>(leftSelector)(m_storage.u_left))
					: Either<L2, R2, RequireTraits_>(EitherTags::emplaceRight, std::forward<G>(rightSelector)(m_storage.u_right));
			}

			/*
			//	Converts  Either<Left,Right>  ->  Either<L2,R2>
			//	First Callable Parameter <(Left) -> L2>
			//	Second Callable Parameter <(Right) -> R2>
			*/
			template<class F, class G,
				class L2 = std::invoke_result_t<F&&, Left_&&>,
				class R2 = std::invoke_result_t<G&&, Right_&&>>
			[[nodiscard]]
			Either<L2, R2, RequireTraits_> mapMatch(F&& leftSelector, G&& rightSelector) &&
				noexcept(std::is_nothrow_invocable_v<F&&, Left_&&>
					&& std::is_nothrow_invocable_v<G&&, Right_&&>)
			{
				return m_hasLeft
					? Either<L2, R2, RequireTraits_>(EitherTags::emplaceLeft, std::forward<F>(leftSelector)(std::move(m_storage.u_left)))
					: Either<L2, R2, RequireTraits_>(EitherTags::emplaceRight, std::forward<G>(rightSelector)(std::move(m_storage.u_right)));
			}



			/*
			//	Converts  Either<Either<Left,Right>, Right>  ->  Either<Left,Right>
			*/
			template<class L = std::enable_if_t<std::is_constructible_v<Left_, Right_ const&>, Left_>>
			[[nodiscard]]
			L flattenLeft() const&
				noexcept(std::is_nothrow_copy_constructible_v<Left_>
					&& std::is_nothrow_constructible_v<Left_, Right_ const&>)
			{
				if (!m_hasLeft) { return Left_(EitherTags::emplaceRight, m_storage.u_right); }
				return m_storage.u_left;
			}

			/*
			//	Converts  Either<Either<Left,Right>, Right>  ->  Either<Left,Right>.
			*/
			template<class L = std::enable_if_t<std::is_constructible_v<Left_, Right_&&>, Left_>>
			[[nodiscard]]
			L flattenLeft()&&
				noexcept(std::is_nothrow_constructible_v<Left_, Right_&&>)
			{
				if (!m_hasLeft) { return Left_(EitherTags::emplaceRight, std::move(m_storage.u_right)); }
				return std::move(m_storage.u_left);
			}



			/*
			//	Converts  Either<Left, Either<Left, Right>>  ->  Either<Left,Right>
			*/
			template<class R = std::enable_if_t<std::is_constructible_v<Right_, Left_ const&>, Right_>>
			[[nodiscard]]
			R flattenRight() const&
				noexcept(std::is_nothrow_copy_constructible_v<Right_>
					&& std::is_nothrow_constructible_v<Right_, Left_ const&>)
			{
				if (m_hasLeft) { return Right_(EitherTags::emplaceLeft, m_storage.u_left); }
				return m_storage.u_right;
			}

			/*
			//	Converts  Either<Left, Either<Left, Right>>  ->  Either<Left,Right>
			*/
			template<class R = std::enable_if_t<std::is_constructible_v<Right_, Left_&&>, Right_>>
			[[nodiscard]]
			R flattenRight() &&
				noexcept(std::is_nothrow_constructible_v<Right_, Left_&&>)
			{
				if (m_hasLeft) { return Right_(EitherTags::emplaceLeft, std::move(m_storage.u_left)); }
				return std::move(m_storage.u_right);
			}
		};



		template<class Left_, class Right_, class RequireTraits_>
		void swap(Either<Left_, Right_, RequireTraits_>& a, Either<Left_, Right_, RequireTraits_>& b) noexcept
		{
			a.swap(b);
		}



		/*
		//	Creates an Either from a Left value.
		//	The type of Right is the first template type parameter.
		*/
		template<class Right_, class Left_>
		[[nodiscard]]
		Either<Left_, Right_> makeLeft(Left_&& left) noexcept
		{
			return Either<Left_, Right_>(EitherTags::emplaceLeft, std::move(left));
		}

		/*
		//	Creates an Either from a Left value.
		//	The type of Right is the first template type parameter.
		*/
		template<class Right_, class Left_>
		[[nodiscard]]
		Either<Left_, Right_> makeLeft(Left_ const& left)
			noexcept(std::is_nothrow_copy_constructible_v<Left_>)
		{
			return Either<Left_, Right_>(EitherTags::emplaceLeft, left);
		}

		/*
		//	Creates an Either from a Right value.
		//	The type of Left is the first template type parameter.
		*/
		template<class Left_, class Right_>
		[[nodiscard]]
		Either<Left_, Right_> makeRight(Right_&& right) noexcept
		{
			return Either<Left_, Right_>(EitherTags::emplaceRight, std::move(right));
		}

		/*
		//	Creates an Either from a Right value.
		//	The type of Left is the first template type parameter.
		*/
		template<class Left_, class Right_>
		[[nodiscard]]
		Either<Left_, Right_> makeRight(Right_ const& right)
			noexcept(std::is_nothrow_copy_constructible_v<Right_>)
		{
			return Either<Left_, Right_>(EitherTags::emplaceRight, right);
		}



		/*
		//	void-like type.
		*/
		struct Voidlike {};
		constexpr Voidlike const voidlike{};



		namespace LeftOps
		{
			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			Left_ operator|(Either<Left_, Right_, RequireTraits_> const& either, F&& alternativeCreator)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_> const&>()
					.joinLeft(std::declval<F&&>())))
			{
				return either.joinLeft(std::forward<F>(alternativeCreator));
			}

			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			Left_ operator|(Either<Left_, Right_, RequireTraits_>&& either, F&& alternativeCreator)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_>&&>()
					.joinLeft(std::declval<F&&>())))
			{
				return std::move(either).joinLeft(std::forward<F>(alternativeCreator));
			}

			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			auto operator&(Either<Left_, Right_, RequireTraits_> const& either, F&& selector)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_> const&>()
					.mapLeft(std::declval<F&&>())))
			{
				return either.mapLeft(std::forward<F>(selector));
			}

			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			auto operator&(Either<Left_, Right_, RequireTraits_>&& either, F&& selector)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_>&&>()
					.mapLeft(std::declval<F&&>())))
			{
				return std::move(either).mapLeft(std::forward<F>(selector));
			}
		}

		namespace RightOps
		{
			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			Right_ operator|(Either<Left_, Right_, RequireTraits_> const& either, F&& alternativeCreator)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_> const&>()
					.joinRight(std::declval<F&&>())))
			{
				return either.joinRight(std::forward<F>(alternativeCreator));
			}

			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			Right_ operator|(Either<Left_, Right_, RequireTraits_>&& either, F&& alternativeCreator)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_>&&>()
					.joinRight(std::declval<F&&>())))
			{
				return std::move(either).joinRight(std::forward<F>(alternativeCreator));
			}

			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			auto operator&(Either<Left_, Right_, RequireTraits_> const& either, F&& selector)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_> const&>()
					.mapRight(std::declval<F&&>())))
			{
				return either.mapRight(std::forward<F>(selector));
			}

			template<class Left_, class Right_, class RequireTraits_, class F>
			[[nodiscard]]
			auto operator&(Either<Left_, Right_, RequireTraits_>&& either, F&& selector)
				noexcept(noexcept(std::declval<Either<Left_, Right_, RequireTraits_>&&>()
					.mapRight(std::declval<F&&>())))
			{
				return std::move(either).mapRight(std::forward<F>(selector));
			}
		}
	}
}
