
#include "be/uniform.hpp"
#include "be/pink/unlit.hpp"

namespace be
{
	namespace pink
	{
		UnlitShader::UnlitShader()
		{
			char const* const vertexShader = R"__(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

layout(location = 0) out vec3 v2fPosition;
layout(location = 1) out vec3 v2fNormal;
layout(location = 2) out vec2 v2fTexCoords;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(inPosition, 1);
	v2fPosition = gl_Position.xyz;
	v2fNormal = inNormal;
	v2fTexCoords = inTexCoords;
}
)__";
			char const* const fragmentShader = R"__(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 v2fPosition;
layout(location = 1) in vec3 v2fNormal;
layout(location = 2) in vec2 v2fTexCoords;

layout(location = 0) out vec4 outColor;

uniform sampler2D tex;
uniform vec4 color = vec4(1.0f);

void main()
{
	outColor = color * texture(tex, v2fTexCoords);
}
)__";
			m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "UnlitShader");
			GLuint const program = m_shader.program.get();
			m_uniformLocations.mvp = glGetUniformLocation(program, "mvp");
			m_uniformLocations.tex = glGetUniformLocation(program, "tex");
			m_uniformLocations.color = glGetUniformLocation(program, "color");

			BE_USE_PROGRAM_SCOPE(program);
			glUniform1i(m_uniformLocations.tex, 0);
		}

		void renderUnlit(RenderUnlitInfo const& info)
		{
			UnlitShader const& shader = info.shader.get();
			BE_USE_PROGRAM_SCOPE(shader.program());

			auto const& loc = shader.uniformLocations();
			be::gl::uniformMat4(loc.mvp, info.mvp);
			be::gl::uniformVec4(loc.color, info.color);

			BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, info.tex, GL_TEXTURE0);

			be::gl::drawBasicMesh(info.mesh);
		}
	}
}
