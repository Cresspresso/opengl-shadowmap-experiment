
#include "assets.hpp"
#include "picket_fence.hpp"

namespace example
{
	PicketFenceShader::PicketFenceShader()
	{
		char const* const vertexShader = R"__(
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoords;

out V2F {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} v2f;

uniform mat4 mvp;
uniform mat4 model;
uniform mat3 fixNormals;
uniform mat4 lightSpaceMatrix;

void main()
{
	vec4 p = vec4(inPosition, 1.0f);
	gl_Position = mvp * p;
	v2f.FragPos = vec3(model * p);
	v2f.Normal = normalize(fixNormals * inNormal);
	v2f.TexCoords = inTexCoords;
	v2f.FragPosLightSpace = lightSpaceMatrix * vec4(v2f.FragPos, 1.0f);
}
)__";
		char const* const fragmentShader = R"__(
#version 330 core

out vec4 outColor;

in V2F {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec4 FragPosLightSpace;
} v2f;

uniform sampler2D diffuseTextures[4];
uniform sampler2D shadowMap;
uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// check whether current frag pos is in shadow
	float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	vec3 color = texture(diffuseTextures[0], v2f.TexCoords).rgb;
	vec3 normal = v2f.Normal;
	vec3 lightColor = vec3(1.0f);

	// ambient
	vec3 ambient = 0.15 * color;

	// diffuse
	vec3 lightDir = normalize(lightPos - v2f.FragPos);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * lightColor;

	// specular
	vec3 viewDir = normalize(viewPos - v2f.FragPos);
	float spec = 0.0;
	vec3 halfwayDir = normalize(lightDir + viewDir);
	spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
	vec3 specular = spec * lightColor;

	// calculate shadow
	float shadow = ShadowCalculation(v2f.FragPosLightSpace);
	vec3 lighting = (ambient + (1.0f - shadow) * (diffuse + specular)) * color;

	outColor = vec4(lighting, 1.0f);

	// DEBUG
	outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
)__";
		m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "picket_fence.cpp");

		m_uniformLocations.mvp = glGetUniformLocation(program(), "mvp");
		m_uniformLocations.model = glGetUniformLocation(program(), "model");
		m_uniformLocations.fixNormals = glGetUniformLocation(program(), "fixNormals");
		m_uniformLocations.lightSpaceMatrix = glGetUniformLocation(program(), "lightSpaceMatrix");
		m_uniformLocations.viewPos = glGetUniformLocation(program(), "viewPos");
		m_uniformLocations.lightPos = glGetUniformLocation(program(), "lightPos");

		for (size_t i = 0; i < m_uniformLocations.diffuseTextures.size(); ++i)
		{
			std::string str = "diffuseTextures[";
			str += std::to_string(i);
			str += ']';
			m_uniformLocations.diffuseTextures[i] = glGetUniformLocation(program(), str.c_str());
		}
	}

	be::pink::model::Model loadPicketFenceModel()
	{
		return be::pink::model::loadModel((assets::projectAssetsFolder / "models/Fence.dae").string());
	}

	void renderPicketFence(
		PicketFenceShader const& shader,
		be::pink::model::Model const& model,
		be::pink::Camera const& camera,
		glm::vec3 const& lightPos,
		glm::mat4 const& lightSpaceMatrix,
		GLuint const shadowMapTextureIndex,
		glm::mat4 const& parentModelMatrix
	)
	{
		BE_USE_PROGRAM_SCOPE(shader.program());

		be::gl::uniformMat4(shader.uniformLocations().lightSpaceMatrix, lightSpaceMatrix);

		be::gl::uniformVec3(shader.uniformLocations().lightPos, lightPos);
		be::gl::uniformVec3(shader.uniformLocations().viewPos, camera.position);
		glUniform1i(shader.uniformLocations().shadowMap, shadowMapTextureIndex);

		be::pink::model::DrawNodeCallback const drawNode = [&](be::pink::model::Node const& node, glm::mat4 const& modelMatrix)
		{
			auto const mvp = camera.vp * modelMatrix;
			be::gl::uniformMat4(shader.uniformLocations().mvp, mvp);

			be::gl::uniformMat4(shader.uniformLocations().model, modelMatrix);

			auto const fixNormals = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));
			be::gl::uniformMat3(shader.uniformLocations().fixNormals, fixNormals);

			for (auto const& w : node.meshes)
			{
				if (auto const mesh = w.lock())
				{
					if (auto const material = mesh->material.lock())
					{
						std::vector<std::function<void()>> deferred;
						CRESS_MOO_DEFER_BEGIN(_);
						for (auto const& f : deferred) { f(); }
						CRESS_MOO_DEFER_END(_);

						if (auto const it = material->textureMap.find(aiTextureType_DIFFUSE);
							it != material->textureMap.end())
						{
							auto const& textures = it->second;
							auto const N = std::min<size_t>(textures.size(), shader.uniformLocations().diffuseTextures.size());
							for (size_t i = 0; i < N; ++i)
							{
								glActiveTexture(GL_TEXTURE0 + i);
								glBindTexture(GL_TEXTURE_2D, textures[i].get());
								glUniform1i(shader.uniformLocations().diffuseTextures[i], i);

								deferred.push_back([&shader, i]()noexcept {
									glActiveTexture(GL_TEXTURE0 + i);
									glBindTexture(GL_TEXTURE_2D, 0);
									});
							}
						}

						be::gl::drawBasicMesh(mesh->data);
					}
				}
			}
		};

		be::pink::model::renderModel(model, drawNode, parentModelMatrix);
	}
}
