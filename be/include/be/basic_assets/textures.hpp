
#pragma once

#include <filesystem>

#include "be/mem/gl.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace textures
		{
			be::mem::gl::Texture loadFlagTexture(std::filesystem::path const& basicAssetsFolder);

			be::mem::gl::Texture loadSkyboxCubemap(std::filesystem::path const& basicAssetsFolder);
		}
	}
}
