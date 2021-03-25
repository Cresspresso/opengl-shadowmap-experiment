
#pragma once

#include <cassert>
#include <glm/glm.hpp>

#include "be/need.hpp"
#include "be/gl.hpp"

namespace be
{
	namespace pink
	{
		class UnlitShader
		{
		private:
			be::gl::ShaderProgram m_shader{};
			struct UniformLocations {
				GLuint mvp;
				GLuint tex;
				GLuint color;
			} m_uniformLocations{};

		public:
			UnlitShader();
			GLuint program() const { return m_shader.program.get(); }
			UniformLocations const& uniformLocations() const { return m_uniformLocations; }
		};

		struct RenderUnlitInfo
		{
			need_ref<UnlitShader const> shader;
			need_ref<be::gl::BasicMesh const> mesh;
			need<GLuint> tex;
			need_ref<glm::vec4 const> color;
			need_ref<glm::mat4 const> mvp;
		};
		void renderUnlit(RenderUnlitInfo const& info);

		struct RenderUnlitLegacyInfo
		{
			UnlitShader const* shader{};
			be::gl::BasicMesh const* mesh{};
			GLuint tex{};
			glm::vec4 const* color{};
			glm::mat4 const* mvp{};
		};
		inline void renderUnlitLegacy(RenderUnlitLegacyInfo const& info)
		{
			assert(info.shader);
			assert(info.mesh);
			assert(info.color);
			assert(info.mvp);

			renderUnlit(RenderUnlitInfo{
				*info.shader,
				*info.mesh,
				info.tex,
				*info.color,
				*info.mvp
				});
		}
	}
}
