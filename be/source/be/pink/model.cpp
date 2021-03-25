
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "be/pink/model.hpp"

namespace be
{
	namespace pink
	{
		namespace model
		{
			std::vector<be::mem::gl::Texture> loadTextures(
				std::string const& dir,
				aiMaterial const* const rawMaterial,
				aiTextureType const type)
			{
				std::vector<be::mem::gl::Texture> textures;
				unsigned int const len = rawMaterial->GetTextureCount(type);
				textures.reserve(len);
				for (unsigned int i = 0; i < len; ++i)
				{
					try
					{
						aiString str;
						rawMaterial->GetTexture(type, i, &str);

						std::string const filename = dir + str.C_Str();

						textures.emplace_back(be::soil::load_OGL_texture(filename.c_str(), SOIL_LOAD_RGBA, 0, 0));
					}
					catch (be::soil::SoilException const&)
					{
						be::Application::logException();
						continue;
					}
				}
				return textures;
			}

			std::shared_ptr<Material> processMaterial(
				std::string const& dir,
				aiMaterial const* const rawMaterial)
			{
				auto material = std::make_shared<Material>();
				for (aiTextureType i = static_cast<aiTextureType>(aiTextureType_NONE + 1);
					i < AI_TEXTURE_TYPE_MAX;
					i = static_cast<aiTextureType>(i + 1))
				{
					material->textureMap[i] = loadTextures(dir, rawMaterial, i);
				}
				return material;
			}

			std::shared_ptr<Mesh> processMesh(
				std::vector<std::shared_ptr<Material>> const& sceneMaterials,
				aiMesh const* const rawMesh)
			{
				using Vertex = be::gl::BasicVertex;

				// process vertices
				std::vector<Vertex> vertices;
				for (unsigned int i = 0; i < rawMesh->mNumVertices; i++)
				{
					Vertex vertex;

					auto const& v = rawMesh->mVertices[i];
					vertex.position = { v.x, v.y, v.z };

					auto const& n = rawMesh->mNormals[i];
					vertex.normal = { n.x, n.y, n.z };

					if (rawMesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
					{
						auto const& t = rawMesh->mTextureCoords[0][i];
						vertex.texCoords = { t.x, t.y };
					}
					else
					{
						vertex.texCoords = {};
					}

					vertices.push_back(vertex);
				}

				// process indices
				std::vector<GLuint> indices;
				for (unsigned int i = 0; i < rawMesh->mNumFaces; i++)
				{
					aiFace face = rawMesh->mFaces[i];
					for (unsigned int j = 0; j < face.mNumIndices; j++)
					{
						indices.push_back(face.mIndices[j]);
					}
				}

				auto mesh = std::make_shared<Mesh>();
				mesh->data = be::gl::makeBasicMesh(vertices, indices);
				mesh->material = sceneMaterials[rawMesh->mMaterialIndex];
				return mesh;
			}

			std::shared_ptr<Node> processNode(
				std::vector<std::shared_ptr<Mesh>> const& sceneMeshes,
				std::shared_ptr<Node> const& parent,
				aiNode const* const rawNode)
			{
				auto node = std::make_shared<Node>();

				node->parent = parent;

				// transformation
				// transpose the raw matrix
				for (unsigned int y = 0; y < 4; ++y)
				{
					for (unsigned int x = 0; x < 4; ++x)
					{
						node->localTransformation[x][y] = rawNode->mTransformation[y][x];
					}
				}

				// meshes
				for (unsigned int i = 0; i < rawNode->mNumMeshes; ++i)
				{
					node->meshes.insert(sceneMeshes[rawNode->mMeshes[i]]);
				}

				// children
				for (unsigned int i = 0; i < rawNode->mNumChildren; ++i)
				{
					node->children.insert(processNode(sceneMeshes, node, rawNode->mChildren[i]));
				}

				return node;
			}

			Model loadModel(std::string const& filename)
			{
				Assimp::Importer importer;

				aiScene const* const rawScene = importer.ReadFile(filename,
					//aiProcess_CalcTangentSpace |
					aiProcess_Triangulate |
					//aiProcess_JoinIdenticalVertices |
					//aiProcess_SortByPType |
					aiProcess_FlipUVs
				);

				if (!rawScene || rawScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !rawScene->mRootNode)
				{
					throw std::runtime_error(std::string("Failed to load model: ") + importer.GetErrorString());
				}

				std::string const dir = filename.substr(0, filename.find_last_of('/') + 1);

				auto scene = Model();

				scene.materials.resize(rawScene->mNumMaterials);
				for (unsigned int i = 0; i < rawScene->mNumMaterials; ++i)
				{
					scene.materials[i] = processMaterial(dir, rawScene->mMaterials[i]);
				}

				scene.meshes.resize(rawScene->mNumMeshes);
				for (unsigned int i = 0; i < rawScene->mNumMeshes; ++i)
				{
					scene.meshes[i] = processMesh(scene.materials, rawScene->mMeshes[i]);
				}

				scene.rootNode = processNode(scene.meshes, nullptr, rawScene->mRootNode);

				return scene;
			}



			glm::mat4 calcModelMatrix(glm::mat4 const& parentModelMatrix, Node const& node)
			{
				return parentModelMatrix * node.localTransformation;
			}

			void calcModelMatricesImpl(
				NodeModelMatrixMap& matrices,
				std::shared_ptr<Node> const& node,
				glm::mat4 const& parentModelMatrix)
			{
				if (!node) { return; }
				glm::mat4 const modelMatrix = calcModelMatrix(parentModelMatrix, *node);
				matrices.emplace(std::make_pair(std::weak_ptr<Node>(node), modelMatrix));
				for (auto const& pChild : node->children)
				{
					calcModelMatricesImpl(matrices, pChild, modelMatrix);
				}
			}

			NodeModelMatrixMap calcModelMatrices(
				std::shared_ptr<Node> const& rootNode,
				glm::mat4 const& parentModelMatrix)
			{
				std::map<std::weak_ptr<Node>, glm::mat4, std::owner_less<std::weak_ptr<Node>>> matrices;
				calcModelMatricesImpl(matrices, rootNode, parentModelMatrix);
				return matrices;
			}



			void renderModelNode(
				std::shared_ptr<Node> const& node,
				DrawNodeCallback const& drawNode,
				NodeModelMatrixMap const& modelMatrices
			)
			{
				if (!node) { return; }
				if (!drawNode) { return; }

				glm::mat4 const modelMatrix = [&]() -> glm::mat4 {
					auto const it = modelMatrices.find(node);
					return it == modelMatrices.end()
						? glm::mat4()
						: it->second;
				}();
				drawNode(*node, modelMatrix);

				for (auto const& child : node->children)
				{
					renderModelNode(child, drawNode, modelMatrices);
				}
			}

			void renderModel(
				Model const& model,
				DrawNodeCallback const& drawNode,
				glm::mat4 const& parentModelMatrix)
			{
				if (!drawNode) { return; }
				auto const modelMatrices = calcModelMatrices(model.rootNode, parentModelMatrix);
				renderModelNode(model.rootNode, drawNode, modelMatrices);
			}
		}
	}
}
