/*
//	be/application
//	The singleton manager of the entire application lifetime.
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

#include <functional>
#include <stdexcept>

#include "be/logger.hpp"
#include "be/game.hpp"

namespace be
{
	class RunException : public std::runtime_error
	{
	protected:
		RunException(char const* what);
		RunException(std::string const& what);
	};

	class RunAgainException : public RunException
	{
	public:
		RunAgainException(char const* what = "[be] run again exception: the application has already been run");
		RunAgainException(std::string const& what);
	};

	class RunInfoException : public RunException
	{
	public:
		RunInfoException(char const* what = "[be] run info exception: invalid argument");
		RunInfoException(std::string const& what);
	};

	class CreateGameException : public RunException
	{
	public:
		CreateGameException(char const* what = "[be] create game exception: functor returned nullptr");
		CreateGameException(std::string const& what);
	};

	class GlewInitException : public RunException
	{
	public:
		GlewInitException(char const* what = "[be] glew init exception");
		GlewInitException(std::string const& what);
	};

	struct ApplicationRunInfo
	{
		// REQUIRED
		Logger* logger{};
		int* argc{};
		char** argv{};
		std::function<std::unique_ptr<Game>()> createGame{};

		// OPTIONAL: Can be provided with custom values.
		int windowWidth = 1920 / 2; // require > 0
		int windowHeight = 1080 / 2; // require > 0
		char const* windowTitle = "be app"; // require != nullptr
	};

	namespace Application
	{
		void run(ApplicationRunInfo const& info) noexcept;
		void exit() noexcept; // warning: not thread safe
		Game* getGame() noexcept;
		Logger* getLogger() noexcept;
		void logException() noexcept;
	};
}
