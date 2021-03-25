
#pragma once

#include <glew/glew.h>
#include <freeglut/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace be
{
	namespace gl
	{
		inline void uniformVec2(GLuint const uniformLocation, glm::vec2 const& vector)
		{
			glUniform2f(uniformLocation, vector.x, vector.y);
		}
		inline void uniformVec3(GLuint const uniformLocation, glm::vec3 const& vector)
		{
			glUniform3f(uniformLocation, vector.x, vector.y, vector.z);
		}
		inline void uniformVec4(GLuint const uniformLocation, glm::vec4 const& vector)
		{
			glUniform4f(uniformLocation, vector.x, vector.y, vector.z, vector.w);
		}

		inline void uniformMat2(GLuint const uniformLocation, glm::mat2 const& matrix)
		{
			glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		inline void uniformMat3(GLuint const uniformLocation, glm::mat3 const& matrix)
		{
			glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
		}
		inline void uniformMat4(GLuint const uniformLocation, glm::mat4 const& matrix)
		{
			glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
		}
	}
}
