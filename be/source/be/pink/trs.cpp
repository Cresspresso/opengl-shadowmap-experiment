
#include "be/pink/trs.hpp"

namespace be
{
	namespace pink
	{
		glm::mat4 calcTrs(
			glm::vec3 const& translation,
			glm::quat const& rotation,
			glm::vec3 const& scale)
		{
			glm::mat4 const T = glm::translate(translation);
			glm::mat4 const R = glm::mat4_cast(rotation);
			glm::mat4 const S = glm::scale(scale);
			return T * R * S;
		}
	}
}
