/*
//	Reads an entire file and stores it in memory as one string.
*/

#pragma once

#include <fstream>

#include "be/read_entire_file.hpp"

namespace be
{
	ReadEntireFileException::ReadEntireFileException(std::string const& msg)
		: std::runtime_error("[be] read entire file exception: " + msg)
	{}



	std::string readEntireStream(std::istream& ins)
	{
		std::string content;

		// Determine the size of of the file in characters and resize the string variable to accomodate
		ins.seekg(0, std::ios::end);
		content.resize(static_cast<size_t>(ins.tellg()));

		// Set the position of the next character to be read back to the beginning
		ins.seekg(0, std::ios::beg);
		// Extract the contents of the file and store in the string variable
		ins.read(&content[0], content.size());

		return content;
	}



	std::string readEntireFile(std::string const& filePath)
	{
		// Open the file for reading
		std::ifstream file{ filePath, std::ios::in };

		// Ensure the file is open and readable
		if (!file.good())
		{
			throw ReadEntireFileException("at: " + filePath);
		}

		return readEntireStream(file);
	}
}
