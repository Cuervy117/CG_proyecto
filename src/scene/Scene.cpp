#include "Scene.h"

void Scene::updateWorld()
{
	root.updateWorld();
}

SceneNode* Scene::find(const std::string& nodeName)
{
	return root.find(nodeName);
}

const SceneNode* Scene::find(const std::string& nodeName) const
{
	return root.find(nodeName);
}
