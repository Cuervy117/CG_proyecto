#include "Chase.h"

#include "SceneNode.h"

#include <gtc/matrix_transform.hpp>

#include <cmath>

static glm::vec3 frenteHorizontal(const SceneNode& target)
{
	glm::vec3 frente(target.world[2]);
	frente.y = 0.0f;

	const float longitudCuadrada = glm::dot(frente, frente);
	if (longitudCuadrada < 1e-6f)
	{
		return glm::vec3(0.0f, 0.0f, 1.0f);
	}

	return frente / std::sqrt(longitudCuadrada);
}

Chase::Chase(float distanciaInicial, float alturaInicial,
	float alturaObjetivoInicial, float adelantoInicial)
	: distancia(distanciaInicial),
	altura(alturaInicial),
	alturaObjetivo(alturaObjetivoInicial),
	adelanto(adelantoInicial)
{
}

void Chase::update(float dt, const SceneNode& target)
{
	(void)dt;

	const glm::vec3 base(target.world[3]);
	const glm::vec3 frente = frenteHorizontal(target);
	const float distanciaSegura = (distancia > 0.01f) ? distancia : 0.01f;

	m_target = base + glm::vec3(0.0f, alturaObjetivo, 0.0f) + frente * adelanto;
	m_pos = base - frente * distanciaSegura + glm::vec3(0.0f, altura, 0.0f);
}

glm::mat4 Chase::view() const
{
	if (glm::length(m_target - m_pos) < 1e-4f)
	{
		return glm::mat4(1.0f);
	}

	return glm::lookAt(m_pos, m_target, m_up);
}

glm::vec3 Chase::posicion() const
{
	return m_pos;
}

glm::vec3 Chase::objetivo() const
{
	return m_target;
}
