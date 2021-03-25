
#pragma once

#include <be/be.hpp>

namespace example
{
	class ShadowShader
	{
	private:
		be::gl::ShaderProgram m_shader{};
		GLuint m_uniformLoc_mvp{};

	public:
		ShadowShader();

		GLuint program() const { return m_shader.program.get(); }
		GLuint uniformLoc_mvp() const { return m_uniformLoc_mvp; }
	};

	void drawDepth(
		ShadowShader const& shader,
		be::gl::BasicMesh const& mesh,
		glm::mat4 const& lightSpaceMvp
	);

	void drawModelDepth(
		ShadowShader const& shader,
		be::pink::model::Model const& model,
		glm::mat4 const& lightSpaceMatrix,
		glm::mat4 const& parentModelMatrix
	);
}
