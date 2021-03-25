
#pragma once

#include <map>
#include <string>
#include <glm/vec2.hpp>

#include "be/mem/gl.hpp"
#include "be/mem/ft.hpp"

namespace be
{
	namespace ft
	{
		struct FontGlyph
		{
			be::mem::gl::Texture texture = {};
			glm::ivec2 size = {};
			glm::ivec2 bearing = {};
			int advance = {};
		};

		using Font = std::map<GLchar, FontGlyph>;



		class LoadFontException final : public be::mem::ft::FreeTypeException
		{
		public:
			LoadFontException(std::string const& msg, char const* const filePath, FT_Error const e)
				: FreeTypeException("[be::ft] load font exception: " + msg + ". font at: " + filePath, e)
			{}
		};

		Font loadFont(char const* const filePath, FT_UInt const glyphWidth, FT_UInt const glyphHeight);
	}
}
