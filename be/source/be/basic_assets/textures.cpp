
#include "be/soil.hpp"
#include "be/basic_assets/textures.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace textures
		{
			be::mem::gl::Texture loadFlagTexture(std::filesystem::path const& basicAssetsFolder)
			{
				auto const path = basicAssetsFolder / "textures/flag.png";
				auto const str = path.string();
				auto texture = be::soil::load_OGL_texture(str.c_str(), SOIL_LOAD_RGBA, 0, 0);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				return texture;
			}

			be::mem::gl::Texture loadSkyboxCubemap(std::filesystem::path const& basicAssetsFolder)
			{
				auto const dir = (basicAssetsFolder / "cubemaps/envmap_interstellar/").string() + "interstellar_";
				auto texture = be::soil::load_OGL_cubemap(
					(dir + "rt.tga").c_str(),
					(dir + "lf.tga").c_str(),
					(dir + "up.tga").c_str(),
					(dir + "dn.tga").c_str(),
					(dir + "bk.tga").c_str(),
					(dir + "ft.tga").c_str(),
					SOIL_LOAD_RGB,
					0,
					SOIL_FLAG_MIPMAPS
				);
				BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_CUBE_MAP, texture.get(), GL_TEXTURE0);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				return texture;
			}
		}
	}
}
