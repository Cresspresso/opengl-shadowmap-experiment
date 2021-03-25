/*
//	be/gl
//	Graphics and Rendering
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

#include <vector>
#include <stdexcept>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "be/mem/gl.hpp"

namespace be
{
	namespace gl
	{
		// SHADER PROGRAM

		struct ShaderProgram
		{
			std::vector<mem::gl::Shader> shaders;
			mem::gl::Program program;
		};

		class ShaderCompilerException final : public std::runtime_error
		{
		public:
			explicit ShaderCompilerException(std::string const& msg)
				: std::runtime_error("[be::gl] shader compiler exception: " + msg)
			{}
		};

		class ProgramLinkerException final : public std::runtime_error
		{
		public:
			explicit ProgramLinkerException(std::string const& msg)
				: std::runtime_error("[be::gl] program linker exception: " + msg)
			{}
		};

		mem::gl::Shader makeShader(
			GLenum type,
			char const* source,
			char const* loggingName
		);

		ShaderProgram makeShaderProgram(
			std::vector<mem::gl::Shader> shaders,
			char const* loggingName
		);

		ShaderProgram makeBasicShaderProgram(
			char const* vertexSource,
			char const* fragmentSource,
			std::string const& loggingName
		);



		// BASIC MESH

		struct BasicMesh
		{
			mem::gl::VertexArray vertexArray;
			mem::gl::Buffer vertexBuffer;
			mem::gl::Buffer elementBuffer;
			GLuint count{};
			GLenum mode = GL_TRIANGLES;
		};

		struct BasicVertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec2 texCoords;
		};

		void drawBasicMesh(BasicMesh const& mesh);

		BasicMesh makeBasicMesh(
			GLsizeiptr verticesSize,
			GLvoid const* verticesData,
			GLsizeiptr indicesSize,
			GLvoid const* indicesData,
			GLuint indicesCount
		);

		inline BasicMesh makeBasicMesh(
			std::vector<BasicVertex> const& vertices,
			std::vector<GLuint> const& indices)
		{
			return makeBasicMesh(
				vertices.size() * sizeof(vertices[0]),
				&vertices[0],
				indices.size() * sizeof(indices[0]),
				&indices[0],
				indices.size()
			);
		}

		template<size_t NV, size_t NI>
		inline BasicMesh makeBasicMesh(
			BasicVertex const (&vertices)[NV],
			GLuint const (&indices)[NI])
		{
			return makeBasicMesh(
				sizeof(vertices),
				&vertices[0],
				sizeof(indices),
				&indices[0],
				sizeof(indices) / sizeof(indices[0])
			);
		}
	}
}
