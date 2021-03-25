
#include "water_scene.hpp"

namespace example
{
	be::mem::gl::Texture attachColorTextureToFrameBuffer(glm::ivec2 const& size)
	{
		auto texture = be::mem::gl::makeTexture();

		BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, texture.get(), GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.get(), 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		//glReadBuffer(GL_NONE);

		return texture;
	}

	be::mem::gl::Texture attachDepthTextureToFrameBuffer(glm::ivec2 const& size)
	{
		auto texture = be::mem::gl::makeTexture();

		BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, texture.get(), GL_TEXTURE0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32,
			size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture.get(), 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);

		return texture;
	}

	WaterScene::WaterScene()
	{
		camera.position = glm::vec3(0, 2, 10);

		waterQuadTransform.base.scale = 10;
		waterQuadTransform.base.rotation = be::pink::quatFromEulerDeg({ -90, 0, 0 });

		backgroundQuads.push_back({ .base = { .translation = { 0, 0, 0 }, .rotation = {} } });
		backgroundQuads.push_back({ .base = { .translation = { 0, 1, -2 }, .rotation = {} } });
		backgroundQuads.push_back({ .base = { .translation = { -1, 0, -1 }, .rotation = {} } });

		{
			refRactionSize = glm::ivec2(1024, 1024);
			refRactionFrameBuffer = be::mem::gl::makeFrameBuffer();
			BE_BIND_FRAMEBUFFER_SCOPE(GL_FRAMEBUFFER, refRactionFrameBuffer.get());
			refRactionColorAttachment = attachColorTextureToFrameBuffer(refRactionSize);
			//refRactionDepthAttachment = attachDepthTextureToFrameBuffer(refRactionSize);

			GLenum const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				throw std::runtime_error("[example] framebuffer exception: "
					+ std::to_string(static_cast<int>(status)));
			}
		}

		{
			refLectionSize = glm::ivec2(1024, 1024);
			refLectionFrameBuffer = be::mem::gl::makeFrameBuffer();
			BE_BIND_FRAMEBUFFER_SCOPE(GL_FRAMEBUFFER, refLectionFrameBuffer.get());
			refLectionColorAttachment = attachColorTextureToFrameBuffer(refLectionSize);
			refLectionDepthAttachment = attachDepthTextureToFrameBuffer(refLectionSize);

			GLenum const status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
			if (status != GL_FRAMEBUFFER_COMPLETE) {
				throw std::runtime_error("[example] framebuffer exception: "
					+ std::to_string(static_cast<int>(status)));
			}
		}


		guiQuadTransform.quadSize = glm::vec2(refRactionSize);
	}

	void WaterScene::update(UpdateInfo const& info)
	{
		auto const& windowSize = info.windowSize.get();
		auto const& windowAspect = info.windowAspect.get();

		{
			guiCamera.ortho = true;
			guiCamera.extentY = windowSize.y * 0.5f;
			guiCamera.aspect = windowAspect;
			be::pink::recalc(guiCamera);
		}

		{
			guiQuadTransform.base.scale = 0.3f * windowSize.y / guiQuadTransform.quadSize.y;
			guiQuadTransform.base.translation.x = -0.5f * windowSize.x + 0.5f * guiQuadTransform.quadSize.x * guiQuadTransform.base.scale;
			guiQuadTransform.base.translation.y = -0.5f * windowSize.y + 0.5f * guiQuadTransform.quadSize.y * guiQuadTransform.base.scale;
		}
	}

	void WaterScene::renderPass(RenderInfo const& info)
	{
		camera.aspect = info.windowAspect.get();
		be::pink::recalc(camera);

		glClearColor(0.1f, 0.1f, 0.3f, 1.0f);
		glStencilMask(~static_cast<GLuint>(0U));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glDisable(GL_DEPTH_TEST);

		be::pink::renderSkybox({
			.shader = info.skyboxShader,
			.mesh = info.skyboxMesh,
			.cubemap = info.skyboxCubemap,
			.cameraProjectionMatrix = camera.projection,
			.cameraViewMatrix = camera.view,
			.scale = 1.0f,
			});

		glEnable(GL_DEPTH_TEST);
		CRESS_MOO_DEFER_EXPRESSION(glDisable(GL_DEPTH_TEST));
		glDepthFunc(GL_LESS);

		for (auto const& quadTransform : backgroundQuads)
		{
			glm::mat4 const model = be::pink::calcTrs(quadTransform);
			glm::mat4 const mvp = camera.vp * model;
			//glm::mat3 const fixNormals = be::calcFixNormalsMatrix(model);
			glm::vec4 const color = glm::vec4(1.0f);
			be::pink::renderUnlit({
				.shader = info.unlitShader.get(),
				.mesh = info.quadMesh.get(),
				.tex = info.flagTexture.get(),
				.color = color,
				.mvp = mvp,
				});
		}
	}

	void WaterScene::render(RenderInfo const& info)
	{
		auto const& windowSize = info.windowSize.get();
		auto const& quadMesh = info.quadMesh.get();

		glEnable(GL_CLIP_DISTANCE0);
		CRESS_MOO_DEFER_EXPRESSION(glDisable(GL_CLIP_DISTANCE0));

		{
			BE_BIND_FRAMEBUFFER_SCOPE(GL_FRAMEBUFFER, refRactionFrameBuffer.get());
			glViewport(0, 0, refRactionSize.x, refRactionSize.y);

			renderPass(info);
		}

		{
			BE_BIND_FRAMEBUFFER_SCOPE(GL_FRAMEBUFFER, refLectionFrameBuffer.get());
			glViewport(0, 0, refLectionSize.x, refLectionSize.y);

			renderPass(info);
		}

		{
			// (note: framebuffer 0 implicitly bound)
			glViewport(0, 0, windowSize.x, windowSize.y);

			renderPass(info);

			// WATER PASS
			{
				glEnable(GL_DEPTH_TEST);
				CRESS_MOO_DEFER_EXPRESSION(glDisable(GL_DEPTH_TEST));
				glDepthFunc(GL_LESS);

				{
					glm::mat4 const model = be::pink::calcTrs(waterQuadTransform);
					glm::mat4 const mvp = camera.vp * model;
					glm::mat3 const fixNormals = be::pink::calcFixNormalsMatrix(model);
					example::renderWater({
						.shader = info.waterShader,
						.mesh = info.quadMesh,
						.mvp = mvp,
						.fixNormals = fixNormals,
						.diffuseTexture = info.waterTexture
						});
				}
			}

			// GUI PASS
			{
				glm::mat4 const mvp = guiCamera.vp * be::pink::calcTrs(guiQuadTransform);
				glm::vec4 const color = glm::vec4(1.0f);
				be::pink::renderUnlit({
					.shader = info.unlitShader.get(),
					.mesh = quadMesh,
					.tex = refRactionColorAttachment.get(),
					.color = color,
					.mvp = mvp,
					});
			}
		}
	}
}
