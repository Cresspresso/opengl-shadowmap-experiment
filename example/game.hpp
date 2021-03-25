/*
CONTROLS
ALT+F4		exits the game
F11			toggles fullscreen
W/A/S/D		move the light source
RMB+Drag	orbit the camera
*/

#pragma once

#include "shadow_scene.hpp"
#include "water_scene.hpp"

namespace example
{
	class Game final : public be::Game
	{
	private:
		// STATE

		be::Input input;
		glm::ivec2 mousePositionInWindow;
		glm::ivec2 previousMousePositionInWindow;

		glm::ivec2 screenSize;
		glm::ivec2 windowPosition;
		glm::ivec2 windowSize;
		float windowAspect{};

		bool isFullScreen = false;


		// RESOURCES

		be::pink::SkyboxShader skyboxShader;
		be::pink::SkyboxMesh skyboxMesh;
		be::mem::gl::Texture skyboxCubemap;

		ShadowShader shadowShader;

		LightGizmoShader lightGizmoShader;

		be::gl::BasicMesh quadMesh;
		be::gl::BasicMesh cubeMesh;

		DepthMapQuadShader depthMapQuadShader;

		GroundShader groundShader;
		be::mem::gl::Texture groundTexture;

		be::pink::UnlitShader unlitShader;
		be::mem::gl::Texture flagTexture;

		PicketFenceShader picketFenceShader;
		be::pink::model::Model picketFenceModel;

		be::pink::text_label::TextLabelShader textLabelShader;
		be::pink::text_label::TextGlyphMesh textGlyphMesh;
		be::ft::Font arialFont;
		float lineHeight{};
		float tabWidth{};

		WaterShader waterShader;

		be::mem::fmod::System audio;


		// SCENES

#if 1
		std::optional<ShadowScene> shadowScene;
#else
		WaterScene waterScene;
#endif 1

	public:
		~Game() noexcept final = default;
		Game();
		Game(Game&&) noexcept = delete;
		Game& operator=(Game&&) noexcept = delete;
		Game(Game const&) = delete;
		Game& operator=(Game const&) = delete;

	private:
		// INTERFACE IMPLEMENTATION
		void Update() final;
		void Render() final;
		void OnMousePositionInWindowChanged(int x, int y) final;
		void OnKeyGoingDown(unsigned char key) final;
		void OnKeyGoingUp(unsigned char key) final;
		void OnSpecialGoingDown(int keycode) final;
		void OnSpecialGoingUp(int keycode) final;
		void OnMouseButtonGoingDown(int button) final;
		void OnMouseButtonGoingUp(int button) final;
		void OnMouseWheelPositive(int wheel) final;
		void OnMouseWheelNegative(int wheel) final;
		//void OnMouseMoveWhileAnyDown() final;
		//void OnMouseMoveWhileAllUp() final;
		void OnMouseEnteredWindow() final;
		void OnMouseLeftWindow() final;
		void OnWindowPositionChanged(int x, int y) final;
		void OnWindowSizeChanged(int width, int height) final;
	};
}
