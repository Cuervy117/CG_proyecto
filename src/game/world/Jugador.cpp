#include "Jugador.h"

#include <cmath>
#include <cstdio>
#include <fstream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

const float ALTURA_PAUL = 4.0f;				// altura del avatar en el mundo
const float LARGO_NAVE = 9.0f;				// largo de la nave del jugador en el mundo
const float ALTURA_NAVE = 1.0f;				// la nave flota un poco sobre la pista
const float ALTURA_NAVE_CAMINO = 1.6f;		// sobre el camino de los planetas
const float ALTURA_NAVE_AGUA = 2.4f;		// sobre el agua del océano
const float LARGO_NAVE_PLANETA = 7.0f;		// en los caminos de los planetas va un poco más chica
const float VELOCIDAD_RIVAL = 18.0f;
const float CARRIL_RIVAL_PISTA = -2.5f;
const float CARRIL_RIVAL_CAMINO = -1.2f;
const float CARRIL_RIVAL_AGUA = -8.0f;
const float CARRIL = 2.5f;					// el jugador sale por el carril derecho; la rival por el izquierdo
const float VELOCIDAD_PIE = 14.0f;
const float VELOCIDAD_NAVE_MAX = 36.0f;		// más rápida que la nave rival (18): se le puede ganar
const float DISTANCIA_PARA_ABORDAR = 7.0f;


Jugador::Jugador()
{
	superficie = nullptr;
	espacio = nullptr;
	oceano = nullptr;
	enNave = false;
	abordo = false;
	carrera = false;
	inicioCarrera = 0.0f;
	posicion = glm::vec3(0.0f);
	rumbo = 0.0f;
	pasos = 0.0f;
	velocidadNave = 0.0f;
	adelanteNave = glm::vec3(0.0f, 0.0f, 1.0f);
	arribaNave = glm::vec3(0.0f, 1.0f, 0.0f);
	oceanoXS = glm::vec2(0.0f);
	velocidadOceano = glm::vec2(0.0f);
	lugarActual = LUGAR_ESPACIO;
	naveEstacionada = false;
	lugarEstacionada = LUGAR_ESPACIO;
	posEstacionada = glm::vec3(0.0f);
	adelanteEstacionada = glm::vec3(0.0f, 0.0f, 1.0f);
	paul = nullptr;
	hayNave = false;
	Material_avatar = Material(0.6f, 32.0f);
	Material_nave = Material(2.0f, 128.0f);
}

// traslación que pone los pies del modelo en y = 0 y lo centra en x, z
glm::vec3 Jugador::PiesDelModelo(const Model& m)
{
	glm::vec3 c = m.getCentro(), t = m.getTamano();
	return glm::vec3(-c.x, -(c.y - 0.5f * t.y), -c.z);
}

// matriz con columnas (izquierda, arriba, adelante) y la posición: los modelos miran hacia +Z
// (como Orientar de la práctica 9-1)
glm::mat4 Jugador::Orientar(glm::vec3 p, glm::vec3 adelante, glm::vec3 arriba)
{
	glm::vec3 f = glm::normalize(adelante);
	glm::vec3 x = glm::normalize(glm::cross(arriba, f));
	glm::vec3 y = glm::cross(f, x);
	glm::mat4 m(1.0f);
	m[0] = glm::vec4(x, 0.0f);
	m[1] = glm::vec4(y, 0.0f);
	m[2] = glm::vec4(f, 0.0f);
	m[3] = glm::vec4(p, 1.0f);
	return m;
}

// la cámara mira hacia donde mira el avatar (yaw en grados, como lo usa la clase Camera)
float Jugador::YawCamaraDesde(float rumboAvatar)
{
	return glm::degrees(atan2f(cosf(rumboAvatar), sinf(rumboAvatar)));
}

// dónde está estacionada miNave en la base
void Jugador::LugarNaveEnBase(glm::vec3& p, glm::vec3& adelante)
{
	glm::vec3 centro, derecha;
	float radio;
	espacio->DatosBase(centro, adelante, derecha, radio);
	p = centro + derecha * (0.35f * radio) + adelante * 1.0f;
}

float Jugador::LargoNaveActual()
{
	return (lugarActual == LUGAR_ROCOSO || lugarActual == LUGAR_SELVATICO) ? LARGO_NAVE_PLANETA : LARGO_NAVE;
}

void Jugador::InicializarJugador(Superficie* superficiePlanetas, Espacio* espacioPista, Oceano* oceanoPlaneta)
{
	superficie = superficiePlanetas;
	espacio = espacioPista;
	oceano = oceanoPlaneta;

	// avatar: el del equipo si ya existe Models/avatar/avatar.obj; mientras, Paul (el mismo modelo del NPC).
	// NUEVO: como el archivo es opcional, primero se revisa si existe (así no sale un error en la consola)
	std::ifstream archivoAvatar("Models/avatar/avatar.obj");
	if (archivoAvatar.good())
	{
		AvatarPropio_M = Model();
		AvatarPropio_M.LoadModel("Models/avatar/avatar.obj");
	}
	if (AvatarPropio_M.estaCargado())
		paul = &AvatarPropio_M;
	else
		paul = superficie->ModeloPaul();
	if (paul == nullptr)
		printf("[AVISO] No hay modelo para el avatar (ni Models/avatar/avatar.obj ni Paul.obj)\n");

	printf("Cargando Models/Intergalactic_Spaceships_Version_2.obj ...\n");
	MiNave_M = Model();
	MiNave_M.LoadModel("Models/Intergalactic_Spaceships_Version_2.obj");
	hayNave = MiNave_M.estaCargado();
	if (hayNave)
		printf("[OK] Nave intergalactica: Models/Intergalactic_Spaceships_Version_2.obj (%d triangulos)\n", MiNave_M.getNumTriangulos());
}

float Jugador::ColocarJugadorEnBase()
{
	glm::vec3 centro, adelante, derecha;
	float radio;
	espacio->DatosBase(centro, adelante, derecha, radio);
	enNave = false;
	lugarActual = LUGAR_ESPACIO;
	posicion = centro - derecha * (0.35f * radio) - adelante * 2.0f;   // del otro lado de la base, frente a la nave
	glm::vec3 pNave, fNave;
	LugarNaveEnBase(pNave, fNave);
	glm::vec3 haciaNave = pNave - posicion;
	rumbo = atan2f(haciaNave.x, haciaNave.z);
	return YawCamaraDesde(rumbo);
}

// NUEVO: "entrar a la nave". Si Paul está junto a miNave (a menos de 7 unidades en el piso), la nave
// pasa a ser el avatar, aparece en la meta (carril derecho) y empieza la carrera: desde ese momento
// se cuenta el tiempo de la nave rival (TiempoDeCarrera).
bool Jugador::IntentarAbordarNave(float tiempo, float& yawCamara)
{
	if (abordo || enNave || !hayNave)
		return false;
	glm::vec3 pNave, fNave;
	LugarNaveEnBase(pNave, fNave);
	if (glm::length(glm::vec2(pNave.x - posicion.x, pNave.z - posicion.z)) > DISTANCIA_PARA_ABORDAR)
		return false;
	abordo = true;
	enNave = true;
	carrera = true;
	inicioCarrera = tiempo;
	velocidadNave = 0.0f;
	espacio->SalidaEnMeta(posicion, adelanteNave, arribaNave, ALTURA_NAVE, CARRIL);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
	yawCamara = YawCamaraDesde(rumbo);
	printf("A bordo de miNave: empieza la carrera\n");
	return true;
}

void Jugador::SeleccionarNave(float tiempo)
{
	if (!hayNave || (enNave && carrera)) return;
	if (!carrera) inicioCarrera = tiempo;
	enNave = abordo = carrera = true;
	naveEstacionada = false;
	velocidadNave = 0;
	espacio->SalidaEnMeta(posicion, adelanteNave, arribaNave, ALTURA_NAVE, CARRIL);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
}

bool Jugador::CarreraIniciada()
{
	return carrera;
}

float Jugador::TiempoDeCarrera(float tiempo)
{
	return carrera ? fmaxf(0.0f, tiempo - inicioCarrera) : 0.0f;
}

float Jugador::ColocarJugadorEnPlaneta(LugarJugador lugar, glm::vec3 inicio, float yawInicioGrados)
{
	lugarActual = lugar;
	if (!abordo)
	{
		// sin nave (solo pasa con la cámara libre): camina como Paul
		enNave = false;
		posicion = glm::vec3(inicio.x, 0.0f, inicio.z);
		glm::vec2 d(cosf(glm::radians(yawInicioGrados)), sinf(glm::radians(yawInicioGrados)));
		rumbo = atan2f(d.x, d.y);
		return yawInicioGrados;
	}
	// con nave: sigue la carrera sobre el camino del planeta
	naveEstacionada = false;
	enNave = true;
	velocidadNave = 0.0f;
	TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
	posicion = superficie->AjustarACamino(tipo, inicio, ALTURA_NAVE_CAMINO, adelanteNave);
	arribaNave = glm::vec3(0.0f, 1.0f, 0.0f);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
	return YawCamaraDesde(rumbo);
}

float Jugador::ColocarJugadorEnOceano()
{
	// en el océano siempre se va en nave; aparece de espaldas a la ola, viendo el mar abierto
	float yaw;
	lugarActual = LUGAR_OCEANO;
	oceano->InicioOceano(oceanoXS.x, oceanoXS.y, yaw);
	naveEstacionada = false;
	enNave = true;
	velocidadOceano = glm::vec2(0.0f);
	rumbo = 3.14159265f;   // mirando hacia -z
	glm::vec3 t, n;
	oceano->PuntoOla(oceanoXS.x, oceanoXS.y, ALTURA_NAVE_AGUA, posicion, t, n);
	adelanteNave = glm::vec3(0.0f, 0.0f, -1.0f);
	arribaNave = n;
	return yaw;
}

float Jugador::ColocarJugadorAlSalir(int planeta)
{
	if (!abordo)
		return ColocarJugadorEnBase();   // si aún no tenía nave, regresa a la base
	float yaw, pitch;
	lugarActual = LUGAR_ESPACIO;
	naveEstacionada = false;
	enNave = true;
	velocidadNave = 0.0f;
	espacio->SalidaDePlaneta(planeta, posicion, yaw, pitch, ALTURA_NAVE);
	posicion = espacio->AjustarAPista(posicion, ALTURA_NAVE, adelanteNave, arribaNave);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
	return yaw;
}

// NUEVO (animación compleja): la nave rival recorre la vuelta completa con velocidad constante.
// d = VELOCIDAD_RIVAL * (segundos de carrera), módulo el largo total de la vuelta; luego se busca en
// qué etapa va (pista, planeta rocoso, pista, selvático, pista, océano, pista) y su punto ahí.
void Jugador::EstadoRival(RivalCarrera rival, float tiempo, LugarJugador& lugar, glm::vec3& pos, glm::vec3& adelante, glm::vec3& arriba)
{
	// vuelta completa: meta -> rocoso -> pista -> selvático -> pista -> océano -> pista -> meta
	float meta = espacio->DistanciaMeta();
	float largos[7] = {
		espacio->LargoTramo(0) - meta,								// 0 pista: meta -> entrada del rocoso
		superficie->LargoCarreraSuperficie(SUPERFICIE_ROCOSA),		// 1 camino del planeta rocoso hasta su portal
		espacio->LargoTramo(1),										// 2 pista: rocoso -> selvático
		superficie->LargoCarreraSuperficie(SUPERFICIE_SELVATICA),	// 3 camino del planeta selvático hasta su portal
		espacio->LargoTramo(2),										// 4 pista: selvático -> océano
		oceano->LargoCarreraOceano(),								// 5 sobre el mar hasta la cresta de la ola
		meta };														// 6 pista: océano -> meta
	float total = 0.0f;
	for (int i = 0; i < 7; i++)
		total += largos[i];
	// Parrilla escalonada y carriles opuestos. El desfase se conserva al cruzar la meta.
	const bool segundo = rival == RivalCarrera::Segundo;
	const float lado = segundo ? -1.0f : 1.0f;
	const float desfase = segundo ? 18.0f : 6.0f;
	float d = fmodf(desfase + VELOCIDAD_RIVAL * TiempoDeCarrera(tiempo), fmaxf(total, 1.0f));
	int etapa = 0;
	while (etapa < 6 && d >= largos[etapa])
	{
		d -= largos[etapa];
		etapa++;
	}
	arriba = glm::vec3(0.0f, 1.0f, 0.0f);
	if (etapa == 0)
	{
		lugar = LUGAR_ESPACIO;
		espacio->PuntoEnPista(0, meta + d, lado * CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba);
	}
	else if (etapa == 1)
	{
		lugar = LUGAR_ROCOSO;
		superficie->PuntoCarreraSuperficie(SUPERFICIE_ROCOSA, d, lado * CARRIL_RIVAL_CAMINO, ALTURA_NAVE_CAMINO, pos, adelante);
	}
	else if (etapa == 2)
	{
		lugar = LUGAR_ESPACIO;
		espacio->PuntoEnPista(1, d, lado * CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba);
	}
	else if (etapa == 3)
	{
		lugar = LUGAR_SELVATICO;
		superficie->PuntoCarreraSuperficie(SUPERFICIE_SELVATICA, d, lado * CARRIL_RIVAL_CAMINO, ALTURA_NAVE_CAMINO, pos, adelante);
	}
	else if (etapa == 4)
	{
		lugar = LUGAR_ESPACIO;
		espacio->PuntoEnPista(2, d, lado * CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba);
	}
	else if (etapa == 5)
	{
		lugar = LUGAR_OCEANO;
		oceano->PuntoCarreraOceano(d, lado * CARRIL_RIVAL_AGUA, ALTURA_NAVE_AGUA, pos, adelante, arriba);
	}
	else
	{
		lugar = LUGAR_ESPACIO;
		espacio->PuntoEnPista(0, d, lado * CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba);
	}
}

bool Jugador::BajarOSubirNave(LugarJugador lugar)
{
	if (lugar != LUGAR_ROCOSO && lugar != LUGAR_SELVATICO)
		return false;   // en el espacio se usa la base; en el océano no se puede bajar al agua
	TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
	if (enNave)
	{
		// se baja: la nave queda estacionada donde está y Paul aparece a un lado
		naveEstacionada = true;
		lugarEstacionada = lugar;
		posEstacionada = posicion;
		adelanteEstacionada = adelanteNave;
		glm::vec3 derecha = glm::normalize(glm::cross(adelanteNave, glm::vec3(0.0f, 1.0f, 0.0f)));
		enNave = false;
		posicion = superficie->LimitarPosicion(tipo, glm::vec3(posicion.x, 0.0f, posicion.z) + derecha * 4.5f);
		posicion.y = 0.0f;
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		printf("Bajaste de miNave: J junto a ella para volver a subir\n");
		return true;
	}
	if (naveEstacionada && lugarEstacionada == lugar)
	{
		if (glm::length(glm::vec2(posEstacionada.x - posicion.x, posEstacionada.z - posicion.z)) > DISTANCIA_PARA_ABORDAR)
		{
			printf("Acercate a tu nave para subir\n");
			return false;
		}
		// se sube y sigue la carrera desde donde dejó la nave
		enNave = true;
		naveEstacionada = false;
		posicion = posEstacionada;
		adelanteNave = adelanteEstacionada;
		arribaNave = glm::vec3(0.0f, 1.0f, 0.0f);
		velocidadNave = 0.0f;
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		return true;
	}
	return false;
}

bool Jugador::JugadorEnNave()
{
	return enNave;
}

glm::vec3 Jugador::PosicionJugador()
{
	return posicion;
}

void Jugador::PuntoFaroNave(glm::vec3& posicionFaro, glm::vec3& direccionFaro)
{
	// al frente de la nave y un poco arriba; alumbra hacia adelante y un poco hacia el piso
	posicionFaro = posicion + adelanteNave * (0.5f * LargoNaveActual()) + arribaNave * 0.6f;
	direccionFaro = glm::normalize(adelanteNave - arribaNave * 0.25f);
}

void Jugador::ActualizarJugador(bool* keys, GLfloat deltaTime, LugarJugador lugar, Camera& camara, float tiempo, bool vueloLibre)
{
	lugarActual = lugar;
	// las teclas se leen del arreglo keys de Window (como Camera::keyControl)
	glm::vec3 frente = camara.getCameraDirection();
	glm::vec3 adelanteH(frente.x, 0.0f, frente.z);
	adelanteH = (glm::length(adelanteH) < 1.0e-3f) ? glm::vec3(sinf(rumbo), 0.0f, cosf(rumbo)) : glm::normalize(adelanteH);
	glm::vec3 derechaH = glm::normalize(glm::cross(adelanteH, glm::vec3(0.0f, 1.0f, 0.0f)));
	float ejeAdelante = (keys[GLFW_KEY_W] ? 1.0f : 0.0f) - (keys[GLFW_KEY_S] ? 1.0f : 0.0f);
	float ejeLado = (keys[GLFW_KEY_D] ? 1.0f : 0.0f) - (keys[GLFW_KEY_A] ? 1.0f : 0.0f);
	float ejeVertical = (keys[GLFW_KEY_E] ? 1.0f : 0.0f) - (keys[GLFW_KEY_Q] ? 1.0f : 0.0f);
	float distancia, alturaMira;

	if (vueloLibre && lugar == LUGAR_ESPACIO && enNave)
	{
		adelanteNave = glm::normalize(frente);
		arribaNave = glm::normalize(glm::cross(derechaH, adelanteNave));
		posicion += (adelanteNave * ejeAdelante + derechaH * ejeLado
			+ glm::vec3(0.0f, 1.0f, 0.0f) * ejeVertical) * VELOCIDAD_NAVE_MAX * deltaTime;
		rumbo = atan2f(frente.x, frente.z);
		distancia = 15.0f;
		alturaMira = 3.0f;
	}
	else if (lugar == LUGAR_ESPACIO && enNave)
	{
		// nave sobre la pista: W acelera hacia donde va la carrera, S frena y va en reversa, A/D cambian de carril
		glm::vec3 adelante, arriba;
		espacio->AjustarAPista(posicion, ALTURA_NAVE, adelante, arriba);
		glm::vec3 derecha = glm::normalize(glm::cross(adelante, arriba));
		float objetivo = (ejeAdelante > 0.0f) ? VELOCIDAD_NAVE_MAX : ((ejeAdelante < 0.0f) ? -12.0f : 0.0f);
		velocidadNave += (objetivo - velocidadNave) * fminf(1.0f, 1.8f * deltaTime);
		posicion += adelante * (velocidadNave * deltaTime) + derecha * (ejeLado * 10.0f * deltaTime);
		posicion = espacio->AjustarAPista(posicion, ALTURA_NAVE, adelanteNave, arribaNave);
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		distancia = 15.0f;
		alturaMira = 2.5f;
	}
	else if ((lugar == LUGAR_ROCOSO || lugar == LUGAR_SELVATICO) && enNave)
	{
		// nave sobre el camino del planeta: igual que en la pista (W, S y A/D para moverse de lado)
		TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
		glm::vec3 adelante;
		superficie->AjustarACamino(tipo, posicion, ALTURA_NAVE_CAMINO, adelante);
		glm::vec3 derecha = glm::normalize(glm::cross(adelante, glm::vec3(0.0f, 1.0f, 0.0f)));
		float objetivo = (ejeAdelante > 0.0f) ? VELOCIDAD_NAVE_MAX : ((ejeAdelante < 0.0f) ? -12.0f : 0.0f);
		velocidadNave += (objetivo - velocidadNave) * fminf(1.0f, 1.8f * deltaTime);
		posicion += adelante * (velocidadNave * deltaTime) + derecha * (ejeLado * 8.0f * deltaTime);
		posicion = superficie->AjustarACamino(tipo, posicion, ALTURA_NAVE_CAMINO, adelanteNave);
		arribaNave = glm::vec3(0.0f, 1.0f, 0.0f);
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		distancia = 13.0f;
		alturaMira = 2.5f;
	}
	else if (lugar == LUGAR_OCEANO)
	{
		if (oceanoXS.y <= 0.0f)
		{
			// mar plano: la nave va hacia donde mira la cámara (W) o de lado (A/D)
			glm::vec3 dir = adelanteH * ejeAdelante + derechaH * ejeLado;
			glm::vec2 objetivo(0.0f);
			if (glm::length(dir) > 0.0f)
			{
				dir = glm::normalize(dir);
				objetivo = glm::vec2(dir.x, dir.z) * VELOCIDAD_NAVE_MAX;   // (x, s): en el mar plano s = z
			}
			velocidadOceano += (objetivo - velocidadOceano) * fminf(1.0f, 1.6f * deltaTime);
			if (glm::length(velocidadOceano) > 1.0f)
			{
				float nuevo = atan2f(velocidadOceano.x, velocidadOceano.y);
				rumbo += atan2f(sinf(nuevo - rumbo), cosf(nuevo - rumbo)) * fminf(1.0f, 6.0f * deltaTime);
			}
		}
		else
		{
			// sobre la ola: W sube, S baja, A/D de lado; si no acelera, resbala hacia abajo
			float objetivoS = (ejeAdelante > 0.0f) ? VELOCIDAD_NAVE_MAX : ((ejeAdelante < 0.0f) ? -0.5f * VELOCIDAD_NAVE_MAX : -10.0f);
			velocidadOceano.y += (objetivoS - velocidadOceano.y) * fminf(1.0f, 1.6f * deltaTime);
			velocidadOceano.x += (ejeLado * 10.0f - velocidadOceano.x) * fminf(1.0f, 3.0f * deltaTime);
			rumbo += atan2f(sinf(0.0f - rumbo), cosf(0.0f - rumbo)) * fminf(1.0f, 4.0f * deltaTime);   // de frente a la ola
		}
		oceanoXS += velocidadOceano * deltaTime;
		oceanoXS = oceano->LimitarOceano(oceanoXS.x, oceanoXS.y);
		// posición y orientación sobre el agua: en la pared de la ola la nave queda mirando hacia arriba
		glm::vec3 tangente, normal;
		oceano->PuntoOla(oceanoXS.x, oceanoXS.y, ALTURA_NAVE_AGUA, posicion, tangente, normal);
		arribaNave = normal;
		glm::vec3 direccion = glm::vec3(1.0f, 0.0f, 0.0f) * sinf(rumbo) + tangente * cosf(rumbo);
		adelanteNave = glm::normalize(direccion);
		distancia = 16.0f;
		alturaMira = 4.0f;
	}
	else
	{
		// a pie: W A S D según hacia dónde mira la cámara; Paul voltea hacia donde camina
		glm::vec3 mov = adelanteH * ejeAdelante + derechaH * ejeLado;
		if (glm::length(mov) > 0.0f)
		{
			mov = glm::normalize(mov);
			posicion += mov * (VELOCIDAD_PIE * deltaTime);
			float objetivo = atan2f(mov.x, mov.z);
			float diferencia = atan2f(sinf(objetivo - rumbo), cosf(objetivo - rumbo));
			rumbo += diferencia * fminf(1.0f, 10.0f * deltaTime);
			pasos += deltaTime * 9.0f;
		}
		if (lugar == LUGAR_ESPACIO)
		{
			// en la base: no se sale de la plataforma
			glm::vec3 centro, adelante, derecha;
			float radio;
			espacio->DatosBase(centro, adelante, derecha, radio);
			glm::vec2 d(posicion.x - centro.x, posicion.z - centro.z);
			float maximo = radio - 1.5f;
			if (glm::length(d) > maximo) d = glm::normalize(d) * maximo;
			posicion = glm::vec3(centro.x + d.x, centro.y, centro.z + d.y);
		}
		else
		{
			// en el planeta: zona del camino y obstáculos (troncos, rocas, cristales...)
			TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
			posicion = superficie->LimitarPosicion(tipo, posicion);
			posicion.y = 0.0f;
			if (naveEstacionada && lugarEstacionada == lugar)
			{
				glm::vec2 d(posicion.x - posEstacionada.x, posicion.z - posEstacionada.z);
				if (glm::length(d) < 3.5f && glm::length(d) > 1.0e-3f)
				{
					d = glm::normalize(d) * 3.5f;
					posicion.x = posEstacionada.x + d.x;
					posicion.z = posEstacionada.z + d.y;
				}
			}
		}
		distancia = 9.0f;
		alturaMira = 3.2f;
	}

	// NUEVO: cámara en tercera persona. Se pone 'distancia' unidades detrás del avatar en la
	// dirección contraria a la que mira la cámara (el mouse la hace orbitar alrededor de él).
	glm::vec3 arribaMira = (lugar == LUGAR_OCEANO) ? arribaNave : glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 objetivo = posicion + arribaMira * alturaMira;
	glm::vec3 posCamara = objetivo - frente * distancia;
	float piso = -1.0e9f;
	if (lugar == LUGAR_ESPACIO && !enNave) piso = posicion.y + 0.8f;   // piso de la base
	else if (lugar != LUGAR_ESPACIO) piso = 0.8f;
	if (posCamara.y < piso) posCamara.y = piso;
	if (lugar == LUGAR_OCEANO)
		posCamara = oceano->AjustarCamaraOceano(posCamara);   // no se mete al agua ni a la ola
	camara.setPosition(posCamara);
}

void Jugador::DibujarJugador(LugarJugador lugar, float tiempo, GLuint modelLocation, GLuint colorLocation,
	GLuint specularIntensityLocation, GLuint shininessLocation)
{
	glm::mat4 model(1.0);
	glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);

	// miNave: estacionada en la base (flotando suave) o como avatar en la pista, los caminos o el mar
	bool estacionada = (lugar == LUGAR_ESPACIO && !abordo);
	if (hayNave && (estacionada || enNave))
	{
		glm::vec3 p = posicion, adelante = adelanteNave, arriba = arribaNave;
		if (estacionada && !enNave)
		{
			LugarNaveEnBase(p, adelante);
			arriba = glm::vec3(0.0f, 1.0f, 0.0f);
			p.y += 0.6f + 0.25f * sinf(1.5f * tiempo);   // NUEVO (animación continua): flota, y = 0.6 + 0.25 sen(1.5 t)
		}
		float largo = (lugar == LUGAR_ROCOSO || lugar == LUGAR_SELVATICO) ? LARGO_NAVE_PLANETA : LARGO_NAVE;
		float escala = largo / fmaxf(MiNave_M.getTamano().z, 0.001f);
		model = Orientar(p, adelante, arriba);
		model = glm::scale(model, glm::vec3(escala));
		model = glm::translate(model, PiesDelModelo(MiNave_M));
		Material_nave.UseMaterial(specularIntensityLocation, shininessLocation);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		MiNave_M.RenderModel();
	}

	// nave estacionada en el planeta mientras el jugador camina (flota: y += 0.2 sen(1.5 t))
	if (naveEstacionada && lugar == lugarEstacionada && hayNave)
	{
		float escala = LARGO_NAVE_PLANETA / fmaxf(MiNave_M.getTamano().z, 0.001f);
		glm::vec3 p = posEstacionada + glm::vec3(0.0f, 0.2f * sinf(1.5f * tiempo), 0.0f);
		model = Orientar(p, adelanteEstacionada, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escala));
		model = glm::translate(model, PiesDelModelo(MiNave_M));
		Material_nave.UseMaterial(specularIntensityLocation, shininessLocation);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		MiNave_M.RenderModel();
	}

	// Paul: el avatar a pie, con un pequeño rebote al caminar (y = 0.12 |sen(pasos)|) y un balanceo
	if (!enNave && !avatarArticulado && paul != nullptr)
	{
		float escala = ALTURA_PAUL / fmaxf(paul->getTamano().y, 0.001f);
		model = glm::mat4(1.0);
		model = glm::translate(model, posicion + glm::vec3(0.0f, 0.12f * fabsf(sinf(pasos)), 0.0f));
		model = glm::rotate(model, rumbo, glm::vec3(0.0f, 1.0f, 0.0f));		// ángulos en radianes
		model = glm::rotate(model, 0.05f * sinf(pasos), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escala));
		model = glm::translate(model, PiesDelModelo(*paul));
		Material_avatar.UseMaterial(specularIntensityLocation, shininessLocation);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		paul->RenderModel();
	}
}

Jugador::~Jugador()
{
}

