#pragma once

#include <glm.hpp>

class SceneNode;

class CameraRig
{
public:
	virtual ~CameraRig() = default;

	virtual void update(float dt, const SceneNode& target) = 0;
	virtual glm::mat4 view() const = 0;

	glm::mat4 projection(float aspect) const;

	// Dirección de mirada para FollowXZ y Flight. Chase usa ángulos relativos a la nave.
	glm::vec3 miradaMouse{0.0f};

	float fov = 45.0f;
	float nearZ = 0.1f;
	float farZ = 2000.0f;

protected:
	glm::vec3 m_pos{0.0f};
	glm::vec3 m_target{0.0f};
	glm::vec3 m_up{0.0f, 1.0f, 0.0f};
};
