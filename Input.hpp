#pragma once

#include <glew.h>
#include <glm.hpp>

#include "Window.h"
#include "Camera.h"

struct EstadoEscena
{
	// Rover
	glm::vec3 roverPos;
	GLfloat   roverYaw;
	GLfloat   roverScale;
	GLfloat   roverSpeed;

	// Avion
	glm::vec3 avionPos;
	GLfloat   avionYaw;
	GLfloat   avionScale;
	GLfloat   avionSpeed;

	// Holocron
	glm::vec3 holocronPos;

	// Nave (practica 8)
	glm::vec3 navePos;
	GLfloat   naveYaw;
	GLfloat   naveScale;
	GLfloat   naveSpeed;

	// Luces: las teclas solo escriben estas banderas.
	unsigned int colorFaro;         
	bool holocronEncendida;
	bool pinzaEncendida;

	// Practica 8: nave con dos motores
	bool naveEncendida;     // N -> point light + spotlight de la nave
	bool motor1Encendido;   // 1 -> llama + point light del motor 1
	bool motor2Encendido;   // 2 -> llama + point light del motor 2
};

extern EstadoEscena escena;


void InitEscena(); // valores iniciales
void ProcesarTeclas(Window& ventana, Camera& camara, GLfloat delta, GLfloat objDelta);  // una vez por frame
