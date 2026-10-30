#pragma once

#include <glm.hpp>

#include <memory>
#include <string>
#include <vector>

class Model;
class MeshModel;
class Texture;

class SceneNode
{
public:
	SceneNode();
	explicit SceneNode(std::string nodeName);

	SceneNode(const SceneNode&) = delete;
	SceneNode& operator=(const SceneNode&) = delete;
	SceneNode(SceneNode&&) = delete;
	SceneNode& operator=(SceneNode&&) = delete;

	glm::vec3 position{0.0f};
	glm::vec3 rotationEuler{0.0f};
	glm::vec3 scale{1.0f};
	bool visible = true;
	glm::vec3 tint{1.0f};
	float specular = 0.3f;
	float shininess = 4.0f;
	glm::mat4 world{1.0f};

	SceneNode* parent = nullptr;

	glm::mat4 local() const;
	SceneNode* add(std::unique_ptr<SceneNode> child);
	SceneNode* find(const std::string& nodeName);
	const SceneNode* find(const std::string& nodeName) const;
	void updateWorld();
	bool visibleInHierarchy() const;

	void setModel(Model* newModel);
	void setMesh(MeshModel* newMesh);
	void setTexture(Texture* newTexture);

	Model* model() const;
	MeshModel* mesh() const;
	Texture* texture() const;

	const std::string& name() const;
	void setName(std::string nodeName);

	const std::vector<std::unique_ptr<SceneNode>>& children() const;

private:
	std::string m_name;
	Model* m_model = nullptr;
	MeshModel* m_mesh = nullptr;
	Texture* m_texture = nullptr;
	std::vector<std::unique_ptr<SceneNode>> m_children;
};
