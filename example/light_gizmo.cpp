
#include "light_gizmo.hpp"

namespace example
{
	LightGizmoShader::LightGizmoShader()
	{
		char const* const vertexShader = R"__(
#version 330 core

layout(location = 0) in vec3 inPosition;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(inPosition, 1);
}
)__";
		char const* const fragmentShader = R"__(
#version 330 core

out vec4 outColor;

uniform vec3 ambientColor = vec3(1.0f);

void main()
{
	outColor = vec4(ambientColor, 1.0f);
}
)__";
		m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "light_gizmo.cpp");
		GLuint const program = m_shader.program.get();
		m_uniformLocations.mvp = glGetUniformLocation(program, "mvp");
		m_uniformLocations.ambientColor = glGetUniformLocation(program, "ambientColor");
	}

	void renderLightGizmo(
		LightGizmoShader const& shader,
		be::gl::BasicMesh const& mesh,
		glm::vec3 const& ambientColor,
		glm::mat4 const& mvp
	)
	{
		BE_USE_PROGRAM_SCOPE(shader.program());

		auto const& loc = shader.uniformLocations();
		be::gl::uniformMat4(loc.mvp, mvp);
		be::gl::uniformVec3(loc.ambientColor, ambientColor);

		be::gl::drawBasicMesh(mesh);
	}
}
