#include "SceneNode.h"

#include <gtc/matrix_transform.hpp>

#include <utility>

SceneNode::SceneNode() = default;

SceneNode::SceneNode(std::string nodeName)
	: m_name(std::move(nodeName))
{
}

glm::mat4 SceneNode::local() const
{
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
	transform = glm::rotate(transform, rotationEuler.y, glm::vec3(0.0f, 1.0f, 0.0f));
	transform = glm::rotate(transform, rotationEuler.x, glm::vec3(1.0f, 0.0f, 0.0f));
	transform = glm::rotate(transform, rotationEuler.z, glm::vec3(0.0f, 0.0f, 1.0f));
	transform = glm::scale(transform, scale);
	return transform;
}

SceneNode* SceneNode::add(std::unique_ptr<SceneNode> child)
{
	if (!child)
	{
		return nullptr;
	}

	child->parent = this;
	SceneNode* rawChild = child.get();
	m_children.push_back(std::move(child));
	return rawChild;
}

SceneNode* SceneNode::find(const std::string& nodeName)
{
	if (m_name == nodeName)
	{
		return this;
	}

	for (const std::unique_ptr<SceneNode>& child : m_children)
	{
		SceneNode* match = child->find(nodeName);
		if (match != nullptr)
		{
			return match;
		}
	}

	return nullptr;
}

const SceneNode* SceneNode::find(const std::string& nodeName) const
{
	if (m_name == nodeName)
	{
		return this;
	}

	for (const std::unique_ptr<SceneNode>& child : m_children)
	{
		const SceneNode* match = child->find(nodeName);
		if (match != nullptr)
		{
			return match;
		}
	}

	return nullptr;
}

void SceneNode::updateWorld()
{
	world = (parent != nullptr ? parent->world : glm::mat4(1.0f)) * local();

	for (const std::unique_ptr<SceneNode>& child : m_children)
	{
		child->updateWorld();
	}
}

bool SceneNode::visibleInHierarchy() const
{
	if (!visible)
	{
		return false;
	}

	return parent == nullptr || parent->visibleInHierarchy();
}

void SceneNode::setModel(Model* newModel)
{
	m_model = newModel;
	if (m_model != nullptr)
	{
		m_mesh = nullptr;
	}
}

void SceneNode::setMesh(MeshModel* newMesh)
{
	m_mesh = newMesh;
	if (m_mesh != nullptr)
	{
		m_model = nullptr;
	}
}

void SceneNode::setTexture(Texture* newTexture)
{
	m_texture = newTexture;
}

Model* SceneNode::model() const
{
	return m_model;
}

MeshModel* SceneNode::mesh() const
{
	return m_mesh;
}

Texture* SceneNode::texture() const
{
	return m_texture;
}

const std::string& SceneNode::name() const
{
	return m_name;
}

void SceneNode::setName(std::string nodeName)
{
	m_name = std::move(nodeName);
}

const std::vector<std::unique_ptr<SceneNode>>& SceneNode::children() const
{
	return m_children;
}
