
#include "ground.hpp"

namespace example
{
	GroundShader::GroundShader()
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
uniform mat4 lightMvp;
uniform vec2 uvScale = vec2(1.0f);

void main()
{
	vec4 p = vec4(inPosition, 1.0f);
	gl_Position = mvp * p;
	v2f.FragPos = vec3(model * p);
	v2f.Normal = fixNormals * inNormal;
	v2f.TexCoords = inTexCoords * uvScale;
	v2f.FragPosLightSpace = lightMvp * p;
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

uniform sampler2D diffuseTexture;
uniform sampler2D shadowMap;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform float maxShadowDistance;

void main()
{
	vec3 color = texture(diffuseTexture, v2f.TexCoords).rgb;
	vec3 normal = normalize(v2f.Normal);
	vec3 lightColor = vec3(1.0f);


	// ambient
	float ambientStr = 0.15f;
	vec3 ambient = 0.05f * color;


	// diffuse
	float diff = max(dot(lightDir, normal), 0.0f);
	vec3 diffuse = diff * lightColor;


	// specular
	vec3 viewDir = normalize(viewPos - v2f.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0f), 64.0f);
	vec3 specular = spec * lightColor;



	// CALCULATE SHADOW
	// perform perspective divide
	vec3 projCoords = v2f.FragPosLightSpace.xyz / v2f.FragPosLightSpace.w;

	// transform to [0,1] range
	projCoords = projCoords * 0.5f + 0.5f;

	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;

	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	// check whether current frag pos is in shadow
	float bias = 0.001f;
	float illumination = (projCoords.x < 0.0f || projCoords.x > 1.0f || projCoords.y < 0.0f || projCoords.y > 1.0f)
		? 1.0f
		: ((currentDepth < maxShadowDistance && currentDepth - bias > closestDepth)
		? 0.0f : 1.0f);

	vec3 lighting = max(ambientStr, illumination) * color + illumination * (diffuse + specular);
	outColor = vec4(lighting, 1.0f);
}
)__";
		m_shader = be::gl::makeBasicShaderProgram(vertexShader, fragmentShader, "ground.cpp");
		GLuint const program = m_shader.program.get();
		m_uniformLocations.diffuseTexture = glGetUniformLocation(program, "diffuseTexture");
		m_uniformLocations.lightDir = glGetUniformLocation(program, "lightDir");
		m_uniformLocations.lightMvp = glGetUniformLocation(program, "lightMvp");
		m_uniformLocations.mvp = glGetUniformLocation(program, "mvp");
		m_uniformLocations.model = glGetUniformLocation(program, "model");
		m_uniformLocations.shadowMap = glGetUniformLocation(program, "shadowMap");
		m_uniformLocations.viewPos = glGetUniformLocation(program, "viewPos");
		m_uniformLocations.uvScale = glGetUniformLocation(program, "uvScale");
		m_uniformLocations.maxShadowDistance = glGetUniformLocation(program, "maxShadowDistance");

		BE_USE_PROGRAM_SCOPE(program);
		glUniform1i(m_uniformLocations.diffuseTexture, 0);
	}



	be::mem::gl::Texture loadGroundTexture()
	{
		auto texture = be::soil::load_OGL_texture(
			"resources/textures/seamless_green_grass_rough_DIFFUSE.jpg",
			SOIL_LOAD_RGBA, 0, SOIL_FLAG_MIPMAPS
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		return texture;
	}



	void renderGround(
		GroundShader const& shader,
		be::gl::BasicMesh const& mesh,
		GLuint const tex,
		be::pink::Camera const& camera,
		glm::vec3 const& lightDir,
		glm::mat4 const& lightSpaceMatrix,
		GLint const shadowMapSlotIndex,
		glm::mat4 const& modelMatrix,
		glm::vec2 const& uvScale,
		GLfloat const maxShadowDistance
	)
	{
		BE_USE_PROGRAM_SCOPE(shader.program());

		auto const& loc = shader.uniformLocations();
		be::gl::uniformMat3(loc.fixNormals, be::pink::calcFixNormalsMatrix(modelMatrix));
		be::gl::uniformVec3(loc.lightDir, glm::normalize(lightDir));
		be::gl::uniformMat4(loc.lightMvp, lightSpaceMatrix * modelMatrix);
		be::gl::uniformMat4(loc.model, modelMatrix);
		be::gl::uniformMat4(loc.mvp, camera.vp * modelMatrix);
		glUniform1i(loc.shadowMap, shadowMapSlotIndex);
		be::gl::uniformVec2(loc.uvScale, uvScale);
		be::gl::uniformVec3(loc.viewPos, camera.position);
		glUniform1f(loc.maxShadowDistance, maxShadowDistance);

		BE_BIND_TEXTURE_SCOPE(GL_TEXTURE_2D, tex, GL_TEXTURE0);

		be::gl::drawBasicMesh(mesh);
	}
}
