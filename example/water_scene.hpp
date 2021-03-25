
#pragma once

#include <be/be.hpp>

#include "water.hpp"

namespace example
{
	be::mem::gl::Texture attachColorTextureToFrameBuffer(glm::ivec2 const& size);
	be::mem::gl::Texture attachDepthTextureToFrameBuffer(glm::ivec2 const& size);

	class WaterScene
	{
	private:
		be::pink::Camera camera;

		std::vector<be::pink::QuadTransform> backgroundQuads;

		be::pink::QuadTransform waterQuadTransform;

		be::mem::gl::FrameBuffer refRactionFrameBuffer;
		glm::ivec2 refRactionSize;
		be::mem::gl::Texture refRactionColorAttachment;
		be::mem::gl::Texture refRactionDepthAttachment;

		be::mem::gl::FrameBuffer refLectionFrameBuffer;
		glm::ivec2 refLectionSize;
		be::mem::gl::Texture refLectionColorAttachment;
		be::mem::gl::Texture refLectionDepthAttachment;

		be::pink::Camera guiCamera;
		be::pink::QuadTransform guiQuadTransform;

	public:
		WaterScene();

		struct UpdateInfo
		{
			be::need_ref<glm::ivec2 const> windowSize;
			be::need<float> windowAspect;
		};
		void update(UpdateInfo const& info);

		struct RenderInfo
		{
			be::need_ref<glm::ivec2 const> windowSize;
			be::need<float> windowAspect;

			be::need_ref<be::gl::BasicMesh const> quadMesh;
			be::need_ref<be::pink::UnlitShader const> unlitShader;
			be::need<GLuint> flagTexture;

			be::need_ref<be::pink::SkyboxShader const> skyboxShader;
			be::need_ref<be::pink::SkyboxMesh const> skyboxMesh;
			be::need<GLuint> skyboxCubemap;

			be::need_ref<WaterShader const> waterShader;
			be::need<GLuint> waterTexture;
		};
		void render(RenderInfo const& info);

	private:
		void renderPass(RenderInfo const& info);
	};
}
