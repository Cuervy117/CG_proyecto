#include "CameraDirector.h"

#include "CameraRig.h"

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

bool CameraDirector::transitioning() const
{
	return m_transitioning;
}

CameraRig* CameraDirector::rig() const
{
	return m_rig.get();
}
