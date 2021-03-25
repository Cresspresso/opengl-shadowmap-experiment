/*
//	Bachelor of Software Engineering
//	Media Design School
//	Auckland
//	New Zealand
//
//	(c) 2019 Media Design School
//
//	File		:	cress/moo/delay.hpp
//
//	Summary		:	Data type with remaining time property, which decreases towards zero when updated with delta time.
//
//	Project		:	moo
//	Namespace	:	cress::moo
//	Author		:	Elijah Shadbolt
//	Email		:	elijah.sha7979@mediadesign.school.nz
//	Date Edited	:	03/10/2019
*/

#pragma once

namespace cress
{
	namespace moo
	{
		namespace DelayTags
		{
			struct Uninitialised {};
			constexpr Uninitialised const uninitialised{};
		}

		/*
		//	Data type with remaining time property, which decreases towards zero when updated with delta time.
		//
		//	Example:

			float const length = 0.5f;
			cress::moo::Delay<float> timer{ length };
			for (int frame = 0; frame < 1000; ++frame)
			{
				float const deltaTime = 1.0f / 60.0f;
				timer.updateUnclamped(deltaTime);
				if (timer.isFinished())
				{
					std::cout << "Timer has finished once on frame " << frame << ".\n";
					timer.setRemaining(length);
				}
			}

		*/
		template<class Duration_>
		class Delay
		{
		private:
			Duration_ m_remaining;

		public:
			using This = Delay;
			using Duration = Duration_;

			~Delay() noexcept = default;
			constexpr Delay(Delay&&) noexcept = default;
			constexpr Delay& operator=(Delay&&) noexcept = default;
			constexpr Delay(Delay const&) = default;
			constexpr Delay& operator=(Delay const&) = default;
			constexpr Delay() : m_remaining() {}
			constexpr Delay(DelayTags::Uninitialised) noexcept {}
			explicit constexpr Delay(Duration_&& remaining) noexcept : m_remaining(static_cast<Duration_&&>(remaining)) {}
			explicit constexpr Delay(Duration_ const& remaining) : m_remaining(remaining) {}

			constexpr Duration_ const& getRemaining() const noexcept { return m_remaining; }
			constexpr void setRemaining(Duration_ const& remaining) { m_remaining = remaining; }
			constexpr void setRemaining(Duration_&& remaining) noexcept { m_remaining = static_cast<Duration_&&>(remaining); }

			/*
			//	True if remaining time <= zero.
			*/
			constexpr bool isFinished() const { return m_remaining <= Duration_(); }

			/*
			//	Decreases remaining time by delta time.
			//	Remaining time may drop below zero.
			//	Faster than updateClamped().
			*/
			constexpr void updateUnclamped(Duration_ const& deltaTime)
			{
				m_remaining -= deltaTime;
			}

			/*
			//	Decreases remaining time by delta time.
			//	Remaining time may drop below zero.
			//	Faster than updateClamped().
			//	Returns isFinished().
			*/
			constexpr bool updateUnclampedFinished(Duration_ const& deltaTime)
			{
				updateUnclamped(deltaTime);
				return isFinished();
			}

			/*
			//	Decreases remaining time by delta time.
			//	Remaining time will be clamped to zero.
			*/
			constexpr void updateClamped(Duration_ const& deltaTime)
			{
				updateUnclamped(deltaTime);

				Duration_ zero = Duration_();
				if (m_remaining < zero)
				{
					m_remaining = static_cast<Duration_&&>(zero);
				}
			}

			/*
			//	Decreases remaining time by delta time.
			//	Remaining time will be clamped to zero.
			//	Returns isFinished().
			*/
			constexpr bool updateClampedFinished(Duration_ const& deltaTime)
			{
				updateClamped(deltaTime);
				return isFinished();
			}
		};
	}
}
