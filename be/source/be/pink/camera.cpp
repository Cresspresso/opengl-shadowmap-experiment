
#include <glm/gtc/matrix_transform.hpp>

#include "be/pink/camera.hpp"

namespace be
{
	namespace pink
	{
		glm::mat4 calcView(Camera const& camera)
		{
			return glm::lookAt(camera.position, camera.target, camera.up);
		}

		glm::mat4 calcProjection(Camera const& camera)
		{
			if (camera.ortho)
			{
				float const extentX = camera.extentY * camera.aspect;
				return glm::ortho<float>(
					-extentX, extentX,
					-camera.extentY, camera.extentY,
					camera.nearClip, camera.farClip
					);
			}
			else
			{
				return glm::perspective<float>(
					camera.fovY, camera.aspect,
					camera.nearClip, camera.farClip
					);
			}
		}

		void recalc(Camera& camera)
		{
			recalcView(camera);
			recalcProjection(camera);
			recalcVP(camera);
		}
	}
}
