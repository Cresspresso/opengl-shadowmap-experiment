/*
//	be/logger
//	Used by Application for logging exceptions.
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

namespace be
{
	// Interface
	class Logger
	{
	public:
		virtual ~Logger() noexcept = default;
	protected:
		Logger() = default;
		Logger(Logger&&) noexcept = default;
		Logger& operator=(Logger&&) noexcept = default;
		Logger(Logger const&) = default;
		Logger& operator=(Logger const&) = default;

	private:
		// INTERFACE METHODS
		virtual void LogException() noexcept = 0;

	public:
		void logException() noexcept { LogException(); }
	};



	class DefaultLogger final : public Logger
	{
	public:
		~DefaultLogger() noexcept final = default;
		DefaultLogger() = default;
		DefaultLogger(DefaultLogger&&) noexcept = default;
		DefaultLogger& operator=(DefaultLogger&&) noexcept = default;
		DefaultLogger(DefaultLogger const&) = default;
		DefaultLogger& operator=(DefaultLogger const&) = default;
	private:
		void LogException() noexcept final;
	};
}
