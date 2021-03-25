
#pragma once

#include "be/gl.hpp"
#include "be/pink/trs.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace meshes
		{
			be::gl::BasicMesh makeQuadMesh();
		}
	}

	namespace pink
	{
		struct QuadTransform
		{
			be::pink::BasicTransform base;
			glm::vec2 quadSize = glm::vec2(1.0f);
		};

		inline glm::mat4 calcTrs(QuadTransform const& t)
		{
			return be::pink::calcTrs(t.base.translation, t.base.rotation, t.base.scale * glm::vec3(t.quadSize, 1.0f));
		}
	}
}
