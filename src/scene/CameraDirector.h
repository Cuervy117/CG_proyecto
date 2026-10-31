#pragma once

#include <glm.hpp>

#include <memory>

class CameraRig;
class SceneNode;

class CameraDirector
{
public:
	void setRig(std::unique_ptr<CameraRig> rig);
	void update(float dt, const SceneNode& target);

	glm::mat4 view() const;
	glm::mat4 projection(float aspect) const;
	bool transitioning() const;
	CameraRig* rig() const;

private:
	std::unique_ptr<CameraRig> m_rig;
	bool m_transitioning = false;
};
