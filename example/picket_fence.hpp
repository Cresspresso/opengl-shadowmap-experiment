
#pragma once

#include <functional>
#include <be/be.hpp>

namespace example
{
	class PicketFenceShader
	{
	private:
		be::gl::ShaderProgram m_shader{};
		struct UniformLocations {
			GLuint mvp;
			GLuint model;
			GLuint fixNormals;
			GLuint lightSpaceMatrix;
			GLuint shadowMap;
			GLuint lightPos;
			GLuint viewPos;
			std::array<GLuint, 4> diffuseTextures;
		} m_uniformLocations{};

	public:
		PicketFenceShader();
		GLuint program() const { return m_shader.program.get(); }
		UniformLocations const& uniformLocations() const { return m_uniformLocations; }
	};
	
	be::pink::model::Model loadPicketFenceModel();

	void renderPicketFence(
		PicketFenceShader const& shader,
		be::pink::model::Model const& model,
		be::pink::Camera const& camera,
		glm::vec3 const& lightPos,
		glm::mat4 const& lightSpaceMatrix,
		GLuint const shadowMapTextureIndex, // e.g. 1 if shadowmap is bound to GL_TEXTURE1
		glm::mat4 const& parentModelMatrix
	);
}
