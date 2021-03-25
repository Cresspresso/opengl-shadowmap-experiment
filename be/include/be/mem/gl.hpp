/*
//	be/mem/gl
//	Memory safety for OpenGL pointers.
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

#include <glew/glew.h>
#include <freeglut/freeglut.h>
#include <memory>
#include <cress/moo/defer.hpp>
#include "fraii.hpp"

namespace be
{
	namespace mem
	{
		namespace gl
		{
			struct ShaderDeleter { void operator()(GLuint p) { glDeleteShader(p); } };
			using Shader = Fraii<GLuint, ShaderDeleter>;
			inline Shader makeShader(GLenum type) { return Shader(glCreateShader(type)); }



			struct ProgramDeleter { void operator()(GLuint p) { glDeleteProgram(p); } };
			using Program = Fraii<GLuint, ProgramDeleter>;
			inline Program makeProgram() { return Program(glCreateProgram()); }

#define BE_USE_PROGRAM_SCOPE(program)\
	glUseProgram(program);\
	CRESS_MOO_DEFER_EXPRESSION(glUseProgram(0))



			struct VertexArrayDeleter { void operator()(GLuint p) { glDeleteVertexArrays(1, &p); } };
			using VertexArray = Fraii<GLuint, VertexArrayDeleter>;
			inline VertexArray makeVertexArray() { GLuint p; glGenVertexArrays(1, &p); return VertexArray(p); }

#define BE_BIND_VERTEX_ARRAY_SCOPE(vao)\
	glBindVertexArray(vao);\
	CRESS_MOO_DEFER_EXPRESSION(glBindVertexArray(0))



			struct BufferDeleter { void operator()(GLuint p) { glDeleteBuffers(1, &p); } };
			using Buffer = Fraii<GLuint, BufferDeleter>;
			inline Buffer makeBuffer() { GLuint p; glGenBuffers(1, &p); return Buffer(p); }



			struct TextureDeleter { void operator()(GLuint p) { glDeleteTextures(1, &p); } };
			using Texture = Fraii<GLuint, TextureDeleter>;
			inline Texture makeTexture() { GLuint p; glGenTextures(1, &p); return Texture(p); }

#define BE_BIND_TEXTURE_SCOPE(target, texture, unit)\
	glActiveTexture(unit);\
	glBindTexture(target, texture);\
	CRESS_MOO_DEFER_CALLABLE([]()noexcept{\
		glActiveTexture(unit);\
		glBindTexture(target, 0);\
	});\



			struct FrameBufferDeleter { void operator()(GLuint p) { glDeleteFramebuffers(1, &p); } };
			using FrameBuffer = Fraii<GLuint, FrameBufferDeleter>;
			inline FrameBuffer makeFrameBuffer() { GLuint p; glGenFramebuffers(1, &p); return FrameBuffer(p); }

#define BE_BIND_FRAMEBUFFER_SCOPE(target, framebuffer)\
	glBindFramebuffer(target, framebuffer);\
	CRESS_MOO_DEFER_EXPRESSION(glBindFramebuffer(target, 0))



		}
		//~ namespace
	}
}
