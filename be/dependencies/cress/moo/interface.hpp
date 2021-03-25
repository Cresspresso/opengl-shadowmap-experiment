/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/interface.hpp
//
//	Summary		:	Virtual destructor, unmovable base class.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	19/10/2019
*/

#pragma once

namespace cress
{
	namespace moo
	{
		/*
		//	Interface
		//	Privately inherit this class to make your class
		//	have a virtual destructor, and be uncopyable and unmovable.
		//
		//	Example:

			class MyInterfaceClass : private cress::moo::Interface
			{
				virtual int getValue() = 0;
			};

		*/
		struct Interface
		{
			virtual ~Interface() noexcept = default;
			constexpr Interface() noexcept = default;
			Interface(Interface const&) = delete;
			Interface& operator=(Interface const&) = delete;
			Interface(Interface&&) = delete;
			Interface& operator=(Interface&&) = delete;
		};

		/*
		//	Unmovable
		//	Privately inherit this class to make your class
		//	uncopyable and unmovable.
		//
		//	Example:

			class MyResponsibleClass : private cress::moo::Unmovable
			{
			private:
				int id{};
			public:
				MyResponsibleClass(int id) : id(id) {}
				int getID() { return id; }
			};

		*/
		struct Unmovable
		{
			~Unmovable() noexcept = default;
			constexpr Unmovable() noexcept = default;
			Unmovable(Unmovable const&) = delete;
			Unmovable& operator=(Unmovable const&) = delete;
			Unmovable(Unmovable&&) = delete;
			Unmovable& operator=(Unmovable&&) = delete;
		};
	}
}
