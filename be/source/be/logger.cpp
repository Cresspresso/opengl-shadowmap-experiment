/*
//	be/logger
//	Used by Application for logging exceptions.
//
//	Elijah Shadbolt
//	2019
*/

#include <iostream>

#include "be/logger.hpp"

namespace be
{
	void DefaultLogger::LogException() noexcept
	{
		std::cerr << "! Exception: ";
		try { throw; }
		catch (std::exception const& e) { std::cerr << e.what(); }
		catch (char const* what) { std::cerr << what; }
		catch (...) { std::cerr << "unknown"; }
		std::cerr << '\n';
	}
}
