#include "FollowXZ.h"

#include "SceneNode.h"

#include <gtc/matrix_transform.hpp>

#include <cmath>

FollowXZ::FollowXZ(float alturaInicial, float distanciaInicial,
	float alturaObjetivoInicial)
	: altura(alturaInicial),
	distancia(distanciaInicial),
	alturaObjetivo(alturaObjetivoInicial)
{
}

void FollowXZ::update(float dt, const SceneNode& target)
{
	(void)dt;

	const glm::vec3 base(target.world[3]);
	const float distanciaSegura = (distancia > 0.01f) ? distancia : 0.01f;

	m_target = base + glm::vec3(0.0f, alturaObjetivo, 0.0f);
	// En R1 sólo cambia el azimut. Mantener la altura evita hundir la cámara
	// al llevar el ratón al límite vertical.
	glm::vec3 horizontal(miradaMouse.x, 0, miradaMouse.z);
	if (glm::dot(horizontal, horizontal) < 1.0e-8f)
		horizontal = glm::vec3(0, 0, 1);
	m_pos = base + m_up * altura - glm::normalize(horizontal) * distanciaSegura;
}

glm::mat4 FollowXZ::view() const
{
	if (glm::length(m_target - m_pos) < 1e-4f)
	{
		return glm::mat4(1.0f);
	}

	return glm::lookAt(m_pos, m_target, m_up);
}

glm::vec3 FollowXZ::posicion() const
{
	return m_pos;
}

glm::vec3 FollowXZ::objetivo() const
{
	return m_target;
}
