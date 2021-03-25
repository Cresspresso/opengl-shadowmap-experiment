
#pragma once

#include <be/be.hpp>

#include "picket_fence.hpp"
#include "ground.hpp"
#include "shadow.hpp"
#include "depth_map_quad.hpp"
#include "light_gizmo.hpp"

namespace example
{
	class ShadowScene
	{
	private:
		be::pink::Camera camera;
		glm::vec3 cameraEulerAngles;

		be::mem::gl::FrameBuffer depthMapFrameBuffer;
		int depthMapWidth{}, depthMapHeight{};
		be::mem::gl::Texture depthMapTexture;

		be::pink::Camera light;

		be::pink::Camera hudCamera;

		be::pink::QuadTransform depthMapQuadTransform;

		be::pink::QuadTransform groundTransform;
		glm::vec2 groundUVScale = glm::vec2(1.0f);

		be::pink::QuadTransform flag1;
		be::pink::QuadTransform flag2;

		be::pink::BasicTransform picketFenceTransform;

		std::string labelText;
		be::pink::BasicTransform labelTransform;
		glm::vec2 labelScale;
		glm::vec4 labelColor;

		be::mem::fmod::Sound popSound;

	public:
		ShadowScene() = delete;

		struct CreateInfo
		{
			be::need_ref<FMOD::System> audio;
		};
		ShadowScene(CreateInfo const& info);

		struct UpdateInfo
		{
			be::need_ref<be::Input const> input;
			be::need_ref<glm::ivec2 const> mousePositionInWindow;
			be::need_ref<glm::ivec2 const> previousMousePositionInWindow;

			be::need_ref<glm::ivec2 const> windowSize;
			be::need<float> windowAspect;

			be::need<bool> isFullScreen;

			be::need<float> lineHeight;

			be::need_ref<FMOD::System> audio;
		};
		void update(UpdateInfo const& info);

		struct RenderInfo
		{
			be::need_ref<glm::ivec2 const> windowSize;

			be::need_ref<be::pink::SkyboxShader const> skyboxShader;
			be::need_ref<be::pink::SkyboxMesh const> skyboxMesh;
			be::need<GLuint> skyboxCubemap;

			be::need_ref<ShadowShader const> shadowShader;

			be::need_ref<LightGizmoShader const> lightGizmoShader;

			be::need_ref<be::gl::BasicMesh const> quadMesh;
			be::need_ref<be::gl::BasicMesh const> cubeMesh;

			be::need_ref<DepthMapQuadShader const> depthMapQuadShader;

			be::need_ref<GroundShader const> groundShader;
			be::need<GLuint> groundTexture;

			be::need_ref<be::pink::UnlitShader const> unlitShader;
			be::need<GLuint> flagTexture;

			be::need_ref<PicketFenceShader const> picketFenceShader;
			be::need_ref<be::pink::model::Model const> picketFenceModel;

			be::need_ref<be::pink::text_label::TextLabelShader const> textLabelShader;
			be::need_ref<be::pink::text_label::TextGlyphMesh /* mutable */> textGlyphMesh;

			be::need_ref<be::ft::Font const> font;
			be::need<float> lineHeight;
			be::need<float> tabWidth;
		};
		void render(RenderInfo const& info);
	};
}
