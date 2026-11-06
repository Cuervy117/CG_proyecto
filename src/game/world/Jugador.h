#pragma once
/*
NUEVO (no viene en las prácticas): jugador (avatar) en tercera persona y nave rival
 - Al iniciar el avatar es Paul y aparece en la base, junto a la pista y antes de la meta. Ahí está
   estacionada su nave, miNave (flotando con un seno).
 - "Entrar a la nave" (tecla K junto a miNave): la nave pasa a ser el avatar, aparece en la meta y
   empieza la carrera; la nave rival arranca en ese mismo momento.
 - Con la nave se entra a los planetas y la carrera sigue contra la nave rival: sobre el camino del
   planeta rocoso y del selvático (hasta su portal) y sobre el mar del planeta océano (subiendo la
   ola gigante hasta su cima). Al salir aparece otra vez sobre la pista.
 - En el rocoso y el selvático, K baja de la nave (queda estacionada) y K junto a ella vuelve a subir.
 - La cámara orbita detrás del avatar con el mouse; W A S D lo mueven (se leen del arreglo keys de
   Window, igual que Camera::keyControl).
 - La nave rival recorre toda la vuelta (pista -> planetas -> pista) con una velocidad constante:
   su posición se calcula con la distancia recorrida (animación compleja basada en una función).
*/

#include <glew.h>
#include <glfw3.h>
#include <glm.hpp>

#include "Camera.h"
#include "Model.h"
#include "Material.h"
#include "Espacio.h"
#include "Superficie.h"
#include "Oceano.h"

enum LugarJugador
{
	LUGAR_ESPACIO = 0,
	LUGAR_ROCOSO,
	LUGAR_SELVATICO,
	LUGAR_OCEANO
};

enum class RivalCarrera { Primero, Segundo };

class Jugador
{
public:
	Jugador();
	// Después de InicializarSuperficies() e InicializarEspacio(): usa a Paul y la base de inicio
	void InicializarJugador(Superficie* superficiePlanetas, Espacio* espacioPista, Oceano* oceanoPlaneta);

	// Las funciones que colocan al jugador regresan el yaw (grados) para que la cámara quede detrás
	float ColocarJugadorEnBase();
	bool IntentarAbordarNave(float tiempo, float& yawCamara);	// tecla K: true si estaba junto a miNave
	float ColocarJugadorEnPlaneta(LugarJugador lugar, glm::vec3 inicio, float yawInicioGrados);
	float ColocarJugadorEnOceano();
	float ColocarJugadorAlSalir(int planeta);	// PLANETA_ROCOSO, PLANETA_SELVATICO o PLANETA_OCEANO

	// Tecla K dentro del planeta rocoso o selvático: si va en la nave se baja (la nave queda
	// estacionada); si va a pie junto a su nave, se sube y sigue la carrera desde ese lugar.
	bool BajarOSubirNave(LugarJugador lugar);
	bool JugadorEnNave();
	float Rumbo() const { return rumbo; }
	bool avatarArticulado = false;	// el portal solo se cruza en la nave

	void SeleccionarNave(float tiempo);
	bool CarreraIniciada();
	float TiempoDeCarrera(float tiempo);	// segundos desde que empezó la carrera (0 si no ha empezado)

	// Mueve al avatar con W A S D y pone la cámara detrás (solo con la cámara del avatar)
	void ActualizarJugador(bool* keys, GLfloat deltaTime, LugarJugador lugar, Camera& camara, float tiempo, bool vueloLibre = false);
	glm::vec3 PosicionJugador();
	// Posición y dirección del faro de miNave (spotlight que se prende y apaga con la tecla L)
	void PuntoFaroNave(glm::vec3& posicion, glm::vec3& direccion);

	// Nave rival: dónde va en este momento (lugar, posición y orientación). Espera en la meta
	// hasta que empieza la carrera.
	void EstadoRival(RivalCarrera rival, float tiempo, LugarJugador& lugar, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);

	// Dibuja al avatar y, si aún no se aborda, a miNave estacionada en la base
	void DibujarJugador(LugarJugador lugar, float tiempo, GLuint modelLocation, GLuint colorLocation,
		GLuint specularIntensityLocation, GLuint shininessLocation);

	~Jugador();

private:
	glm::vec3 PiesDelModelo(const Model& m);	// traslación que deja los pies del modelo en y = 0
	glm::mat4 Orientar(glm::vec3 p, glm::vec3 adelante, glm::vec3 arriba);
	float YawCamaraDesde(float rumboAvatar);
	void LugarNaveEnBase(glm::vec3& p, glm::vec3& adelante);
	float LargoNaveActual();

	Superficie* superficie;
	Espacio* espacio;
	Oceano* oceano;

	bool enNave;				// false = Paul a pie, true = miNave
	bool abordo;				// ya se subió a miNave al menos una vez (empezó la carrera)
	bool carrera;
	float inicioCarrera;
	glm::vec3 posicion;
	float rumbo;				// hacia dónde mira el avatar (radianes; 0 = +Z)
	float pasos;				// fase de la caminata
	float velocidadNave;
	glm::vec3 adelanteNave, arribaNave;
	glm::vec2 oceanoXS;			// en el océano: (x, s); s <= 0 es el mar plano, s > 0 la ola
	glm::vec2 velocidadOceano;	// velocidad en (x, s)
	LugarJugador lugarActual;	// dónde se actualizó por última vez (para el faro)

	// nave estacionada en un planeta mientras el jugador camina (tecla K)
	bool naveEstacionada;
	LugarJugador lugarEstacionada;
	glm::vec3 posEstacionada, adelanteEstacionada;

	Model* paul;
	Model AvatarPropio_M;
	Model MiNave_M;			// miNave (la nave del jugador)
	bool hayNave;
	Material Material_avatar;
	Material Material_nave;
};

