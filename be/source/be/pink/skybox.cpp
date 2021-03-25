
#include <glm/gtx/transform.hpp>

#include "be/uniform.hpp"
#include "be/pink/skybox.hpp"

namespace be
{
	namespace pink
	{
		SkyboxShader::SkyboxShader()
		{
			char const* const vertexShader = R"__(
#version 330 core
in vec3 p;
out vec3 d;
uniform mat4 vp;
uniform float scale;
void main()
{
	gl_Position = vp * vec4(p * scale, 1);
	d = p;
}
)__";
			char const* const fragmentShader = R"__(
#version 330 core
in vec3 d;
out vec4 r;
uniform samplerCube cubemap;
void main()
{
	r = texture(cubemap, d);
}
)__";
			m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "skybox.cpp");
			GLuint const program = m_shader.program.get();
			m_uniformLoc_vp = glGetUniformLocation(program, "vp");
			m_uniformLoc_scale = glGetUniformLocation(program, "scale");
			m_uniformLoc_cubemap = glGetUniformLocation(program, "cubemap");

			BE_USE_PROGRAM_SCOPE(program);
			glUniform1i(m_uniformLoc_cubemap, 0);
		}

		SkyboxMesh makeSkyboxMesh()
		{
			GLfloat const skyboxVertices[108] =
			{
				// positions          
				-1.0f, 1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, -1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, -1.0f, 1.0f,
				-1.0f, -1.0f, 1.0f,

				-1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, -1.0f,
				1.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, 1.0f,
				-1.0f, 1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, 1.0f,
				1.0f, -1.0f, 1.0f
			};

			auto vertexArray = be::mem::gl::makeVertexArray();
			BE_BIND_VERTEX_ARRAY_SCOPE(vertexArray.get());

			auto vertexBuffer = be::mem::gl::makeBuffer();
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.get());
			glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices[0], GL_STATIC_DRAW);

			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), static_cast<GLvoid*>(0));

			return SkyboxMesh(std::move(vertexArray), std::move(vertexBuffer));
		}

		void renderSkybox(RenderSkyboxInfo const& info)
		{
			SkyboxShader const& shader = info.shader.get();

			glm::mat4 const vp =
				info.cameraProjectionMatrix.get()
				* glm::mat4(glm::mat3(info.cameraViewMatrix.get()));

			BE_USE_PROGRAM_SCOPE(shader.program());

			be::gl::uniformMat4(shader.uniformLoc_vp(), vp);
			glUniform1f(shader.uniformLoc_scale(), info.scale);

			BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_CUBE_MAP, info.cubemap.get(), GL_TEXTURE0);
			//glUniform1i(shader.uniformLoc_cubemap(), 0);

			GLboolean const b = glIsEnabled(GL_DEPTH_TEST);
			glDisable(GL_DEPTH_TEST);
			CRESS_MOO_DEFER_CALLABLE([b]() noexcept { if (b) { glEnable(GL_DEPTH_TEST); } });

			auto const& [vertexArray, vertexBuffer] = info.mesh.get();
			BE_BIND_VERTEX_ARRAY_SCOPE(vertexArray.get());

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
	}
}
