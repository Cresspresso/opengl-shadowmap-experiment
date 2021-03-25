
#include "assets.hpp"
#include "shadow_scene.hpp"

namespace example
{
	ShadowScene::ShadowScene(CreateInfo const& info)
	{
		camera.ortho = false;
		camera.fovY = glm::radians(30.0f);
		camera.aspect = 1920.0f / 1080.0f;


		{
			depthMapFrameBuffer = be::mem::gl::makeFrameBuffer();
			depthMapHeight = 1024;
			depthMapWidth = depthMapHeight;
			depthMapTexture = be::mem::gl::makeTexture();

			BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, depthMapTexture.get(), GL_TEXTURE0);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				depthMapWidth, depthMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

			BE_BIND_FRAMEBUFFER_SCOPE(GL_FRAMEBUFFER, depthMapFrameBuffer.get());
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture.get(), 0);
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);

			GLenum const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				throw std::runtime_error("[example] framebuffer exception: "
					+ std::to_string(static_cast<int>(status)));
			}
		}


		//lightPos = be::quatFromEulerDeg({ 90, 0, 0 }) * glm::vec3(0.0f, 1.0f, 0.0f) * 10.0f;
		light.target = glm::vec3(0.0f, 0.0f, 0.0f);
		light.position = glm::vec3(0.0f, 6.0f, 20.0f);
		light.up = glm::vec3(0.0f, 1.0f, 0.0f);
		light.ortho = true;
		light.extentY = 8.0f;
		light.aspect = static_cast<float>(depthMapWidth) / static_cast<float>(depthMapHeight);
		light.nearClip = 0.1f;
		light.farClip = 100.0f;
		//light.nearClip = 0.9f;
		//light.farClip = 3.0f;


		groundTransform.base.rotation = glm::quat(glm::radians(glm::vec3(-90.0f, 0, 0)));
		groundTransform.base.translation = glm::vec3(0, -1.0f, 0);
		groundTransform.base.scale = 100.0f;
		//groundTransform.base.translation = glm::vec3(0, 0, -1.0f);
		groundUVScale = glm::vec2(1.0f * 5.0f);


		flag1.base.translation = groundTransform.base.translation;
		flag1.base.scale = 1.0f;

		flag2.base.translation = glm::vec3(-10, 2, 0);
		flag2.base.rotation = be::pink::quatFromEulerDeg({ 0, 90, 0 });
		flag2.quadSize = glm::vec2(1.0f, 5.0f);


		picketFenceTransform.translation = groundTransform.base.translation;
		//picketFenceTransform.rotation = be::quatFromEulerDeg({ 90, 0, 0 });


		labelText = "Alt+F4\nF11\nRMB+Drag\n\tWASD/Arrows\nP\nG";
		labelScale = glm::vec2(1.0f);
		labelColor = glm::vec4(glm::vec3(0.85f), 1.0f);


		popSound = be::mem::fmod::createSound(info.audio.get(),
			(assets::basicAssetsFolder / "audio/bubble.wav").string().c_str(),
			FMOD_DEFAULT,
			nullptr
			);
	}

	void ShadowScene::update(UpdateInfo const& info)
	{
		auto const& windowSize = info.windowSize.get();


		// update camera
		{
			camera.aspect = info.windowAspect.get();

			if (info.isFullScreen || be::input::isDownAtAll(
				be::input::getElseConsiderUp(
					info.input.get().mouseButtons,
					GLUT_RIGHT_BUTTON)))
			{
				auto const delta = info.previousMousePositionInWindow.get() - info.mousePositionInWindow.get();
				//if (glm::length(glm::vec2(delta)) > 0.001f)
				//{
				//	printf_s("dx=%d, dy=%d\n", delta.x, delta.y);
				//}

				cameraEulerAngles.y += static_cast<float>(delta.x) * 0.005f;
				cameraEulerAngles.x += static_cast<float>(delta.y) * 0.005f;
				cameraEulerAngles.x = std::clamp(cameraEulerAngles.x, glm::radians(-89.999f), glm::radians(89.999f));

				float const r = 10;
				glm::quat const rot = glm::quat(cameraEulerAngles);
				camera.position = camera.target + (rot * glm::vec3(0, 0, r));
				camera.up = rot * glm::vec3(0, 1, 0);
			}
		}


		// poll input
		{
			auto const isGoingDown = [&](unsigned char k) {
				return be::input::isGoingDown(
					be::input::getElseConsiderUp(
						info.input.get().keyboardKeys,
						k));
			};
			auto const isGoingDown_CaseInsensitive = [&isGoingDown](unsigned char lowercase) {
				return isGoingDown(lowercase)
					|| isGoingDown(std::toupper(lowercase));
			};
			if (isGoingDown_CaseInsensitive('a'))
			{
				printf_s("Key 'a' is going down!\n");
			}

			if (isGoingDown_CaseInsensitive('p'))
			{
				be::mem::fmod::require_ok(info.audio.get().playSound(
					popSound.get(),
					nullptr, false, nullptr
				), "[example] FMOD::System::playSound() failed");
			}
		}


		{
			auto const isDown = [&](unsigned char k) {
				return be::input::isDownAtAll(
					be::input::getElseConsiderUp(
						info.input.get().keyboardKeys,
						k));
			};
			auto const isDown_CaseInsensitive = [&isDown](unsigned char lowercase) {
				return isDown(lowercase)
					|| isDown(std::toupper(lowercase));
			};
			auto const isArrowDown = [&](int k) {
				return be::input::isDownAtAll(
					be::input::getElseConsiderUp(
						info.input.get().specialKeys,
						k));
			};

			if (isDown_CaseInsensitive('a') || isArrowDown(GLUT_KEY_LEFT))
			{
				light.position.x -= 0.05f;
			}
			if (isDown_CaseInsensitive('d') || isArrowDown(GLUT_KEY_RIGHT))
			{
				light.position.x += 0.05f;
			}
			if (isDown_CaseInsensitive('s') || isArrowDown(GLUT_KEY_DOWN))
			{
				light.position.y -= 0.05f;
			}
			if (isDown_CaseInsensitive('w') || isArrowDown(GLUT_KEY_UP))
			{
				light.position.y += 0.05f;
			}
			//light.target = light.position + glm::vec3(0.0f, 0.0f, -1.0f);

			if (isDown_CaseInsensitive('g'))
			{
				printf_s("moving window!\n");
				glutPositionWindow(30, 30);
			}
		}


		/*{
			groundTransform.base.translation = light.position + glm::vec3(0.0f, 0.0f, -2.0f);
			groundTransform.quadSize.y = light.extentY * 1.8f;
			groundTransform.quadSize.x = light.extentY * light.aspect * 1.8f;
		}*/


		{
			hudCamera.ortho = true;
			hudCamera.extentY = windowSize.y * 0.5f;
			hudCamera.aspect = info.windowAspect.get();
			be::pink::recalc(hudCamera);
		}


		{
			auto const isDown = [&](int k) {
				return be::input::isDownAtAll(
					be::input::getElseConsiderUp(
						info.input.get().specialKeys,
						k));
			};
			if (isDown(GLUT_KEY_LEFT))
			{
				depthMapQuadTransform.base.translation.x -= 0.01f;
			}
			if (isDown(GLUT_KEY_RIGHT))
			{
				depthMapQuadTransform.base.translation.x += 0.01f;
			}
			if (isDown(GLUT_KEY_DOWN))
			{
				depthMapQuadTransform.base.translation.y -= 0.01f;
			}
			if (isDown(GLUT_KEY_UP))
			{
				depthMapQuadTransform.base.translation.y += 0.01f;
			}
		}


		{
			depthMapQuadTransform.quadSize.y = windowSize.y * (3.0f / 12.0f);
			depthMapQuadTransform.quadSize.x = depthMapQuadTransform.quadSize.y * light.aspect;

			depthMapQuadTransform.base.translation = glm::vec3(
				windowSize.x * 0.5f - depthMapQuadTransform.quadSize.x * 0.5f,
				windowSize.y * 0.5f - depthMapQuadTransform.quadSize.y * 0.5f,
				0.0f
			);
		}


		{
			auto const num_lines = std::count(labelText.begin(), labelText.end(), '\n');
			glm::vec2 const labelPosBL = glm::vec2(
				10.0f,
				15.0f + info.lineHeight.get() * static_cast<float>(num_lines)
			);
			labelTransform.translation = glm::vec3(
				-0.5f * windowSize.x + labelPosBL.x,
				-0.5f * windowSize.y + labelPosBL.y,
				0.0f
			);
		}
	}

	void ShadowScene::render(RenderInfo const& info)
	{
		auto const& windowSize = info.windowSize.get();
		auto const& shadowShader = info.shadowShader.get();
		auto const& quadMesh = info.quadMesh.get();
		auto const& picketFenceModel = info.picketFenceModel.get();


		be::pink::recalc(camera);
		be::pink::recalc(light);


		// 1. first render to depth map
		try
		{
			BE_BIND_FRAMEBUFFER_SCOPE(GL_FRAMEBUFFER, depthMapFrameBuffer.get());
			glViewport(0, 0, depthMapWidth, depthMapHeight);
			glClear(GL_DEPTH_BUFFER_BIT);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			CRESS_MOO_DEFER_EXPRESSION(glDisable(GL_DEPTH_TEST));

			BE_USE_PROGRAM_SCOPE(shadowShader.program());

			example::drawDepth(shadowShader, quadMesh, light.vp * be::pink::calcTrs(flag1));
			example::drawDepth(shadowShader, quadMesh, light.vp * be::pink::calcTrs(flag2));
			example::drawDepth(shadowShader, quadMesh, light.vp * be::pink::calcTrs(groundTransform));
			example::drawModelDepth(shadowShader, picketFenceModel, light.vp, be::pink::calcTrs(picketFenceTransform));
		}
		catch (...) { be::Application::logException(); }


		// 2. then render scene as normal with shadow mapping (using depth map)
		{
			// (note: framebuffer 0 implicitly bound)
			glViewport(0, 0, windowSize.x, windowSize.y);

			glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
			glStencilMask(~static_cast<GLuint>(0U));
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


			// SCENE

			try
			{
				glDisable(GL_DEPTH_TEST);

				be::pink::renderSkybox({
					.shader = info.skyboxShader,
					.mesh = info.skyboxMesh,
					.cubemap = info.skyboxCubemap,
					.cameraProjectionMatrix = camera.projection,
					.cameraViewMatrix = camera.view,
					.scale = 1.0f
					});



				BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, depthMapTexture.get(), GL_TEXTURE9);
				GLint const shadowMapSlotIndex = 9;

				glEnable(GL_DEPTH_TEST);
				CRESS_MOO_DEFER_EXPRESSION(glDisable(GL_DEPTH_TEST));
				glDepthFunc(GL_LESS);

				{
					auto const color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
					auto const mvp = camera.vp * be::pink::calcTrs(flag1);
					be::pink::renderUnlit({
						.shader = info.unlitShader.get(),
						.mesh = quadMesh,
						.tex = info.flagTexture.get(),
						.color = color,
						.mvp = mvp,
						});
				}

				{
					auto const color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
					auto const mvp = camera.vp * be::pink::calcTrs(flag2);
					be::pink::renderUnlit({
						.shader = info.unlitShader.get(),
						.mesh = quadMesh,
						.tex = info.flagTexture.get(),
						.color = color,
						.mvp = mvp,
						});
				}

				example::renderPicketFence(
					info.picketFenceShader.get(),
					picketFenceModel,
					camera,
					light.position,
					light.vp,
					shadowMapSlotIndex,
					be::pink::calcTrs(picketFenceTransform)
				);

				example::renderGround(
					info.groundShader.get(),
					quadMesh,
					info.groundTexture.get(),
					camera,
					light.target - light.position,
					light.vp,
					shadowMapSlotIndex,
					calcTrs(groundTransform),
					groundUVScale,
					light.farClip - 0.001f
				);

				example::renderLightGizmo(
					info.lightGizmoShader.get(),
					info.cubeMesh.get(),
					glm::vec3(1.0f, 1.0f, 0.0f),
					camera.vp * be::pink::calcTrs(light.position, glm::quat(), 0.3f)
				);
			}
			catch (...) { be::Application::logException(); }


			// HUD

			try
			{
				example::renderDepthMapQuad(
					info.depthMapQuadShader.get(),
					quadMesh,
					hudCamera.vp * be::pink::calcTrs(depthMapQuadTransform),
					depthMapTexture.get());

				{
					glm::mat4 const mvp = hudCamera.vp * be::pink::calcTrs(labelTransform);

					glm::mat4 const mvpDropshadow = mvp * glm::translate(glm::vec3(-1.0f, -1.0f, 0.0f));
					glm::vec4 const colorDropshadow = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

					be::pink::text_label::RenderTextLabelInfo in = {
						.shader = info.textLabelShader,
						.mesh = info.textGlyphMesh,
						.font = info.font,
						.lineHeight = info.lineHeight,
						.tabWidth = info.tabWidth,
						.mvp = mvpDropshadow,
						.color = colorDropshadow,
						.scale = labelScale,
						.text = labelText,
					};
					be::pink::text_label::renderTextLabel(in);

					in.mvp = mvp;
					in.color = labelColor;
					be::pink::text_label::renderTextLabel(in);
				}
			}
			catch (...) { be::Application::logException(); }
		}
	}
}
