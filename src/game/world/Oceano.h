#pragma once
/*
NUEVO (no viene en las prácticas): planeta océano "Aquaris" (como el planeta de Miller en Interstellar)

¿Por qué hace falta este archivo?
 - Es el tercer planeta del escenario y es parte del circuito de la carrera: el tramo 0 de la pista
   (Espacio.cpp) EMPIEZA dentro de él y el tramo 2 TERMINA dentro de él.
 - Espacio.cpp usa CentroPlanetaOceano() y RadioAtmosferaOceano() para el arco de luz de la entrada
   y para saber dónde reaparece la nave al salir del océano.
 - Jugador.cpp usa PuntoOla(), LimitarOceano(), AjustarCamaraOceano(), LargoCarreraOceano() y
   PuntoCarreraOceano() para que el jugador y la nave rival vuelen sobre el agua y suban la ola.
 - El main lo usa para entrar al planeta (tecla E o cruzando su atmósfera), para salir por la cresta
   de la ola y para pintar su cielo despejado (ObtenerAmbienteOceano).
Si se borra, el proyecto ya no compila y la pista queda cortada a mitad del espacio.

Qué tiene:
 - Por fuera (en el espacio): esfera de agua (Models/planetaOceano/planetaOceano.obj) con un halo de
   atmósfera de tres capas transparentes.
 - Por dentro: mar plano hasta el horizonte y una ola gigante. El agua se curva hasta quedar vertical
   (la nave termina mirando hacia arriba) y en la cima se sale a la pista.
 - La textura del agua se desplaza con toffset (textura con movimiento, igual que en la práctica 9).

Coordenadas dentro del océano: x (a lo ancho) y s (a lo largo del recorrido). Con s <= 0 se está sobre
el mar plano (z = s); con s > 0, sobre la ola: primero una curva y luego una pared vertical.
*/

#include <vector>
#include <glew.h>
#include <glm.hpp>

#include "Model.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh_tn.h"

// Niebla y cielo del interior del océano según la hora del día
struct AmbienteOceano
{
	glm::vec3 colorNiebla;
	float densidadNiebla;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;
	glm::vec3 colorCenit;	// cielo despejado: color arriba (en el horizonte es el de la bruma)
	float cieloSolido;		// 1 = cielo despejado en lugar del cielo del espacio
};

class Oceano
{
public:
	Oceano();
	void InicializarOceano();	// carga el planeta y arma el mar y la ola (con OpenGL ya iniciado)

	// --- por fuera, en el espacio ---
	glm::vec3 CentroPlanetaOceano();
	float RadioAtmosferaOceano();
	void DibujarPlanetaOceano(float tiempo, GLuint modelLocation, GLuint colorLocation,
		GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation);

	// --- por dentro ---
	AmbienteOceano ObtenerAmbienteOceano(float factorDia);
	void InicioOceano(float& x, float& s, float& yawGrados);	// de espaldas a la ola, viendo el mar abierto
	void PuntoOla(float x, float s, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);
	glm::vec2 LimitarOceano(float x, float s);					// que no se aleje demasiado
	glm::vec3 AjustarCamaraOceano(glm::vec3 camara);			// la cámara no se mete al agua ni a la ola
	bool LlegoALaCresta(glm::vec3 posicion);					// en la cima de la ola: sale a la pista

	// Recorrido de la nave rival: del inicio, derecho hacia la ola y hasta la cima
	float LargoCarreraOceano();
	void PuntoCarreraOceano(float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);

	// toffset: desplazamiento de la textura del agua (textura con movimiento)
	void DibujarOceano(glm::vec3 posCamara, glm::vec2 toffset, GLuint modelLocation, GLuint colorLocation,
		GLuint offsetLocation, GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation);

	~Oceano();

private:
	void Perfil(float s, glm::vec2& p, glm::vec2& tangente, glm::vec2& normal);
	glm::vec3 En3D(glm::vec2 v);
	void AgregarVertice(std::vector<GLfloat>& v, glm::vec3 p, float s, float t, glm::vec3 n);
	void ConstruirOla();
	void ConstruirMar();

	Model PlanetaOceano_M;
	bool hayPlaneta;
	Texture aguaTexture;
	bool texturaAguaLista = false;
	Material Material_planeta;
	Material Material_agua;
	MeshModel* mar;
	MeshModel* ola;
	bool inicializado;
};

