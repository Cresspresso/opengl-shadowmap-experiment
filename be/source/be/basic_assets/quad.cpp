
#include "be/basic_assets/quad.hpp"

namespace be
{
	namespace basic_assets
	{
		namespace meshes
		{
			be::gl::BasicMesh makeQuadMesh()
			{
				using Vertex = be::gl::BasicVertex;

				float const h = 0.5f;
				Vertex const vertices[4] = { Vertex
				{ { -h, -h, 0 }, { 0, 0, 1 }, { 0, 1 } },
				{ { +h, -h, 0 }, { 0, 0, 1 }, { 1, 1 } },
				{ { +h, +h, 0 }, { 0, 0, 1 }, { 1, 0 } },
				{ { -h, +h, 0 }, { 0, 0, 1 }, { 0, 0 } },
				};

				GLuint const indices[3 * 2] = {
					0, 1, 2,
					2, 3, 0,
				};

				return be::gl::makeBasicMesh(vertices, indices);
			}
		}
	}
}
