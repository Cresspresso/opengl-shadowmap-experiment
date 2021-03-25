
// SOURCE: https://www.youtube.com/watch?v=0NH9k4zTAqk

#include "water.hpp"

namespace example
{
	WaterShader::WaterShader()
	{
		char const* const vertexShader = R"__(
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

out vec3 v2fNormal;
out vec2 v2fTexCoords;

uniform mat4 mvp;
uniform mat3 fixNormals;

void main()
{
	vec4 p = vec4(inPosition, 1.0f);
	gl_Position = mvp * p;
	v2fNormal = fixNormals * inNormal;
	v2fTexCoords = inTexCoords;
}
)__";
		char const* const fragmentShader = R"__(
#version 330 core

out vec4 outColor;

in vec3 v2fNormal;
in vec2 v2fTexCoords;

uniform sampler2D diffuseTexture;

void main()
{
	outColor = vec4(texture(diffuseTexture, v2fTexCoords).rgb, 1.0f);
}
)__";
		m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "WaterShader");
		GLuint const program = m_shader.program.get();
		m_uniformLocations.diffuseTexture = glGetUniformLocation(program, "diffuseTexture");
		m_uniformLocations.mvp = glGetUniformLocation(program, "mvp");
		m_uniformLocations.fixNormals = glGetUniformLocation(program, "fixNormals");

		BE_USE_PROGRAM_SCOPE(program);
		glUniform1i(m_uniformLocations.diffuseTexture, 0);
	}

	void renderWater(RenderWaterInfo const& info)
	{
		auto const& shader = info.shader.get();
		BE_USE_PROGRAM_SCOPE(shader.program());
		auto const& loc = shader.uniformLocations();

		be::gl::uniformMat4(loc.mvp, info.mvp.get());
		be::gl::uniformMat3(loc.fixNormals, info.fixNormals.get());

		BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, info.diffuseTexture.get(), GL_TEXTURE0);

		be::gl::drawBasicMesh(info.mesh.get());
	}
}
