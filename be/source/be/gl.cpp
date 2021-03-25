
#include <algorithm>

#include "be/gl.hpp"

namespace be
{
	namespace gl
	{
		mem::gl::Shader makeShader(
			GLenum type,
			char const* source,
			char const* loggingName)
		{
			auto shader = mem::gl::makeShader(type);
			glShaderSource(shader.get(), 1, &source, nullptr);
			glCompileShader(shader.get());

			GLint status = GL_FALSE;
			glGetShaderiv(shader.get(), GL_COMPILE_STATUS, &status);
			if (status == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader.get(), GL_INFO_LOG_LENGTH, &maxLength);
				maxLength = std::clamp<decltype(maxLength)>(maxLength, 0, 511);
				std::vector<char> buffer(maxLength);
				glGetShaderInfoLog(shader.get(), maxLength, &maxLength, buffer.data());

				std::string msg = "(at ";
				msg += loggingName;
				msg += " ):\n";
				msg.append(buffer.data(), static_cast<size_t>(maxLength));
				throw ShaderCompilerException(msg);
			}

			return shader;
		}

		ShaderProgram makeShaderProgram(
			std::vector<mem::gl::Shader> shaders,
			char const* loggingName)
		{
			std::vector<mem::gl::Shader> attached;
			attached.reserve(shaders.size());

			auto program = mem::gl::makeProgram();

			CRESS_MOO_DEFER_BEGIN(unattach);
			for (auto const& shader : attached)
			{
				glDetachShader(program.get(), shader.get());
			}
			CRESS_MOO_DEFER_END(unattach);

			for (auto& shader : shaders)
			{
				glAttachShader(program.get(), shader.get());
				attached.push_back(std::move(shader));
			}

			glLinkProgram(program.get());

			GLint status = GL_FALSE;
			glGetProgramiv(program.get(), GL_LINK_STATUS, &status);
			if (status == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetProgramiv(program.get(), GL_INFO_LOG_LENGTH, &maxLength);
				maxLength = std::clamp<decltype(maxLength)>(maxLength, 0, 511);
				std::vector<char> buffer(maxLength);
				glGetProgramInfoLog(program.get(), maxLength, &maxLength, buffer.data());

				std::string msg = "(at ";
				msg += loggingName;
				msg += " ):\n";
				msg.append(buffer.data(), static_cast<size_t>(maxLength));
				throw ProgramLinkerException(msg);
			}

			ShaderProgram result;
			result.shaders = std::move(attached);
			result.program = std::move(program);
			return result;
		}

		ShaderProgram makeBasicShaderProgram(
			char const* vertexSource,
			char const* fragmentSource,
			std::string const& loggingName)
		{
			std::vector<be::mem::gl::Shader> shaders;
			shaders.reserve(2);

			shaders.push_back(be::gl::makeShader(
				GL_VERTEX_SHADER,
				vertexSource,
				(loggingName + ": vertex shader").c_str()
			));

			shaders.push_back(be::gl::makeShader(
				GL_FRAGMENT_SHADER,
				fragmentSource,
				(loggingName + ": fragment shader").c_str()
			));

			return be::gl::makeShaderProgram(
				std::move(shaders),
				(loggingName + ": shader linker").c_str()
			);
		}



		// BASIC MESH

		void drawBasicMesh(BasicMesh const& mesh)
		{
			BE_BIND_VERTEX_ARRAY_SCOPE(mesh.vertexArray.get());
			glDrawElements(mesh.mode, mesh.count, GL_UNSIGNED_INT, nullptr);
		}

		BasicMesh makeBasicMesh(
			GLsizeiptr verticesSize,
			GLvoid const* verticesData,
			GLsizeiptr indicesSize,
			GLvoid const* indicesData,
			GLuint indicesCount)
		{
			using Vertex = BasicVertex;

			auto vertexArray = be::mem::gl::makeVertexArray();
			BE_BIND_VERTEX_ARRAY_SCOPE(vertexArray.get());

			auto elementBuffer = be::mem::gl::makeBuffer();
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer.get());
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, indicesData, GL_STATIC_DRAW);

			auto vertexBuffer = be::mem::gl::makeBuffer();
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.get());
			glBufferData(GL_ARRAY_BUFFER, verticesSize, verticesData, GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid const*>(offsetof(Vertex, position)));
			glEnableVertexAttribArray(0);

			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid const*>(offsetof(Vertex, normal)));
			glEnableVertexAttribArray(1);

			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid const*>(offsetof(Vertex, texCoords)));
			glEnableVertexAttribArray(2);

			BasicMesh mesh;
			mesh.vertexArray = std::move(vertexArray);
			mesh.vertexBuffer = std::move(vertexBuffer);
			mesh.elementBuffer = std::move(elementBuffer);
			mesh.count = indicesCount;
			mesh.mode = GL_TRIANGLES;

			return mesh;
		}
	}
}
