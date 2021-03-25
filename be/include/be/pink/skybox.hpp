
#pragma once

#include <cassert>

#include "be/gl.hpp"
#include "be/need.hpp"

#include "camera.hpp"

namespace be
{
	namespace pink
	{
		class SkyboxShader
		{
		private:
			be::gl::ShaderProgram m_shader{};
			GLuint m_uniformLoc_vp{};
			GLuint m_uniformLoc_scale{};
			GLuint m_uniformLoc_cubemap{};

		public:
			SkyboxShader();

			GLuint program() const { return m_shader.program.get(); }
			GLuint uniformLoc_vp() const { return m_uniformLoc_vp; }
			GLuint uniformLoc_scale() const { return m_uniformLoc_scale; }
			GLuint uniformLoc_cubemap() const { return m_uniformLoc_cubemap; }
		};

		using SkyboxMesh = std::pair<be::mem::gl::VertexArray, be::mem::gl::Buffer>;
		SkyboxMesh makeSkyboxMesh();

		struct RenderSkyboxInfo
		{
			need_ref<SkyboxShader const> shader;
			need_ref<SkyboxMesh const> mesh;
			need<GLuint> cubemap;
			need_ref<glm::mat4 const> cameraProjectionMatrix;
			need_ref<glm::mat4 const> cameraViewMatrix;
			float scale = 1.0f;
		};
		void renderSkybox(RenderSkyboxInfo const& info);

		struct RenderSkyboxLegacyInfo
		{
			SkyboxShader const* shader{};
			SkyboxMesh const* mesh{};
			GLuint cubemap{};
			glm::mat4 const* cameraProjectionMatrix{};
			glm::mat4 const* cameraViewMatrix{};
			float scale = 1.0f;
		};
		inline void renderSkyboxLegacy(RenderSkyboxLegacyInfo const& info)
		{
			assert(info.shader);
			assert(info.mesh);
			assert(info.cameraProjectionMatrix);
			assert(info.cameraViewMatrix);

			return renderSkybox(RenderSkyboxInfo{
				*info.shader,
				*info.mesh,
				info.cubemap,
				*info.cameraProjectionMatrix,
				*info.cameraViewMatrix,
				info.scale
				});
		}
	}
}
