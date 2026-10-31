#include "Flight.h"

#include "SceneNode.h"

#include <gtc/matrix_transform.hpp>

#include <cmath>

static glm::vec3 frente3D(const SceneNode& target)
{
	glm::vec3 frente(target.world[2]);

	const float longitudCuadrada = glm::dot(frente, frente);
	if (longitudCuadrada < 1e-6f)
	{
		return glm::vec3(0.0f, 0.0f, 1.0f);
	}

	return frente / std::sqrt(longitudCuadrada);
}

Flight::Flight(float distanciaInicial, float alturaInicial,
	float alturaObjetivoInicial, float adelantoInicial)
	: distancia(distanciaInicial),
	altura(alturaInicial),
	alturaObjetivo(alturaObjetivoInicial),
	adelanto(adelantoInicial)
{
}

void Flight::update(float dt, const SceneNode& target)
{
	(void)dt;

	const glm::vec3 base(target.world[3]);
	const glm::vec3 frente = frente3D(target);
	const float distanciaSegura = (distancia > 0.01f) ? distancia : 0.01f;

	m_pos = base - frente * distanciaSegura + m_up * altura;
	m_target = base + frente * adelanto + m_up * alturaObjetivo;
}

glm::mat4 Flight::view() const
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

glm::vec3 Flight::posicion() const
{
	return m_pos;
}

glm::vec3 Flight::objetivo() const
{
	return m_target;
}
