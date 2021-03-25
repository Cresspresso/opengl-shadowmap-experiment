/*
//	Reads an entire file and stores it in memory as one string.
*/

#pragma once

#include <string>
#include <iosfwd>
#include <stdexcept>

namespace be
{
	class ReadEntireFileException final : public std::runtime_error
	{
	public:
		ReadEntireFileException(std::string const& msg);
	};

	std::string readEntireStream(std::istream& ins);
	std::string readEntireFile(std::string const& filePath);
}
