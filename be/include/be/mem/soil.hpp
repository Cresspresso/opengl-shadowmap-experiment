/*
//	be/mem/soil
//	Memory safety for SOIL pointers.
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

#include <memory>
#include <soil/SOIL.h>
#include <cress/moo/defer.hpp>

namespace be
{
	namespace mem
	{
		namespace soil
		{
			struct ImageDeleter { void operator()(unsigned char* p) { SOIL_free_image_data(p); } };
			using Image = std::unique_ptr<unsigned char[], ImageDeleter>;
		}
	}
}
