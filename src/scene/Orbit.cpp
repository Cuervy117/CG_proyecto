#include "Orbit.h"

#include "SceneNode.h"

#include <gtc/matrix_transform.hpp>

#include <cmath>

Orbit::Orbit(float distanciaInicial, float yawInicial, float pitchInicial,
	float alturaObjetivoInicial, float velocidadAngularInicial)
	: distancia(distanciaInicial),
	yaw(yawInicial),
	pitch(pitchInicial),
	alturaObjetivo(alturaObjetivoInicial),
	velocidadAngular(velocidadAngularInicial)
{
}

void Orbit::update(float dt, const SceneNode& target)
{
	yaw += velocidadAngular * dt;

	const glm::vec3 base(target.world[3]);
	const glm::vec3 centro = base + m_up * alturaObjetivo;
	const float distanciaSegura = (distancia > 0.01f) ? distancia : 0.01f;
	const float cosPitch = std::cos(pitch);
	const glm::vec3 offset(
		std::sin(yaw) * cosPitch,
		std::sin(pitch),
		std::cos(yaw) * cosPitch);

	m_pos = centro + offset * distanciaSegura;
	m_target = centro;
}

glm::mat4 Orbit::view() const
{
	const glm::vec3 direccion = m_target - m_pos;
	const float longitudCuadrada = glm::dot(direccion, direccion);
	if (longitudCuadrada < 1e-8f)
	{
		return glm::mat4(1.0f);
	}

	glm::vec3 arriba = m_up;
	if (std::fabs(glm::dot(glm::normalize(direccion), arriba)) > 0.999f)
	{
		arriba = glm::vec3(0.0f, 0.0f, 1.0f);
	}

	return glm::lookAt(m_pos, m_target, arriba);
}

glm::vec3 Orbit::posicion() const
{
	return m_pos;
}

glm::vec3 Orbit::objetivo() const
{
	return m_target;
}
