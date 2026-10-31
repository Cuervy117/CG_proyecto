#pragma once

#include "CameraRig.h"

class Flight : public CameraRig
{
public:
	explicit Flight(float distancia = 16.0f, float altura = 6.0f,
		float alturaObjetivo = 2.0f, float adelanto = 4.0f);

	void update(float dt, const SceneNode& target) override;
	glm::mat4 view() const override;

	glm::vec3 posicion() const;
	glm::vec3 objetivo() const;

	float distancia;
	float altura;
	float alturaObjetivo;
	float adelanto;
};
