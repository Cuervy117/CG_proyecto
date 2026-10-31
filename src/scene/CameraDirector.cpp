#include "CameraDirector.h"

#include "CameraRig.h"
#include "Chase.h"
#include "Flight.h"
#include "FollowXZ.h"
#include "Orbit.h"

#include <glfw3.h>
#include <gtc/matrix_transform.hpp>

#include <utility>

namespace
{
	std::unique_ptr<CameraRig> crearRig(CameraMode mode)
	{
		switch (mode)
		{
		case CameraMode::Planeta:
			return std::make_unique<FollowXZ>();
		case CameraMode::Carrera:
			return std::make_unique<Chase>();
		case CameraMode::Vuelo:
			return std::make_unique<Flight>();
		case CameraMode::Observacion:
			return std::make_unique<Orbit>();
		}

		return nullptr;
	}

	bool modoParaTecla(int key, CameraMode& mode)
	{
		switch (key)
		{
		case GLFW_KEY_F1:
			mode = CameraMode::Planeta;
			return true;
		case GLFW_KEY_F2:
			mode = CameraMode::Carrera;
			return true;
		case GLFW_KEY_F3:
			mode = CameraMode::Vuelo;
			return true;
		case GLFW_KEY_F4:
			mode = CameraMode::Observacion;
			return true;
		default:
			return false;
		}
	}
}

bool CameraDirector::setMode(CameraMode mode)
{
	if (m_rig && m_mode == mode)
	{
		return false;
	}

	std::unique_ptr<CameraRig> rig = crearRig(mode);
	if (!rig)
	{
		return false;
	}

	m_rig = std::move(rig);
	m_mode = mode;
	m_transitioning = false;
	return true;
}

CameraMode CameraDirector::mode() const
{
	return m_mode;
}

bool CameraDirector::setModeForKey(int key)
{
	CameraMode mode;
	if (!modoParaTecla(key, mode))
	{
		return false;
	}

	setMode(mode);
	return true;
}

void CameraDirector::update(float dt, const SceneNode& target)
{
	if (m_rig)
	{
		m_rig->update(dt, target);
	}
}

glm::mat4 CameraDirector::view() const
{
	if (!m_rig)
	{
		return glm::mat4(1.0f);
	}

	return m_rig->view();
}

glm::mat4 CameraDirector::projection(float aspect) const
{
	if (!m_rig)
	{
		return glm::perspective(glm::radians(45.0f), aspect, 0.1f, 2000.0f);
	}

	return m_rig->projection(aspect);
}

bool CameraDirector::transitioning() const
{
	return m_transitioning;
}

CameraRig* CameraDirector::rig() const
{
	return m_rig.get();
}
