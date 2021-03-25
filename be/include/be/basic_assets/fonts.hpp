
#pragma once

#include <filesystem>

#include "be/ft.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace fonts
		{
			be::ft::Font loadArialFont(
				std::filesystem::path const& basicAssetsFolder,
				FT_UInt width,
				FT_UInt height = 0
			);
		}
	}
}
