#pragma once

#include <glm.hpp>

#include <memory>

class CameraRig;
class SceneNode;

enum class CameraMode
{
	Planeta,
	Carrera,
	Vuelo,
	Observacion
};

class CameraDirector
{
public:
	bool setMode(CameraMode mode);
	CameraMode mode() const;
	bool setModeForKey(int key);

	void update(float dt, const SceneNode& target);

	glm::mat4 view() const;
	glm::mat4 projection(float aspect) const;
	bool transitioning() const;
	CameraRig* rig() const;

private:
	std::unique_ptr<CameraRig> m_rig;
	CameraMode m_mode = CameraMode::Planeta;
	bool m_transitioning = false;
};
