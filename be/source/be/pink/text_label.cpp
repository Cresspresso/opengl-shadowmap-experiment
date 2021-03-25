
#include "be/uniform.hpp"
#include "be/pink/text_label.hpp"

namespace be
{
	namespace pink
	{
		namespace text_label
		{
			TextLabelShader::TextLabelShader()
			{
				char const* const vertexShader = R"__(
#version 330 core

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoords;

uniform mat4 mvp;

out vec2 v2fTexCoords;

void main()
{
	gl_Position = mvp * vec4(inPosition, 0, 1);
	v2fTexCoords = inTexCoords;
}
)__";
				char const* const fragmentShader = R"__(
#version 330 core

in vec2 v2fTexCoords;

out vec4 outColor;

uniform vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform sampler2D glyphTexture;

void main()
{
	float a = texture(glyphTexture, v2fTexCoords).r;
	outColor = color * vec4(vec3(1.0f), a);
}
)__";
				m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "TextLabelShader");
				GLuint const program = m_shader.program.get();
				m_uniformLocations.mvp = glGetUniformLocation(program, "mvp");
				m_uniformLocations.color = glGetUniformLocation(program, "color");
				m_uniformLocations.glyphTexture = glGetUniformLocation(program, "glyphTexture");

				BE_USE_PROGRAM_SCOPE(m_shader.program.get());
				glUniform1i(m_uniformLocations.glyphTexture, 0);
			}

			TextGlyphMesh makeTextGlyphMesh()
			{
				using Vertex = TextGlyphVertex;

				TextGlyphMesh mesh;
				mesh.vertexArray = be::mem::gl::makeVertexArray();
				BE_BIND_VERTEX_ARRAY_SCOPE(mesh.vertexArray.get());

				mesh.vertexBuffer = be::mem::gl::makeBuffer();
				glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer.get());
				glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, nullptr, GL_DYNAMIC_DRAW);

				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid const*>(offsetof(Vertex, position)));
				glEnableVertexAttribArray(0);

				glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid const*>(offsetof(Vertex, texCoords)));
				glEnableVertexAttribArray(1);

				return mesh;
			}

			void renderTextLabel(RenderTextLabelInfo const& info)
			{
				auto const& shader = info.shader.get();
				auto& mesh = info.mesh.get();
				auto const& font = info.font.get();
				auto const& lineHeight = info.lineHeight.get();
				auto const& tabWidth = info.tabWidth.get();
				auto const& mvp = info.mvp.get();
				auto const& color = info.color.get();
				auto const& scale = info.scale.get();
				auto const& text = info.text.get();


				glEnable(GL_BLEND);
				CRESS_MOO_DEFER_EXPRESSION(glDisable(GL_BLEND));
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				BE_USE_PROGRAM_SCOPE(shader.program());

				auto const& loc = shader.uniformLocations();
				be::gl::uniformMat4(loc.mvp, mvp);
				be::gl::uniformVec4(loc.color, color);

				BE_BIND_VERTEX_ARRAY_SCOPE(mesh.vertexArray.get());
				glBindBuffer(GL_ARRAY_BUFFER, mesh.vertexBuffer.get());

				// iterate through the characters of the text
				glm::vec2 pos = glm::vec2();
				//std::vector<size_t> unwrittenIndices;
				bool failed = false;
				for (size_t i = 0; i < text.size(); ++i)
				{
					auto const c = text[i];
					if (c == '\n')
					{
						pos.x = 0.0f;
						pos.y -= lineHeight;
						continue;
					}
					else if (c == '\t')
					{
						pos.x += tabWidth;
						continue;
					}

					auto const it = font.find(c);
					if (it == font.cend())
					{
						//unwrittenIndices.push_back(i);
						failed = true;
						continue;
					}

					be::ft::FontGlyph const& glyph = it->second;
					CRESS_MOO_DEFER_EXPRESSION(pos.x += glyph.advance * scale.x);

					float const xpos = pos.x
						+ glyph.bearing.x * scale.x;
					float const ypos = pos.y
						- (glyph.size.y - glyph.bearing.y) * scale.y;
					float const glyphWidth = glyph.size.x * scale.x;
					float const glyphHeight = glyph.size.y * scale.y;

					TextGlyphVertex const vertices[4] = { TextGlyphVertex
					{ { xpos, ypos + glyphHeight }, { 0, 0 } },
					{ { xpos, ypos }, { 0, 1 } },
					{ { xpos + glyphWidth, ypos }, { 1, 1 } },
					{ { xpos + glyphWidth, ypos + glyphHeight }, { 1, 0 } },
					};

					// update VBO for this glyph.
					// WARNING: assumes buffer is GL_DYNAMIC_DRAW.
					glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

					// render the glyph over the mesh
					BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, glyph.texture.get(), GL_TEXTURE0);

					glDrawArrays(GL_QUADS, 0, 4);
				}

				//if (!unwrittenIndices.empty())
				if (failed)
				{
					throw RenderTextLabelException(text);
				}
			}
		}
	}
}
