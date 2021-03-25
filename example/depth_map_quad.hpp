
#pragma once

#include <be/be.hpp>

namespace example
{
	class DepthMapQuadShader
	{
	private:
		be::gl::ShaderProgram m_shader{};
		GLuint m_uniformLoc_mvp{};
		GLuint m_uniformLoc_depthMap{};

	public:
		DepthMapQuadShader();

		GLuint program() const { return m_shader.program.get(); }
		GLuint uniformLoc_mvp() const { return m_uniformLoc_mvp; }
		GLuint uniformLoc_depthMap() const { return m_uniformLoc_depthMap; }
	};

	void renderDepthMapQuad(
		DepthMapQuadShader const& shader,
		be::gl::BasicMesh const& mesh,
		glm::mat4 const& mvp,
		GLuint const depthMapTexture
	);
}
