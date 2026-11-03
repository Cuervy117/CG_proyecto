#pragma once
// Espacio entre los planetas:
//  - Pista de carreras (modelo Models/pista/pista.obj) en tres tramos que ENTRAN a los planetas
//    (la carrera sigue dentro de cada planeta y sale por su portal o por la cresta de la ola):
//      tramo 0: sale del océano -> sube a la meta -> entra al planeta rocoso por abajo
//      tramo 1: sale del rocoso -> sube y cruza el cinturón -> entra al selvático por arriba
//      tramo 2: sale del selvático por abajo -> baja -> entra al planeta océano
//  - Cinturón de asteroides entre los dos planetas (asteroide.zip), cada uno girando.
//  - Cámara "sobre la pista": camina sobre el camino como en los planetas.

#include <glew.h>
#include <glm.hpp>

struct UniformsEspacio
{
	GLuint model;
	GLuint color;
	GLuint especular;
	GLuint brillo;
	GLuint emision;   // uniform vec3 emision de shader_light.frag
};

// Carga la pista y los asteroides. Llamar una vez, con OpenGL ya iniciado.
void InicializarEspacio();

// Pista (pista.obj), sus luces, la línea de meta y los arcos de entrada a cada atmósfera
void DibujarPista(float tiempo, const UniformsEspacio& u);

// Cinturón de asteroides (solo los que están frente a la cámara)
void DibujarCinturon(const glm::vec3& posCamara, const glm::vec3& dirCamara, float tiempo, const UniformsEspacio& u);

// Base de inicio: plataforma bajo la pista, antes de la meta (ahí aparece el jugador)
void DibujarBase(float tiempo, const UniformsEspacio& u);
// centro del piso de la base, dirección de la carrera, su derecha y el radio de la plataforma
void DatosBase(glm::vec3& centro, glm::vec3& adelante, glm::vec3& derecha, float& radio);

// Planetas a los que entra la pista
enum PlanetaPista
{
	PLANETA_ROCOSO = 0,
	PLANETA_SELVATICO,
	PLANETA_OCEANO
};

// Recorrido de la pista (lo usa la nave rival): tramos 0, 1 y 2, la meta está en el tramo 0
float LargoTramo(int tramo);
float DistanciaMeta();
void PuntoEnPista(int tramo, float s, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);

// Regresa la posición corregida para quedar sobre el camino, a 'altura' sobre el piso y sin
// salirse de sus orillas; 'adelante' y 'arriba' (opcionales) dan la orientación del camino ahí
glm::vec3 AjustarAPista(const glm::vec3& posicion, float altura, glm::vec3* adelante = nullptr, glm::vec3* arriba = nullptr);

// Lugar de salida en la meta para la nave del jugador ('carril' > 0 = a la derecha)
void SalidaEnMeta(glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba, float altura, float carril);

// Puntos de aparición (yaw y pitch en grados, como los usa la clase Camera):
// la meta al iniciar el programa y la salida de cada planeta al cruzar su portal
void InicioDePista(glm::vec3& posicion, float& yaw, float& pitch, float alturaOjos);
void SalidaDePlaneta(int planeta, glm::vec3& posicion, float& yaw, float& pitch, float alturaOjos);
