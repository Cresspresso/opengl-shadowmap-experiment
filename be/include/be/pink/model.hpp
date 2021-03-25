
#pragma once

#include <functional>
#include <set>
#include <assimp/material.h>
#include <be/be.hpp>

#include "camera.hpp"

namespace be
{
	namespace pink
	{
		namespace model
		{
			struct Material
			{
				std::map<aiTextureType, std::vector<be::mem::gl::Texture>> textureMap;
			};

			struct Mesh
			{
				be::gl::BasicMesh data;
				std::weak_ptr<Material> material;
			};

			struct Node
			{
				std::weak_ptr<Node> parent{};
				std::set<std::shared_ptr<Node>> children;
				std::set<std::weak_ptr<Mesh>, std::owner_less<std::weak_ptr<Mesh>>> meshes;
				glm::mat4 localTransformation;
			};

			struct Model
			{
				std::vector<std::shared_ptr<Material>> materials;
				std::vector<std::shared_ptr<Mesh>> meshes;
				std::shared_ptr<Node> rootNode;
			};



			Model loadModel(std::string const& filename);



			using NodeModelMatrixMap = std::map<
				std::weak_ptr<Node>,
				glm::mat4,
				std::owner_less<std::weak_ptr<Node>>>;

			NodeModelMatrixMap calcModelMatrices(
				std::shared_ptr<Node> const& rootNode,
				glm::mat4 const& parentModelMatrix = glm::mat4()
			);



			using DrawNodeCallback = std::function<void(
				be::pink::model::Node const& node,
				glm::mat4 const& modelMatrix
				)>;

			void renderModelNode(
				std::shared_ptr<Node> const& node,
				DrawNodeCallback const& drawNode,
				NodeModelMatrixMap const& modelMatrices
			);

			void renderModel(
				Model const& model,
				DrawNodeCallback const& drawNode,
				glm::mat4 const& parentModelMatrix
			);
		}
	}
}
