#pragma once

#include "CameraRig.h"

class FollowXZ : public CameraRig
{
public:
	explicit FollowXZ(float altura = 6.0f, float distancia = 12.0f,
		float alturaObjetivo = 2.0f);

	void update(float dt, const SceneNode& target) override;
	glm::mat4 view() const override;

	glm::vec3 posicion() const;
	glm::vec3 objetivo() const;

	float altura;
	float distancia;
	float alturaObjetivo;
};
