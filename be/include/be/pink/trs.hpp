
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

namespace be
{
	namespace pink
	{
		glm::mat4 calcTrs(
			glm::vec3 const& translation,
			glm::quat const& rotation,
			glm::vec3 const& scale
		);

		inline glm::mat4 calcTrs(
			glm::vec3 const& translation,
			glm::quat const& rotation,
			float const scale)
		{
			return calcTrs(translation, rotation, glm::vec3(scale));
		}

		struct BasicTransform
		{
			glm::vec3 translation = glm::vec3();
			glm::quat rotation = glm::quat();
			float scale = 1.0f;
		};

		inline glm::mat4 calcTrs(BasicTransform const& t)
		{
			return calcTrs(t.translation, t.rotation, t.scale);
		}

		inline glm::quat quatFromEulerDeg(glm::vec3 const& eulerAnglesInDegrees)
		{
			return glm::quat(glm::radians(eulerAnglesInDegrees));
		}

		inline glm::mat3 calcFixNormalsMatrix(glm::mat4 const& modelMatrix)
		{
			return glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
		}
	}
}
