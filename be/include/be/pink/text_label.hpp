
#pragma once

#include <cassert>
#include <glm/glm.hpp>

#include "be/need.hpp"
#include "be/gl.hpp"
#include "be/ft.hpp"

namespace be
{
	namespace pink
	{
		namespace text_label
		{
			class TextLabelShader
			{
			private:
				be::gl::ShaderProgram m_shader{};
				struct UniformLocations {
					GLuint mvp;
					GLuint color;
					GLuint glyphTexture;
				} m_uniformLocations{};

			public:
				TextLabelShader();
				GLuint program() const { return m_shader.program.get(); }
				UniformLocations const& uniformLocations() const { return m_uniformLocations; }
			};

			struct TextGlyphVertex
			{
				glm::vec2 position;
				glm::vec2 texCoords;
			};
			struct TextGlyphMesh
			{
				be::mem::gl::VertexArray vertexArray;
				be::mem::gl::Buffer vertexBuffer; // should be GL_DYNAMIC_DRAW
			};
			TextGlyphMesh makeTextGlyphMesh();

			class RenderTextLabelException final : public std::runtime_error
			{
			public:
				RenderTextLabelException(std::string const& text)
					: std::runtime_error("[example] render text label exception: "
						"failed to render all characters of text: " + text)
				{}
			};

			struct RenderTextLabelInfo
			{
				need_ref<TextLabelShader const> shader;
				need_ref<TextGlyphMesh /* mutable */> mesh;
				need_ref<be::ft::Font const> font;
				need<float> lineHeight;
				need<float> tabWidth;
				need_ref<glm::mat4 const> mvp;
				need_ref<glm::vec4 const> color;
				need<glm::vec2> scale;
				need_ref<std::string const> text;
			};
			void renderTextLabel(RenderTextLabelInfo const& info);

			struct RenderTextLabelLegacyInfo
			{
				TextLabelShader const* shader{};
				TextGlyphMesh* mesh{};
				be::ft::Font const* font{};
				float lineHeight{};
				float tabWidth{};
				glm::mat4 const* mvp{};
				glm::vec4 const* color{};
				glm::vec2 scale{};
				std::string const* text{};
			};
			inline void renderTextLabelLegacy(RenderTextLabelLegacyInfo const& info)
			{
				assert(info.shader);
				assert(info.mesh);
				assert(info.font);
				assert(info.mvp);
				assert(info.color);
				assert(info.text);

				renderTextLabel(RenderTextLabelInfo{
					*info.shader,
					*info.mesh,
					*info.font,
					info.lineHeight,
					info.tabWidth,
					*info.mvp,
					*info.color,
					info.scale,
					*info.text
					});
			}
		}
	}
}
