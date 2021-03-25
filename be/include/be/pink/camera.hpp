
#pragma once

#include <glm/glm.hpp>

namespace be
{
	namespace pink
	{
		struct Camera
		{
			glm::vec3 position = glm::vec3(0, 0, 10);
			glm::vec3 target = glm::vec3(0, 0, 0);
			glm::vec3 up = glm::vec3(0, 1, 0);

			bool ortho = false;
			float extentY = 1.0f;
			float fovY = glm::radians(30.0f);
			float aspect = 1.0f;
			float nearClip = 0.1f;
			float farClip = 5000.0f;

			glm::mat4 view;
			glm::mat4 projection;
			glm::mat4 vp;
		};

		glm::mat4 calcView(Camera const& camera);
		glm::mat4 calcProjection(Camera const& camera);

		inline void recalcView(Camera& camera) { camera.view = calcView(camera); }
		inline void recalcProjection(Camera& camera) { camera.projection = calcProjection(camera); }
		inline void recalcVP(Camera& camera) { camera.vp = camera.projection * camera.view; }
		void recalc(Camera& camera);
	}
}
