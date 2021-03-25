
#include "depth_map_quad.hpp"

namespace example
{
	DepthMapQuadShader::DepthMapQuadShader()
	{
		char const* const vertexShader = R"__(
#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inTexCoords;

out vec2 v2fTexCoords;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(inPosition, 1.0f);
	v2fTexCoords = inTexCoords;
}
)__";
		char const* const fragmentShader = R"__(
#version 330 core

out vec4 outColor;
  
in vec2 v2fTexCoords;

uniform sampler2D depthMap;

void main()
{
	float depthValue = texture(depthMap, vec2(v2fTexCoords.x, 1.0f - v2fTexCoords.y)).r;
	outColor = vec4(vec3(depthValue), 1.0f);
}
)__";
		m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "depth_map_quad.cpp");
		GLuint const program = m_shader.program.get();
		m_uniformLoc_mvp = glGetUniformLocation(program, "mvp");
		m_uniformLoc_depthMap = glGetUniformLocation(program, "depthMap");

		BE_USE_PROGRAM_SCOPE(program);
		glUniform1i(m_uniformLoc_depthMap, 0);
	}

	void renderDepthMapQuad(
		DepthMapQuadShader const& shader,
		be::gl::BasicMesh const& mesh,
		glm::mat4 const& mvp,
		GLuint const depthMapTexture
	)
	{
		BE_USE_PROGRAM_SCOPE(shader.program());
		be::gl::uniformMat4(shader.uniformLoc_mvp(), mvp);
		BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, depthMapTexture, GL_TEXTURE0);
		be::gl::drawBasicMesh(mesh);
	}
}
