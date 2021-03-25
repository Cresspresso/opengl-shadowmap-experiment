/*
//	be/input
//	Input state.
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

#include <glew/glew.h>
#include <freeglut/freeglut.h>
#include <map>
#include <optional>
#include <glm/vec2.hpp>

namespace be
{
	enum class InputState : std::int_fast8_t
	{
		CurrentlyUp = 0b0000,
		CurrentlyDown = 0b0001,
		GoingUp = 0b0010,
		GoingDown = 0b0011,
		GoingUpAgain = 0b0110,
		GoingDownAgain = 0b0111,
	};

	template<class Key>
	using InputMap = std::map<Key, InputState>;

	/*enum class MouseWheelState : std::int_fast8_t
	{
		None = 0,
		Positive = 1,
		Negative = -1
	};*/

	struct Input
	{
		InputMap<unsigned char> keyboardKeys;
		InputMap<int> specialKeys;
		InputMap<int> mouseButtons;
	};

	namespace input
	{
		inline constexpr bool isDownAtAll(InputState const state) noexcept
		{
			switch (state)
			{
			case InputState::CurrentlyDown:
			case InputState::GoingDown:
			case InputState::GoingDownAgain:
				return true;
			default:
				return false;
			}
		}

		inline constexpr bool isGoingDown(InputState const state) noexcept
		{
			return InputState::GoingDown == state;
		}

		inline constexpr bool isGoingUp(InputState const state) noexcept
		{
			return InputState::GoingUp == state;
		}

		inline void clearInputStates(Input& input) noexcept
		{
			input.keyboardKeys.clear();
			input.specialKeys.clear();
			input.mouseButtons.clear();
		}

		template<class Key>
		InputState getElseConsiderUp(InputMap<Key> const& states, Key const& key) noexcept
		{
			if (auto const it = states.find(static_cast<Key>(key));
				it != states.end())
			{
				return it->second;
			}
			else
			{
				return InputState::CurrentlyUp;
			}
		}

		template<class Key>
		InputState informGoingDown(InputMap<Key>& states, Key const& key)
		{
			auto& state = states[key];
			switch (state)
			{
			case InputState::CurrentlyDown:
			case InputState::GoingDown:
			case InputState::GoingDownAgain:
				state = InputState::GoingDownAgain;
				break;
			default:
				state = InputState::GoingDown;
				break;
			}
			return state;
		}

		template<class Key>
		InputState informGoingUp(InputMap<Key>& states, Key const& key)
		{
			auto& state = states[key];
			switch (state)
			{
			case InputState::CurrentlyUp:
			case InputState::GoingUp:
			case InputState::GoingUpAgain:
				state = InputState::GoingUpAgain;
				break;
			default:
				state = InputState::GoingUp;
				break;
			}
			return state;
		}

		//MouseWheelState informGoingPositive(Input& input, int wheel)
		//{
		//	input.mouseWheels[wheel] = MouseWheelState::Positive;
		//}

		inline void afterUpdate(InputState& state) noexcept
		{
			switch (state)
			{
			case InputState::GoingDown:
			case InputState::GoingDownAgain:
				state = InputState::CurrentlyDown;
				break;
			case InputState::GoingUp:
			case InputState::GoingUpAgain:
				state = InputState::CurrentlyUp;
				break;
			}
		}

		template<class Key>
		void afterUpdate(InputMap<Key>& states) noexcept
		{
			for (auto& [_, state] : states)
			{
				afterUpdate(state);
			}
		}

		inline void afterUpdate(Input& input) noexcept
		{
			afterUpdate(input.keyboardKeys);
			afterUpdate(input.specialKeys);
			afterUpdate(input.mouseButtons);
		}
	}
}
