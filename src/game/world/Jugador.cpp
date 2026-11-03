#include "Jugador.h"

#include <cmath>
#include <cstdio>
#include <fstream>

#include <glfw3.h>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Camera.h"
#include "Model.h"
#include "Texture.h"
#include "Material.h"
#include "Espacio.h"
#include "Superficie.h"
#include "Oceano.h"

namespace
{
	const char* RUTA_AVATAR = "Models/avatar/avatar.obj";             // el avatar del equipo, cuando esté listo
	const char* RUTA_NAVE = "Models/miNave/miNave.obj";
	const char* RUTA_TEXTURA_NAVE = "Textures/miNaveTexture.png";
	const char* RUTA_TEXTURA_NAVE_CARPETA = "Models/miNave/miNaveTexture.png";

	const float ALTURA_PAUL = 4.0f;          // altura del avatar en el mundo
	const float LARGO_NAVE = 7.0f;           // largo de miNave en el mundo
	const float ALTURA_NAVE = 1.0f;          // la nave flota un poco sobre la pista
	const float ALTURA_NAVE_CAMINO = 1.6f;   // sobre el camino de los planetas
	const float ALTURA_NAVE_AGUA = 2.4f;     // sobre el agua del océano
	const float LARGO_NAVE_PLANETA = 5.5f;   // en los caminos de los planetas va un poco más chica
	const float VELOCIDAD_RIVAL = 18.0f;
	const float CARRIL_RIVAL_PISTA = -2.5f, CARRIL_RIVAL_CAMINO = -1.2f, CARRIL_RIVAL_AGUA = -8.0f;
	const float CARRIL = 2.5f;               // el jugador sale por el carril derecho; la rival por el izquierdo
	const float VELOCIDAD_PIE = 14.0f;
	const float VELOCIDAD_NAVE_MAX = 36.0f;  // más rápida que la nave rival (18): se le puede ganar
	const float DISTANCIA_PARA_ABORDAR = 7.0f;

	enum Avatar { AV_PAUL, AV_NAVE };
	Avatar avatar = AV_PAUL;
	bool abordo = false;
	bool carrera = false;
	float inicioCarrera = 0.0f;
	glm::vec3 posicion(0.0f);
	float rumbo = 0.0f;                      // hacia dónde mira el avatar (radianes; 0 = +Z)
	float pasos = 0.0f;                      // fase de la caminata
	float velocidadNave = 0.0f;
	glm::vec3 adelanteNave(0.0f, 0.0f, 1.0f), arribaNave(0.0f, 1.0f, 0.0f);
	glm::vec2 oceano(0.0f);                  // en el océano: (x, s); s <= 0 es el mar plano, s > 0 la ola
	glm::vec2 velocidadOceano(0.0f);         // velocidad en (x, s)

	// nave estacionada en un planeta mientras el jugador camina (tecla K)
	bool naveEstacionada = false;
	LugarJugador lugarEstacionada = LUGAR_ESPACIO;
	glm::vec3 posEstacionada(0.0f), adelanteEstacionada(0.0f, 0.0f, 1.0f);

	Model* paul = nullptr;
	Model avatarPropio;
	Model nave;
	bool hayNave = false;
	Texture texNave;
	Material matAvatar(0.6f, 32.0f);
	Material matNave(2.0f, 128.0f);

	bool existe(const char* ruta)
	{
		std::ifstream f(ruta);
		return f.good();
	}

	glm::mat4 base(const Model& m)   // pies en y = 0 y centrado en x, z
	{
		glm::vec3 c = m.getCentro(), t = m.getTamano();
		return glm::translate(glm::mat4(1.0f), glm::vec3(-c.x, -(c.y - 0.5f * t.y), -c.z));
	}

	// matriz con columnas (izquierda, arriba, adelante): los modelos miran hacia +Z
	glm::mat4 orientar(const glm::vec3& p, const glm::vec3& adelante, const glm::vec3& arriba)
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

	float yawCamaraDesde(float rumboAvatar)   // la cámara mira hacia donde mira el avatar
	{
		return glm::degrees(atan2f(cosf(rumboAvatar), sinf(rumboAvatar)));
	}

	// dónde está estacionada miNave en la base
	void lugarNaveEnBase(glm::vec3& p, glm::vec3& adelante)
	{
		glm::vec3 centro, derecha;
		float radio;
		DatosBase(centro, adelante, derecha, radio);
		p = centro + derecha * (0.35f * radio) + adelante * 1.0f;
	}
}

void InicializarJugador()
{
	// avatar: el del equipo si ya existe Models/avatar/avatar.obj; mientras, Paul (el mismo modelo del NPC)
	if (existe(RUTA_AVATAR))
	{
		avatarPropio.LoadModel(RUTA_AVATAR);
		if (avatarPropio.estaCargado()) paul = &avatarPropio;
	}
	if (paul == nullptr)
		paul = ModeloPaul();
	if (paul == nullptr)
		printf("[AVISO] No hay modelo para el avatar (ni Models/avatar/avatar.obj ni Paul.obj)\n");

	if (existe(RUTA_NAVE))
	{
		printf("Cargando %s ...\n", RUTA_NAVE);
		nave.LoadModel(RUTA_NAVE);
		hayNave = nave.estaCargado();
		if (hayNave && !nave.usaTexturaPropia())
		{
			const char* ruta = existe(RUTA_TEXTURA_NAVE) ? RUTA_TEXTURA_NAVE : RUTA_TEXTURA_NAVE_CARPETA;
			texNave = Texture(ruta);
			if (texNave.LoadTextureA()) nave.SetTexturaRespaldo(&texNave);
		}
		if (hayNave) printf("[OK] miNave: %s (%d triangulos)\n", RUTA_NAVE, nave.getNumTriangulos());
	}
	else
		printf("[ERROR] No se encontro %s\n", RUTA_NAVE);
}

float ColocarJugadorEnBase()
{
	glm::vec3 centro, adelante, derecha;
	float radio;
	DatosBase(centro, adelante, derecha, radio);
	avatar = AV_PAUL;
	posicion = centro - derecha * (0.35f * radio) - adelante * 2.0f;   // del otro lado de la base, frente a la nave
	glm::vec3 pNave, fNave;
	lugarNaveEnBase(pNave, fNave);
	glm::vec3 haciaNave = pNave - posicion;
	rumbo = atan2f(haciaNave.x, haciaNave.z);
	return yawCamaraDesde(rumbo);
}

bool IntentarAbordarNave(float tiempo, float& yawCamara)
{
	if (abordo || avatar != AV_PAUL || !hayNave)
		return false;
	glm::vec3 pNave, fNave;
	lugarNaveEnBase(pNave, fNave);
	if (glm::length(glm::vec2(pNave.x - posicion.x, pNave.z - posicion.z)) > DISTANCIA_PARA_ABORDAR)
		return false;
	// sube a miNave: aparece en la meta (carril derecho) y empieza la carrera
	abordo = true;
	avatar = AV_NAVE;
	carrera = true;
	inicioCarrera = tiempo;
	velocidadNave = 0.0f;
	SalidaEnMeta(posicion, adelanteNave, arribaNave, ALTURA_NAVE, CARRIL);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
	yawCamara = yawCamaraDesde(rumbo);
	printf("A bordo de miNave: empieza la carrera\n");
	return true;
}

bool CarreraIniciada()
{
	return carrera;
}

float TiempoDeCarrera(float tiempo)
{
	return carrera ? fmaxf(0.0f, tiempo - inicioCarrera) : 0.0f;
}

float ColocarJugadorEnPlaneta(LugarJugador lugar, const glm::vec3& inicio, float yawInicioGrados)
{
	if (!abordo)
	{
		// sin nave (solo pasa con la cámara libre): camina como Paul
		avatar = AV_PAUL;
		posicion = glm::vec3(inicio.x, 0.0f, inicio.z);
		glm::vec2 d(cosf(glm::radians(yawInicioGrados)), sinf(glm::radians(yawInicioGrados)));
		rumbo = atan2f(d.x, d.y);
		return yawInicioGrados;
	}
	// con nave: sigue la carrera sobre el camino del planeta
	naveEstacionada = false;
	avatar = AV_NAVE;
	velocidadNave = 0.0f;
	TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
	posicion = AjustarACamino(tipo, inicio, ALTURA_NAVE_CAMINO, &adelanteNave);
	arribaNave = glm::vec3(0.0f, 1.0f, 0.0f);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
	return yawCamaraDesde(rumbo);
}

float ColocarJugadorEnOceano()
{
	// en el océano siempre se va en nave; aparece de espaldas a la ola, viendo el mar abierto
	float yaw;
	InicioOceano(oceano.x, oceano.y, yaw);
	naveEstacionada = false;
	avatar = AV_NAVE;
	velocidadOceano = glm::vec2(0.0f);
	rumbo = 3.14159265f;   // mirando hacia -z
	glm::vec3 t, n;
	PuntoOla(oceano.x, oceano.y, ALTURA_NAVE_AGUA, posicion, t, n);
	adelanteNave = glm::vec3(0.0f, 0.0f, -1.0f);
	arribaNave = n;
	return yaw;
}

float ColocarJugadorAlSalir(int planeta)
{
	if (!abordo)
		return ColocarJugadorEnBase();   // si aún no tenía nave, regresa a la base
	float yaw, pitch;
	naveEstacionada = false;
	avatar = AV_NAVE;
	velocidadNave = 0.0f;
	SalidaDePlaneta(planeta, posicion, yaw, pitch, ALTURA_NAVE);
	posicion = AjustarAPista(posicion, ALTURA_NAVE, &adelanteNave, &arribaNave);
	rumbo = atan2f(adelanteNave.x, adelanteNave.z);
	return yaw;
}

void EstadoRival(float tiempo, LugarJugador& lugar, glm::vec3& pos, glm::vec3& adelante, glm::vec3& arriba)
{
	// vuelta completa: meta -> rocoso -> pista -> selvático -> pista -> océano -> pista -> meta
	float meta = DistanciaMeta();
	float largos[7] = {
		LargoTramo(0) - meta,                            // 0 pista: meta -> entrada del rocoso
		LargoCarreraSuperficie(SUPERFICIE_ROCOSA),       // 1 camino del planeta rocoso hasta su portal
		LargoTramo(1),                                   // 2 pista: rocoso -> selvático
		LargoCarreraSuperficie(SUPERFICIE_SELVATICA),    // 3 camino del planeta selvático hasta su portal
		LargoTramo(2),                                   // 4 pista: selvático -> océano
		LargoCarreraOceano(),                            // 5 sobre el mar hasta la cresta de la ola
		meta };                                          // 6 pista: océano -> meta
	float total = 0.0f;
	for (float l : largos) total += l;
	float d = fmodf(VELOCIDAD_RIVAL * TiempoDeCarrera(tiempo), fmaxf(total, 1.0f));
	int etapa = 0;
	while (etapa < 6 && d >= largos[etapa]) { d -= largos[etapa]; etapa++; }
	arriba = glm::vec3(0.0f, 1.0f, 0.0f);
	switch (etapa)
	{
	case 0: lugar = LUGAR_ESPACIO; PuntoEnPista(0, meta + d, CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba); break;
	case 1: lugar = LUGAR_ROCOSO; PuntoCarreraSuperficie(SUPERFICIE_ROCOSA, d, CARRIL_RIVAL_CAMINO, ALTURA_NAVE_CAMINO, pos, adelante); break;
	case 2: lugar = LUGAR_ESPACIO; PuntoEnPista(1, d, CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba); break;
	case 3: lugar = LUGAR_SELVATICO; PuntoCarreraSuperficie(SUPERFICIE_SELVATICA, d, CARRIL_RIVAL_CAMINO, ALTURA_NAVE_CAMINO, pos, adelante); break;
	case 4: lugar = LUGAR_ESPACIO; PuntoEnPista(2, d, CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba); break;
	case 5: lugar = LUGAR_OCEANO; PuntoCarreraOceano(d, CARRIL_RIVAL_AGUA, ALTURA_NAVE_AGUA, pos, adelante, arriba); break;
	default: lugar = LUGAR_ESPACIO; PuntoEnPista(0, d, CARRIL_RIVAL_PISTA, 1.3f, pos, adelante, arriba); break;
	}
}

bool BajarOSubirNave(LugarJugador lugar)
{
	if (lugar != LUGAR_ROCOSO && lugar != LUGAR_SELVATICO)
		return false;   // en el espacio se usa la base; en el océano no se puede bajar al agua
	TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
	if (avatar == AV_NAVE)
	{
		// se baja: la nave queda estacionada donde está y Paul aparece a un lado
		naveEstacionada = true;
		lugarEstacionada = lugar;
		posEstacionada = posicion;
		adelanteEstacionada = adelanteNave;
		glm::vec3 derecha = glm::normalize(glm::cross(adelanteNave, glm::vec3(0.0f, 1.0f, 0.0f)));
		avatar = AV_PAUL;
		posicion = LimitarPosicion(tipo, glm::vec3(posicion.x, 0.0f, posicion.z) + derecha * 4.5f);
		posicion.y = 0.0f;
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		printf("Bajaste de miNave: K junto a ella para volver a subir\n");
		return true;
	}
	if (avatar == AV_PAUL && naveEstacionada && lugarEstacionada == lugar)
	{
		if (glm::length(glm::vec2(posEstacionada.x - posicion.x, posEstacionada.z - posicion.z)) > DISTANCIA_PARA_ABORDAR)
		{
			printf("Acercate a tu nave para subir\n");
			return false;
		}
		// se sube y sigue la carrera desde donde dejó la nave
		avatar = AV_NAVE;
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

bool JugadorEnNave()
{
	return avatar == AV_NAVE;
}

glm::vec3 PosicionJugador()
{
	return posicion;
}

void ActualizarJugador(bool* teclas, float dt, LugarJugador lugar, Camera& camara, float tiempo)
{
	glm::vec3 frente = camara.getCameraDirection();
	glm::vec3 adelanteH(frente.x, 0.0f, frente.z);
	adelanteH = (glm::length(adelanteH) < 1.0e-3f) ? glm::vec3(sinf(rumbo), 0.0f, cosf(rumbo)) : glm::normalize(adelanteH);
	glm::vec3 derechaH = glm::normalize(glm::cross(adelanteH, glm::vec3(0.0f, 1.0f, 0.0f)));
	float ejeAdelante = (teclas[GLFW_KEY_W] ? 1.0f : 0.0f) - (teclas[GLFW_KEY_S] ? 1.0f : 0.0f);
	float ejeLado = (teclas[GLFW_KEY_D] ? 1.0f : 0.0f) - (teclas[GLFW_KEY_A] ? 1.0f : 0.0f);
	float distancia, alturaMira;

	if (lugar == LUGAR_ESPACIO && avatar == AV_NAVE)
	{
		// nave sobre la pista: W acelera hacia donde va la carrera, S frena y va en reversa, A/D cambian de carril
		glm::vec3 adelante, arriba;
		AjustarAPista(posicion, ALTURA_NAVE, &adelante, &arriba);
		glm::vec3 derecha = glm::normalize(glm::cross(adelante, arriba));
		float objetivo = (ejeAdelante > 0.0f) ? VELOCIDAD_NAVE_MAX : ((ejeAdelante < 0.0f) ? -12.0f : 0.0f);
		velocidadNave += (objetivo - velocidadNave) * fminf(1.0f, 1.8f * dt);
		posicion += adelante * (velocidadNave * dt) + derecha * (ejeLado * 10.0f * dt);
		posicion = AjustarAPista(posicion, ALTURA_NAVE, &adelanteNave, &arribaNave);
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		distancia = 15.0f;
		alturaMira = 2.5f;
	}
	else if ((lugar == LUGAR_ROCOSO || lugar == LUGAR_SELVATICO) && avatar == AV_NAVE)
	{
		// nave sobre el camino del planeta: igual que en la pista (W, S y A/D para moverse de lado)
		TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
		glm::vec3 adelante;
		AjustarACamino(tipo, posicion, ALTURA_NAVE_CAMINO, &adelante);
		glm::vec3 derecha = glm::normalize(glm::cross(adelante, glm::vec3(0.0f, 1.0f, 0.0f)));
		float objetivo = (ejeAdelante > 0.0f) ? VELOCIDAD_NAVE_MAX : ((ejeAdelante < 0.0f) ? -12.0f : 0.0f);
		velocidadNave += (objetivo - velocidadNave) * fminf(1.0f, 1.8f * dt);
		posicion += adelante * (velocidadNave * dt) + derecha * (ejeLado * 8.0f * dt);
		posicion = AjustarACamino(tipo, posicion, ALTURA_NAVE_CAMINO, &adelanteNave);
		arribaNave = glm::vec3(0.0f, 1.0f, 0.0f);
		rumbo = atan2f(adelanteNave.x, adelanteNave.z);
		distancia = 13.0f;
		alturaMira = 2.5f;
	}
	else if (lugar == LUGAR_OCEANO)
	{
		if (oceano.y <= 0.0f)
		{
			// mar plano: la nave va hacia donde mira la cámara (W) o de lado (A/D)
			glm::vec3 dir = adelanteH * ejeAdelante + derechaH * ejeLado;
			glm::vec2 objetivo(0.0f);
			if (glm::length(dir) > 0.0f)
			{
				dir = glm::normalize(dir);
				objetivo = glm::vec2(dir.x, dir.z) * VELOCIDAD_NAVE_MAX;   // (x, s): en el mar plano s = z
			}
			velocidadOceano += (objetivo - velocidadOceano) * fminf(1.0f, 1.6f * dt);
			if (glm::length(velocidadOceano) > 1.0f)
			{
				float nuevo = atan2f(velocidadOceano.x, velocidadOceano.y);
				rumbo += atan2f(sinf(nuevo - rumbo), cosf(nuevo - rumbo)) * fminf(1.0f, 6.0f * dt);
			}
		}
		else
		{
			// sobre la ola: W sube, S baja, A/D de lado; si no acelera, resbala hacia abajo
			float objetivoS = (ejeAdelante > 0.0f) ? VELOCIDAD_NAVE_MAX : ((ejeAdelante < 0.0f) ? -0.5f * VELOCIDAD_NAVE_MAX : -10.0f);
			velocidadOceano.y += (objetivoS - velocidadOceano.y) * fminf(1.0f, 1.6f * dt);
			velocidadOceano.x += (ejeLado * 10.0f - velocidadOceano.x) * fminf(1.0f, 3.0f * dt);
			rumbo += atan2f(sinf(0.0f - rumbo), cosf(0.0f - rumbo)) * fminf(1.0f, 4.0f * dt);   // de frente a la ola
		}
		oceano += velocidadOceano * dt;
		oceano = LimitarOceano(oceano.x, oceano.y);
		// posición y orientación sobre el agua: en la pared de la ola la nave queda mirando hacia arriba
		glm::vec3 tangente, normal;
		PuntoOla(oceano.x, oceano.y, ALTURA_NAVE_AGUA, posicion, tangente, normal);
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
			posicion += mov * (VELOCIDAD_PIE * dt);
			float objetivo = atan2f(mov.x, mov.z);
			float diferencia = atan2f(sinf(objetivo - rumbo), cosf(objetivo - rumbo));
			rumbo += diferencia * fminf(1.0f, 10.0f * dt);
			pasos += dt * 9.0f;
		}
		if (lugar == LUGAR_ESPACIO)
		{
			// en la base: no se sale de la plataforma
			glm::vec3 centro, adelante, derecha;
			float radio;
			DatosBase(centro, adelante, derecha, radio);
			glm::vec2 d(posicion.x - centro.x, posicion.z - centro.z);
			float maximo = radio - 1.5f;
			if (glm::length(d) > maximo) d = glm::normalize(d) * maximo;
			posicion = glm::vec3(centro.x + d.x, centro.y, centro.z + d.y);
		}
		else
		{
			// en el planeta: zona del camino y obstáculos (troncos, rocas, cristales...)
			TipoSuperficie tipo = (lugar == LUGAR_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
			posicion = LimitarPosicion(tipo, posicion);
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

	// cámara en tercera persona: detrás del avatar, orbitando con el mouse, mirando hacia él
	glm::vec3 arribaMira = (lugar == LUGAR_OCEANO) ? arribaNave : glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 objetivo = posicion + arribaMira * alturaMira;
	glm::vec3 posCamara = objetivo - frente * distancia;
	float piso = -1.0e9f;
	if (lugar == LUGAR_ESPACIO && avatar == AV_PAUL) piso = posicion.y + 0.8f;            // piso de la base
	else if (lugar != LUGAR_ESPACIO) piso = 0.8f;
	if (posCamara.y < piso) posCamara.y = piso;
	if (lugar == LUGAR_OCEANO)
		posCamara = AjustarCamaraOceano(posCamara);   // no se mete al agua ni a la ola
	camara.setPosition(posCamara);
}

void DibujarJugador(LugarJugador lugar, float tiempo, const UniformsJugador& u)
{
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);

	// miNave: estacionada en la base (flotando suave) o como avatar en la pista, los caminos o el mar
	bool estacionada = (lugar == LUGAR_ESPACIO && !abordo);
	if (hayNave && (estacionada || avatar == AV_NAVE))
	{
		glm::vec3 p = posicion, adelante = adelanteNave, arriba = arribaNave;
		if (estacionada && avatar != AV_NAVE)
		{
			lugarNaveEnBase(p, adelante);
			arriba = glm::vec3(0.0f, 1.0f, 0.0f);
			p.y += 0.6f + 0.25f * sinf(1.5f * tiempo);   // flotando: y = 0.6 + 0.25 sen(1.5 t)
		}
		{
			float largo = (lugar == LUGAR_ROCOSO || lugar == LUGAR_SELVATICO) ? LARGO_NAVE_PLANETA : LARGO_NAVE;
			float escala = largo / fmaxf(nave.getTamano().z, 0.001f);
			glm::mat4 m = orientar(p, adelante, arriba) * glm::scale(glm::mat4(1.0f), glm::vec3(escala)) * base(nave);
			matNave.UseMaterial(u.especular, u.brillo);
			glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
			nave.RenderModel();
		}
	}

	// nave estacionada en el planeta mientras el jugador camina (flota: y += 0.2 sen(1.5 t))
	if (naveEstacionada && lugar == lugarEstacionada && hayNave)
	{
		float escala = LARGO_NAVE_PLANETA / fmaxf(nave.getTamano().z, 0.001f);
		glm::vec3 p = posEstacionada + glm::vec3(0.0f, 0.2f * sinf(1.5f * tiempo), 0.0f);
		glm::mat4 m = orientar(p, adelanteEstacionada, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(escala)) * base(nave);
		matNave.UseMaterial(u.especular, u.brillo);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
		nave.RenderModel();
	}

	// Paul: el avatar a pie (con un pequeño rebote al caminar: y = 0.12 |sen(pasos)|)
	if (avatar == AV_PAUL && paul != nullptr)
	{
		float escala = ALTURA_PAUL / fmaxf(paul->getTamano().y, 0.001f);
		glm::mat4 m = glm::translate(glm::mat4(1.0f), posicion + glm::vec3(0.0f, 0.12f * fabsf(sinf(pasos)), 0.0f))
			* glm::rotate(glm::mat4(1.0f), rumbo, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), 0.05f * sinf(pasos), glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(escala))
			* base(*paul);
		matAvatar.UseMaterial(u.especular, u.brillo);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
		paul->RenderModel();
	}
}
