#pragma once
// Planeta océano (como el planeta de Miller en Interstellar)
//  - Por fuera: esfera de agua azul con brillo (Models/planetaOceano/planetaOceano.obj) y un halo
//    de atmósfera.
//  - Por dentro (sin animación, solo la textura del mar): al entrar se ve el océano abierto hasta el
//    horizonte; detrás está la ola gigante. El agua se curva hasta quedar vertical (la nave termina
//    mirando hacia arriba) y en la cima se sale a la pista.

#include <glew.h>
#include <glm.hpp>
#include "Espacio.h"   // UniformsEspacio

void InicializarOceano();

// --- por fuera, en el espacio ---
glm::vec3 CentroPlanetaOceano();
float RadioAtmosferaOceano();
void DibujarPlanetaOceano(float tiempo, const UniformsEspacio& u);

// --- por dentro ---
struct AmbienteOceano
{
	glm::vec3 colorNiebla;
	float densidadNiebla;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;
	glm::vec3 colorCenit;   // cielo despejado: color arriba (en el horizonte es el de la bruma)
	float cieloSolido;      // 1 = cielo despejado en lugar del cielo del espacio
};
AmbienteOceano ObtenerAmbienteOceano(float factorDia);

// Coordenadas dentro del océano: x (a lo ancho) y s (a lo largo del recorrido). Con s <= 0 se está
// sobre el mar plano (z = s); con s > 0, sobre la ola: primero una curva y luego una pared vertical.
void InicioOceano(float& x, float& s, float& yawGrados);   // de espaldas a la ola, viendo el mar abierto
void PuntoOla(float x, float s, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);
glm::vec2 LimitarOceano(float x, float s);                 // no se aleja demasiado (x, s)
glm::vec3 AjustarCamaraOceano(const glm::vec3& camara);    // la cámara no se mete al agua ni a la ola
bool LlegoALaCresta(const glm::vec3& posicion);            // en la cima de la ola: sale a la pista

// Recorrido de la nave rival: del inicio, derecho hacia la ola y hasta la cima
float LargoCarreraOceano();
void PuntoCarreraOceano(float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);

void DibujarOceano(const glm::vec3& posCamara, const UniformsEspacio& u);
