
#pragma once

#include "be/gl.hpp"
#include "be/pink/trs.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace meshes
		{
			be::gl::BasicMesh makeCubeMesh();
		}
	}

	namespace pink
	{
		struct BoxTransform
		{
			be::pink::BasicTransform base;
			glm::vec3 boxSize = glm::vec3(1.0f);
		};

		inline glm::mat4 calcTrs(BoxTransform const& t)
		{
			return be::pink::calcTrs(t.base.translation, t.base.rotation, t.base.scale * t.boxSize);
		}
	}
}
