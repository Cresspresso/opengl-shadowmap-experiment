
// SOURCE: https://www.youtube.com/watch?v=HusvGeEDU_U

#pragma once

#include <be/be.hpp>

namespace example
{
	class WaterShader
	{
	private:
		be::gl::ShaderProgram m_shader{};
		struct UniformLocations {
			GLuint mvp;
			GLuint diffuseTexture;
			GLuint fixNormals;
		} m_uniformLocations{};

	public:
		WaterShader();
		GLuint program() const { return m_shader.program.get(); }
		UniformLocations const& uniformLocations() const { return m_uniformLocations; }
	};

	struct RenderWaterInfo
	{
		be::need_ref<WaterShader const> shader;
		be::need_ref<be::gl::BasicMesh const> mesh;
		be::need_ref<glm::mat4 const> mvp;
		be::need_ref<glm::mat3 const> fixNormals;
		be::need<GLuint> diffuseTexture;
	};

	void renderWater(RenderWaterInfo const& info);
}
