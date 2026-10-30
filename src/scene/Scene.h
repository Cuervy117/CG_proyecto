#pragma once

#include <glm.hpp>

#include <string>

#include "SceneNode.h"

class Shader;

class Scene
{
public:
	SceneNode root;

	void updateWorld();
	SceneNode* find(const std::string& nodeName);
	const SceneNode* find(const std::string& nodeName) const;

	void render(Shader& shader, const glm::mat4& view, const glm::mat4& projection);
};
