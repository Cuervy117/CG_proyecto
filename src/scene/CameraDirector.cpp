#include "CameraDirector.h"

#include "CameraRig.h"

#include <gtc/matrix_transform.hpp>

#include <utility>

void CameraDirector::setRig(std::unique_ptr<CameraRig> rig)
{
	m_rig = std::move(rig);
	m_transitioning = false;
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
