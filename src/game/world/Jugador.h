#pragma once
// Jugador (avatar) en tercera persona.
//  - Al iniciar es Paul (mientras se termina el avatar del equipo) y aparece en la base, junto a
//    la pista y antes de la meta. Ahí está estacionada su nave, miNave.
//  - Con K junto a miNave: la nave pasa a ser el avatar, aparece en la meta y empieza la carrera
//    (la nave rival también arranca en ese momento).
//  - Con la nave entra a los planetas y sigue la carrera contra la nave rival: sobre el camino del
//    planeta rocoso y del selvático (hasta su portal) y sobre el mar del planeta océano (subiendo la
//    ola gigante hasta su cima). Al salir aparece otra vez sobre la pista.
//  - En el rocoso y el selvático, K baja de la nave (queda estacionada) y K junto a ella vuelve a subir.
// La cámara orbita detrás del avatar con el mouse; W A S D lo mueven.

#include <glew.h>
#include <glm.hpp>

class Camera;

enum LugarJugador
{
	LUGAR_ESPACIO = 0,
	LUGAR_ROCOSO,
	LUGAR_SELVATICO,
	LUGAR_OCEANO
};

struct UniformsJugador
{
	GLuint model;
	GLuint color;
	GLuint especular;
	GLuint brillo;
};

void InicializarJugador();   // después de InicializarSuperficies() e InicializarEspacio()

// Las funciones que colocan al jugador regresan el yaw (grados) para que la cámara quede detrás
float ColocarJugadorEnBase();
bool IntentarAbordarNave(float tiempo, float& yawCamara);   // tecla K: true si estaba junto a miNave
float ColocarJugadorEnPlaneta(LugarJugador lugar, const glm::vec3& inicio, float yawInicioGrados);
float ColocarJugadorEnOceano();
float ColocarJugadorAlSalir(int planeta);   // PLANETA_ROCOSO, PLANETA_SELVATICO o PLANETA_OCEANO

// Tecla K dentro del planeta rocoso o selvático: si va en la nave, se baja y la nave queda
// estacionada ahí; si va a pie junto a su nave, se sube y sigue la carrera desde ese lugar.
bool BajarOSubirNave(LugarJugador lugar);
bool JugadorEnNave();   // el portal solo se cruza en la nave

bool CarreraIniciada();
float TiempoDeCarrera(float tiempo);   // segundos desde que empezó la carrera (0 mientras no empieza)

// Mueve al avatar con W A S D y pone la cámara detrás (solo con la cámara del avatar, no la libre)
void ActualizarJugador(bool* teclas, float dt, LugarJugador lugar, Camera& camara, float tiempo);
glm::vec3 PosicionJugador();

// Nave rival: dónde va en este momento (lugar, posición y orientación). Recorre la pista y
// también el camino de los planetas y el mar del océano; espera en la meta hasta que empieza la carrera.
void EstadoRival(float tiempo, LugarJugador& lugar, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);

// Dibuja al avatar y, si aún no se aborda, a miNave estacionada en la base
void DibujarJugador(LugarJugador lugar, float tiempo, const UniformsJugador& u);
