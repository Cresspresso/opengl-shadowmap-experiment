
#pragma once

#include <be/be.hpp>

namespace example
{
	class LightGizmoShader
	{
	private:
		be::gl::ShaderProgram m_shader{};
		struct UniformLocations {
			GLuint mvp;
			GLuint ambientColor;
		} m_uniformLocations{};

	public:
		LightGizmoShader();
		GLuint program() const { return m_shader.program.get(); }
		UniformLocations const& uniformLocations() const { return m_uniformLocations; }
	};

	void renderLightGizmo(
		LightGizmoShader const& shader,
		be::gl::BasicMesh const& mesh,
		glm::vec3 const& ambientColor,
		glm::mat4 const& mvp
	);
}
