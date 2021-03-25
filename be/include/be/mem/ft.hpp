
#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace be
{
	namespace mem
	{
		namespace ft
		{
			class FreeTypeException : public std::runtime_error
			{
			protected:
				FreeTypeException(std::string const& msg, FT_Error const e)
					: std::runtime_error(msg + " (FT_Error: " + std::to_string(e) + ')')
				{}
			};



			class InitLibraryException final : public FreeTypeException
			{
			public:
				InitLibraryException(FT_Error const e)
					: FreeTypeException("[be::mem::ft] init library exception", e)
				{}
			};

			struct LibraryDeleter { using pointer = FT_Library; void operator()(pointer p) { FT_Done_FreeType(p); } };
			using Library = std::unique_ptr<FT_Library, LibraryDeleter>;
			inline Library makeLibrary()
			{
				FT_Library p;
				if (FT_Error const e = FT_Init_FreeType(&p))
				{
					throw InitLibraryException(e);
				}
				return Library(p);
			}



			class LoadFaceException final : public FreeTypeException {
			public:
				LoadFaceException(char const*const filePath, FT_Error const e)
					: FreeTypeException(std::string("[be::mem::ft] load face exception: font at: ") + filePath, e)
				{}
			};

			struct FaceDeleter { using pointer = FT_Face; void operator()(pointer p) { FT_Done_Face(p); } };
			using Face = std::unique_ptr<FT_Face, FaceDeleter>;
			inline Face loadFace(FT_Library const library, char const* const filePath)
			{
				FT_Face p;
				if (FT_Error const e = FT_New_Face(library, filePath, 0, &p))
				{
					throw LoadFaceException(filePath, e);
				}
				return Face(p);
			}
		}
	}
}
