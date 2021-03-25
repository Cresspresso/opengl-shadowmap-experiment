
#include "be/basic_assets/fonts.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace fonts
		{
			be::ft::Font loadArialFont(
				std::filesystem::path const& basicAssetsFolder,
				FT_UInt width,
				FT_UInt height
			)
			{
				return be::ft::loadFont((basicAssetsFolder / "fonts/arial.ttf").string().c_str(), width, height);
			}
		}
	}
}
