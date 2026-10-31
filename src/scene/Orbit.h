#pragma once

#include "CameraRig.h"

class Orbit : public CameraRig
{
public:
	explicit Orbit(float distancia = 20.0f, float yaw = 0.0f,
		float pitch = 0.5235988f, float alturaObjetivo = 2.0f,
		float velocidadAngular = 0.12f);

	void update(float dt, const SceneNode& target) override;
	glm::mat4 view() const override;

	glm::vec3 posicion() const;
	glm::vec3 objetivo() const;

	float distancia;
	float yaw;
	float pitch;
	float alturaObjetivo;
	float velocidadAngular;
};
