
#include "shadow.hpp"

namespace example
{
	ShadowShader::ShadowShader()
	{
		char const* const vertexShader = R"__(
#version 330 core
layout (location = 0) in vec3 inPosition;
uniform mat4 mvp;
void main()
{
	gl_Position = mvp * vec4(inPosition, 1.0f);
}
)__";
		char const* const fragmentShader = R"__(
#version 330 core
void main(){}
)__";
		m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "shadow.cpp");
		GLuint const program = m_shader.program.get();
		m_uniformLoc_mvp = glGetUniformLocation(program, "mvp");
	}



	void drawDepth(
		ShadowShader const& shader,
		be::gl::BasicMesh const& mesh,
		glm::mat4 const& lightSpaceMvp
	)
	{
		be::gl::uniformMat4(shader.uniformLoc_mvp(), lightSpaceMvp);
		be::gl::drawBasicMesh(mesh);
	}

	void drawModelDepth(
		ShadowShader const& shader,
		be::pink::model::Model const& model,
		glm::mat4 const& lightSpaceMatrix,
		glm::mat4 const& parentModelMatrix
	)
	{
		be::pink::model::DrawNodeCallback const drawNode = [&](be::pink::model::Node const& node, glm::mat4 const& modelMatrix)
		{
			auto const mvp = lightSpaceMatrix * modelMatrix;
			be::gl::uniformMat4(shader.uniformLoc_mvp(), mvp);

			for (auto const& w : node.meshes)
			{
				if (auto const mesh = w.lock())
				{
					be::gl::drawBasicMesh(mesh->data);
				}
			}
		};

		be::pink::model::renderModel(model, drawNode, parentModelMatrix);
	}
}
