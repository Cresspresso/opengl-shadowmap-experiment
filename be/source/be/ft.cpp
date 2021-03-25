
#include <vector>

#include "be/mem/ft.hpp"

#include "be/ft.hpp"

namespace be
{
	namespace ft
	{
		namespace
		{
			static be::mem::gl::Texture makeGlyphTexture(FT_Face const face)
			{
				auto texture = be::mem::gl::makeTexture();
				BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, texture.get(), GL_TEXTURE0);
				
				auto const& b = face->glyph->bitmap;
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, b.width, b.rows, 0, GL_RED, GL_UNSIGNED_BYTE, b.buffer);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				return texture;
			}
		}

		Font loadFont(char const* const filePath, FT_UInt const glyphWidth, FT_UInt const glyphHeight)
		{
			Font font;
			std::map<GLubyte, FT_Error> notLoaded;

			{
				auto ft = be::mem::ft::makeLibrary();
				auto face = be::mem::ft::loadFace(ft.get(), filePath);

				if (FT_Error const e = FT_Set_Pixel_Sizes(face.get(), glyphWidth, glyphHeight))
				{
					throw LoadFontException("set pixel sizes failed", filePath, e);
				}

				glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
				CRESS_MOO_DEFER_EXPRESSION(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));

				// load the glyphs
				for (GLubyte c = 0U; c < 128U; ++c)
				{
					if (FT_Error const e = FT_Load_Char(face.get(), c, FT_LOAD_RENDER))
					{
						notLoaded.insert(std::make_pair(c, e));
						continue;
					}

					FontGlyph glyph;
					glyph.texture = makeGlyphTexture(face.get());

					auto const* p = face->glyph;
					glyph.size = glm::ivec2(p->bitmap.width, p->bitmap.rows);
					glyph.bearing = glm::ivec2(p->bitmap_left, p->bitmap_top);
					glyph.advance = p->advance.x >> 6;

					font.insert(std::pair<GLchar, FontGlyph>(c, std::move(glyph)));
				}
			}

			// return
			if (notLoaded.empty() == false)
			{
				std::string msg = "some glyphs failed to load for characters: ";
				for (auto const& [c, e] : notLoaded)
				{
					if (isprint(c)) {
						msg += '\'';
						msg += c;
						msg += '\'';
					}
					else {
						msg += std::to_string(static_cast<int>(c));
					}
					msg += " (";
					msg += std::to_string(e);
					msg += "), ";
				}
				throw LoadFontException(msg, filePath, 0);
			}

			return font;
		}
	}
}
