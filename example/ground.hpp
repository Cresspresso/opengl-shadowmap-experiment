
#pragma once

#include <be/be.hpp>

namespace example
{
	class GroundShader
	{
	private:
		be::gl::ShaderProgram m_shader{};
		struct UniformLocations {
			GLuint mvp;
			GLuint model;
			GLuint fixNormals;
			GLuint lightMvp;
			GLuint shadowMap;
			GLuint lightDir;
			GLuint viewPos;
			GLuint diffuseTexture;
			GLuint uvScale;
			GLuint maxShadowDistance;
		} m_uniformLocations{};

	public:
		GroundShader();
		GLuint program() const { return m_shader.program.get(); }
		UniformLocations const& uniformLocations() const { return m_uniformLocations; }
	};

	be::mem::gl::Texture loadGroundTexture();

	void renderGround(
		GroundShader const& shader,
		be::gl::BasicMesh const& mesh,
		GLuint const tex,
		be::pink::Camera const& camera,
		glm::vec3 const& lightDir,
		glm::mat4 const& lightSpaceMatrix,
		GLint const shadowMapSlotIndex,
		glm::mat4 const& modelMatrix,
		glm::vec2 const& uvScale,
		GLfloat const maxShadowDistance
	);
}
