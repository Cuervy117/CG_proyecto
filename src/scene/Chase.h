#pragma once

#include "CameraRig.h"

class Chase : public CameraRig
{
public:
	explicit Chase(float distancia = 10.0f, float altura = 5.0f,
		float alturaObjetivo = 2.0f, float adelanto = 2.0f);

	void update(float dt, const SceneNode& target) override;
	glm::mat4 view() const override;

	glm::vec3 posicion() const;
	glm::vec3 objetivo() const;

	float distancia;
	float altura;
	float alturaObjetivo;
	float adelanto;
};
