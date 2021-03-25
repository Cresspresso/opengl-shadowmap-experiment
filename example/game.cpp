
#include "assets.hpp"
#include "game.hpp"

namespace example
{
	Game::Game()
	{
		screenSize.x = glutGet(GLUT_SCREEN_WIDTH);
		screenSize.y = glutGet(GLUT_SCREEN_HEIGHT);


		skyboxCubemap = be::basic_assets::textures::loadSkyboxCubemap(assets::basicAssetsFolder);
		skyboxMesh = be::pink::makeSkyboxMesh();


		quadMesh = be::basic_assets::meshes::makeQuadMesh();
		cubeMesh = be::basic_assets::meshes::makeCubeMesh();


		groundTexture = example::loadGroundTexture();
		flagTexture = be::basic_assets::textures::loadFlagTexture(assets::basicAssetsFolder);


		picketFenceModel = example::loadPicketFenceModel();


		textGlyphMesh = be::pink::text_label::makeTextGlyphMesh();
		FT_UInt const fontSize = 24;
		arialFont = be::basic_assets::fonts::loadArialFont(assets::basicAssetsFolder, fontSize, 0);
		lineHeight = static_cast<float>(fontSize) * 1.5f;
		tabWidth = static_cast<float>(4 * arialFont.at(' ').advance);


		audio = be::mem::fmod::System_Create_init(
			30,
			FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED,
			nullptr
		);

		shadowScene.emplace(typename ShadowScene::CreateInfo{
			.audio = *audio
			});

		this->onWindowSizeChanged(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
		this->update();
	}

	void Game::Update()
	{
		try
		{
#if 1
			shadowScene->update({
				.input = input,
				.mousePositionInWindow = mousePositionInWindow,
				.previousMousePositionInWindow = previousMousePositionInWindow,
				//.screenSize = screenSize,
				//.windowPosition = windowPosition,
				.windowSize = windowSize,
				.windowAspect = windowAspect,

				.isFullScreen = isFullScreen,

				//.skyboxShader = skyboxShader,
				//.skyboxMesh = skyboxMesh,
				//.skyboxCubemap = skyboxCubemap.get(),

				//.shadowShader = shadowShader,

				//.lightGizmoShader = lightGizmoShader,

				//.quadMesh = quadMesh,
				//.cubeMesh = cubeMesh,

				//.depthMapQuadShader = depthMapQuadShader,

				//.groundShader = groundShader,
				//.groundTexture = groundTexture.get(),

				//.unlitShader = unlitShader,
				//.flagTexture = flagTexture.get(),

				//.picketFenceShader = picketFenceShader,
				//.picketFenceModel = picketFenceModel,

				//.textLabelShader = textLabelShader,
				//.textGlyphMesh = textGlyphMesh,
				//.arial48Font = arial48Font,
				.lineHeight = lineHeight,
				//.tabWidth = tabWidth,

				.audio = *audio
				});
#else
			waterScene.update({
				.windowSize = windowSize,
				.windowAspect = windowAspect,
				});
#endif 1

			be::mem::fmod::require_ok(audio->update(), "[example] FMOD::System::update failed");

		}
		catch (...) { be::Application::logException(); }



		// update input

		if (isFullScreen)
		{
			glm::ivec2 const v = glm::ivec2(screenSize.x / 2, screenSize.y / 2);
			previousMousePositionInWindow = v;
			glutWarpPointer(v.x, v.y);
		}
		else
		{
			previousMousePositionInWindow = mousePositionInWindow;
		}

		be::input::afterUpdate(this->input);
	}

	void Game::Render()
	{
#if 1
		shadowScene->render({
			//.input = input,
			//.mousePositionInWindow = mousePositionInWindow,
			//.previousMousePositionInWindow = previousMousePositionInWindow,
			//.screenSize = screenSize,
			//.windowPosition = windowPosition,
			.windowSize = windowSize,
			//.windowAspect = windowAspect,

			//.isFullScreen = isFullScreen,

			.skyboxShader = skyboxShader,
			.skyboxMesh = skyboxMesh,
			.skyboxCubemap = skyboxCubemap.get(),

			.shadowShader = shadowShader,

			.lightGizmoShader = lightGizmoShader,

			.quadMesh = quadMesh,
			.cubeMesh = cubeMesh,

			.depthMapQuadShader = depthMapQuadShader,

			.groundShader = groundShader,
			.groundTexture = groundTexture.get(),

			.unlitShader = unlitShader,
			.flagTexture = flagTexture.get(),

			.picketFenceShader = picketFenceShader,
			.picketFenceModel = picketFenceModel,

			.textLabelShader = textLabelShader,
			.textGlyphMesh = textGlyphMesh,
			.font = arialFont,
			.lineHeight = lineHeight,
			.tabWidth = tabWidth,
			});
#else
		waterScene.render({
			.windowSize = windowSize,
			.windowAspect = windowAspect,

			.quadMesh = quadMesh,
			.unlitShader = unlitShader,
			.flagTexture = flagTexture.get(),

			.skyboxShader = skyboxShader,
			.skyboxMesh = skyboxMesh,
			.skyboxCubemap = skyboxCubemap.get(),

			.waterShader = waterShader,
			.waterTexture = flagTexture.get(),
			});
#endif 1
	}

	void Game::OnMousePositionInWindowChanged(int x, int y)
	{
		mousePositionInWindow.x = x;
		mousePositionInWindow.y = y;
	}

	void Game::OnKeyGoingDown(unsigned char key)
	{
		be::input::informGoingDown(input.keyboardKeys, key);
		printf_s("Key %c is going down!\n", key);
	}

	void Game::OnKeyGoingUp(unsigned char key)
	{
		be::input::informGoingUp(input.keyboardKeys, key);
		printf_s("Key %c is going up!\n", key);
	}

	void Game::OnSpecialGoingDown(int keycode)
	{
		be::input::informGoingDown(input.specialKeys, keycode);

		if (be::InputState::GoingDown == input.specialKeys[keycode])
		{
			if (keycode == GLUT_KEY_F11)
			{
				isFullScreen = !isFullScreen;
				if (isFullScreen) {
					glutFullScreen();
					glutSetCursor(GLUT_CURSOR_NONE);
				}
				else {
					int const w = 1920 / 2;
					int const h = 1080 / 2;
					glutPositionWindow((screenSize.x - w) / 2, (screenSize.y - h) / 2);
					glutReshapeWindow(w, h);
					glutLeaveFullScreen();
					glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
				}
			}
			else if (keycode == GLUT_KEY_F4)
			{
				if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0)
				{
					be::Application::exit();
				}
			}
		}
	}

	void Game::OnSpecialGoingUp(int keycode)
	{
		be::input::informGoingUp(input.specialKeys, keycode);
	}

	void Game::OnMouseButtonGoingDown(int button)
	{
		be::input::informGoingDown(input.mouseButtons, button);
	}

	void Game::OnMouseButtonGoingUp(int button)
	{
		be::input::informGoingUp(input.mouseButtons, button);
	}

	void Game::OnMouseWheelPositive(int wheel)
	{
		printf_s("wheel + #%d\n", wheel);
	}

	void Game::OnMouseWheelNegative(int wheel)
	{
		printf_s("wheel - #%d\n", wheel);
	}

	//void Game::OnMouseMoveWhileAnyDown()
	//{
	//	//printf_s("mouse move while down\n");
	//}

	//void Game::OnMouseMoveWhileAllUp()
	//{
	//	//printf_s("mouse move while up\n");
	//}

	void Game::OnMouseEnteredWindow()
	{
		printf_s("Entered\n");
	}

	void Game::OnMouseLeftWindow()
	{
		printf_s("Left\n");
	}

	void Game::OnWindowPositionChanged(int x, int y)
	{
		windowPosition.x = x;
		windowPosition.y = y;
	}

	void Game::OnWindowSizeChanged(int width, int height)
	{
		windowSize.x = width;
		windowSize.y = height;
		windowAspect = static_cast<float>(windowSize.x) / static_cast<float>(windowSize.y);
	}
}
