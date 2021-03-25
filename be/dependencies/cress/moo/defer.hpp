/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/defer.hpp
//
//	Summary		:	Executes an action in the destructor.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	27/11/2019
*/

#pragma once

#include <type_traits>

namespace cress
{
	namespace moo
	{
		/*
		//	Defer
		//	Executes an action in the destructor.
		//
		//	Example:

			cress::moo::Defer fa{ [&]()noexcept { cleanupStuff(); } };

		//	Example 2:

			CRESS_MOO_DEFER(fa, ([&]()noexcept { cleanupStuff(); }));

		//	Example 3:

			CRESS_MOO_DEFER_SINGLE(fa, cleanupStuff());

		//	Example 4:

			CRESS_MOO_DEFER_BEGIN(fa);
			cleanupStuff();
			cleanupOtherStuff();
			CRESS_MOO_DEFER_END(fa);

		*/
		template<class Callable,
			class = std::enable_if_t<std::is_nothrow_invocable_v<Callable>>
		> class Defer
		{
		public:
			~Defer() noexcept { action(); }

			Defer() = delete;
			Defer(Defer const&) = delete;
			Defer& operator=(Defer const&) = delete;
			Defer(Defer&&) = delete;
			Defer& operator=(Defer&&) = delete;

			explicit Defer(Callable&& action) noexcept
				: action(std::forward<Callable>(action)) {}

			explicit Defer(Callable const& action) noexcept(std::is_nothrow_copy_constructible_v<Callable>)
				: action(action) {}

		private:
			Callable action{};
		};
	}
}

#define CRESS_MOO_TOKEN_CONCAT(a, b) a ## b

#define CRESS_MOO_ANONYMOUS_IDENTIFIER_IMPL(line)\
	CRESS_MOO_TOKEN_CONCAT(anonymous, line)

// Anonymous identifier based on current line of source code.
#define CRESS_MOO_ANONYMOUS_IDENTIFIER\
	CRESS_MOO_ANONYMOUS_IDENTIFIER_IMPL(__LINE__)

#define CRESS_MOO_DEFER_IMPL(name, callable)\
	auto CRESS_MOO_TOKEN_CONCAT(name, __fn) = (callable);\
	::cress::moo::Defer<decltype(CRESS_MOO_TOKEN_CONCAT(name, __fn))> name{\
		::std::move(CRESS_MOO_TOKEN_CONCAT(name, __fn))\
	}

// Defers execution of the callable until the end of scope.
#define CRESS_MOO_DEFER_CALLABLE(callable)\
	CRESS_MOO_DEFER_IMPL(CRESS_MOO_ANONYMOUS_IDENTIFIER, callable)

// Defers execution of the expression until the end of scope.
#define CRESS_MOO_DEFER_EXPRESSION(expression)\
	CRESS_MOO_DEFER_CALLABLE([&]()noexcept{ static_cast<void>(expression); })

// Defers execution of a body of code until the end of the surrounding scope.
#define CRESS_MOO_DEFER_BEGIN(name)\
	auto CRESS_MOO_TOKEN_CONCAT(name, __fn) = ([&]()noexcept { do{}while(0)

// Ends the body scope of the code to defer.
#define CRESS_MOO_DEFER_END(name)\
	});\
	::cress::moo::Defer<decltype(CRESS_MOO_TOKEN_CONCAT(name, __fn))> name{\
		::std::move(CRESS_MOO_TOKEN_CONCAT(name, __fn))\
	}
