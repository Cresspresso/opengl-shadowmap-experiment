/*
//	be/soil
//	Image loading
//
//	Elijah Shadbolt
//	2019
*/

#pragma once

#include <stdexcept>

#include "be/mem/soil.hpp"
#include "be/mem/gl.hpp"

namespace be
{
	namespace soil
	{
		struct Image
		{
			mem::soil::Image data;
			int width{};
			int height{};
			int channels{};
		};

		class SoilException : public std::runtime_error
		{
		public:
			SoilException(std::string const& msg)
				: std::runtime_error("[be::soil] soil exception: " + msg)
			{}
		};

		inline Image load_image(char const* filename, int force_channels)
		{
			Image image;
			image.data.reset(SOIL_load_image(
				filename,
				&image.width,
				&image.height,
				&image.channels,
				force_channels
			));
			if (image.data == nullptr) {
				throw SoilException(std::string(SOIL_last_result()) + " (file at: " + filename + " )");
			}
			return image;
		}

		inline mem::gl::Texture load_OGL_texture(
			char const* filename,
			int force_channels,
			GLuint reuse_texture_id,
			unsigned int flags)
		{
			auto texture = mem::gl::Texture(SOIL_load_OGL_texture(
				filename,
				force_channels,
				reuse_texture_id,
				flags
			));
			if (texture == mem::nullFraii) {
				throw SoilException(std::string(SOIL_last_result()) + " (file at: " + filename + " )");
			}
			return texture;
		}

		inline mem::gl::Texture load_OGL_cubemap(
			const char* x_pos_file,
			const char* x_neg_file,
			const char* y_pos_file,
			const char* y_neg_file,
			const char* z_pos_file,
			const char* z_neg_file,
			int force_channels,
			unsigned int reuse_texture_ID,
			unsigned int flags)
		{
			auto texture = mem::gl::Texture(SOIL_load_OGL_cubemap(
				x_pos_file,
				x_neg_file,
				y_pos_file,
				y_neg_file,
				z_pos_file,
				z_neg_file,
				force_channels,
				reuse_texture_ID,
				flags
			));
			if (texture == mem::nullFraii) {
				throw SoilException(std::string(SOIL_last_result()) + " (cubemap near: " + x_pos_file + " )");
			}
			return texture;
		}

		inline void initCubemapParameters()
		{
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		}
	}
}
