/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/accessor.hpp
//
//	Summary		:	Macros for defining getters, setters, and accessors.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	27/11/2019
*/

/*
//	Example:

	#include <string>

	#include "accessors.hpp"

	class Fancy
	{
	private:
		std::string m_name;
	public:
		CRESS_MOO_PROPERTY(Name, std::string, { return m_name; }, { m_name = CRESS_MOO_SETTER_VALUE_MOVED; });
		CRESS_MOO_PROPERTY(Heart, char,
			{ // get:
				return m_name.size() < 1
					? '?'
					: m_name[1];
			},
			{ // set:
				if (m_name.size() >= 1) {
					m_name[1] = CRESS_MOO_SETTER_VALUE_MOVED;
				}
			});

		CRESS_MOO_ACCESSOR(at(size_t const i), char&, char, noexcept { return m_name[i]; });
	};

	#include <cassert>

	int main()
	{
		Fancy a;
		a.setName("name");
		assert(a.getName() == "name");

		std::string const c = "slit";
		a.setName(c);
		assert(a.getName() == "slit");
		assert(c == "slit");

		a.setHeart('p');
		assert(a.getHeart() == 'p');
		assert(a.getName() == "spit");

		Fancy const& r = a;
		assert('p' == r.at(1));
		a.at(1) = 'L';
		assert('L' == r.at(1));
	}

//
*/

#pragma once

#define CRESS_MOO_TOKEN_CONCAT(a, b) a ## b

#define CRESS_MOO_GETTER(name, type, body)\
	type CRESS_MOO_TOKEN_CONCAT(get, name)() const body\
	struct DUMMY__

#define CRESS_MOO_SETTER(name, type, body)\
	void CRESS_MOO_TOKEN_CONCAT(set, name)(type const& value) body\
	void CRESS_MOO_TOKEN_CONCAT(set, name)(type&& value) body\
	struct DUMMY__

#define CRESS_MOO_SETTER_VALUE_MOVED (std::forward<decltype(value)>(value))

#define CRESS_MOO_SETTER_VALUE_ACCESSED (value)

#define CRESS_MOO_PROPERTY(name, type, getter, setter)\
	CRESS_MOO_GETTER(name, type, getter);\
	CRESS_MOO_SETTER(name, type, setter)

#define CRESS_MOO_ACCESSOR(templ, sig, mutable_result_t, const_result_t, body)\
	templ mutable_result_t sig body\
	templ const_result_t sig const body\
	struct DUMMY__
