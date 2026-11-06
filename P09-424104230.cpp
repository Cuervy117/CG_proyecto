/*
Proyecto final de Computación Gráfica e Interacción Humano-Computadora (2027-1)
Temáticas: "Viajes espaciales" y "Evolución de especies intergalácticas"

Este main sigue la misma estructura de las prácticas (P5, P8, P9, P09-1 y P09-2):
 1) Variables globales: ventana, cámara, texturas, modelos, skybox, materiales y luces.
 2) CreateObjects() (en Create.cpp) y CreateShaders().
 3) main(): se cargan las texturas, los modelos y el skybox (en Load.cpp), se declaran las luces y
    dentro del while se reciben las teclas, se actualizan las animaciones y se dibuja todo con
    model = glm::translate(model, ...), UseTexture(), UseMaterial() y RenderModel().
Las teclas se asignan en Window.cpp (ManejaTeclado), igual que en las prácticas.
Todo lo que NO viene en las prácticas está marcado con "NUEVO" y explicado en su comentario.

Teclas:
 W A S D + mouse	mover el avatar (o la cámara libre)
 C					cámara del avatar <-> cámara libre
 E					entrar al planeta cercano / salir al espacio
 J					subir a miNave en la base (empieza la carrera) / bajar o subir en los planetas
 2					abrir / cerrar la puerta de la cápsula (animación básica)
 3					abrir / cerrar los paneles del satélite (animación básica)
 F, L, K			linterna, faro de miNave y foco de la base (spotlights)
 F9				liberar / capturar el cursor
 ESC				salir
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include "Input.hpp"
#include "CameraDirector.h"
#include "Orbit.h"
#include "Chase.h"
#include "Scene.h"
#include "src/game/avatar/Avatar.h"
#include "src/ui/Hud.h"
bool VerificarEscenaIntegrada(const SceneNode&, SceneNode&);
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <cmath>
#include <fstream>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

//creación de mallas (Create.cpp) y carga de texturas, modelos y skybox (Load.cpp)
#include "Create.hpp"
#include "Load.hpp"

// NUEVO: módulos del escenario (cada .h explica qué hace y por qué existe)
#include "Superficie.h"		// superficie de los planetas rocoso y selvático
#include "Espacio.h"		// pista de carreras, cinturón de asteroides y base de inicio
#include "Oceano.h"			// planeta océano (por fuera y por dentro, con la ola gigante)
#include "Jugador.h"		// avatar en tercera persona, miNave y la nave rival
#include "Reloj.h"			// reloj en pantalla
const float toRadians = 3.14159265f / 180.0f;

//variables para animación
//textura con movimiento (práctica 9): desplazamiento de la textura del agua del océano
float toffsetaguau = 0.0f;
float toffsetaguav = 0.0f;

Window mainWindow;
std::vector<MeshModel*> meshListModel; // recibe xyz uv nx ny nz (se llena en CreateObjects)
std::vector<Shader> shaderList;

Camera camera;

Texture plainTexture;
Texture llamaTexture;

Model Nave_M;
Model Planeta_M;
Model PlanetaRocoso_M;
Model PlanetaSelvatico_M;
Model CapsulaCuerpo_M;
Model CapsulaPuerta_M;
Model SateliteBase_M;
Model SatelitePanelDer_M;
Model SatelitePanelIzq_M;
Model SateliteAspa_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;

// NUEVO: un objeto por cada módulo del escenario (igual que se declara Skybox skybox)
Superficie superficie;
Espacio espacio;
Oceano oceano;
Jugador jugador;
Reloj reloj;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
// NUEVO: tiempo total de la animación en segundos (lo usan el ciclo de día y noche y las animaciones continuas)
GLfloat tiempoAnimacion = 0.0f;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";

// Estas texturas forman parte del paquete minimo del proyecto. Si falta una, detener el
// arranque evita que el cargador de modelos la oculte usando Textures/plain.png.
static bool VerificarTexturasCriticas()
{
	const char* rutas[] = {
		"Textures/planetaOceanoTexture.png",
		"Textures/planetaRocaTexture.png",
		"Textures/planetaSelvaTexture.png",
		"Textures/pistaTexture.png",
		"Textures/baseTexture.png",
		"Textures/hud_reloj.png",
		"Textures/hud_digitos.png",
		"Textures/hud_iconos.png"
	};
	bool completas = true;
	for (const char* ruta : rutas)
	{
		std::ifstream archivo(ruta, std::ios::binary);
		if (!archivo.good())
		{
			fprintf(stderr, "[ERROR] Falta textura critica: %s\n", ruta);
			completas = false;
		}
	}
	if (!completas)
		fprintf(stderr, "Ejecuta scripts/Verify-Project-Assets.ps1 desde la raiz del proyecto.\n");
	return completas;
}

// NUEVO: lugar donde está el jugador. Solo se dibuja el lugar activo: el espacio o la superficie de
// un planeta (al entrar a un planeta se cambia de "mapa", no se camina sobre la esfera).
enum ZonaActiva
{
	ZONA_ESPACIO = 0,
	ZONA_PLANETA_ROCOSO,
	ZONA_PLANETA_SELVATICO,
	ZONA_PLANETA_OCEANO
};
ZonaActiva zonaActual = ZONA_ESPACIO;

// cámara del avatar (tercera persona) o cámara libre; se cambia con la tecla C (Window.cpp)
bool camaraLibre = false;

// planetas en el espacio (posición, radio del modelo y radio de su atmósfera)
glm::vec3 posPlanetaRocoso = glm::vec3(0.0f, 0.0f, 0.0f);
float radioPlanetaRocoso = 14.0f;
float atmosferaRocoso = 22.0f;
glm::vec3 posPlanetaSelvatico = glm::vec3(120.0f, 10.0f, 90.0f);
float radioPlanetaSelvatico = 16.0f;
float atmosferaSelvatico = 24.0f;
float alturaCamaraSuperficie = 3.5f;

// Animación básica 1: puerta de la cápsula (tecla 2). La puerta gira sobre su bisagra, que es su
// borde inferior y no es paralela al eje X (está girada ~21 grados alrededor de Y): por eso gira
// sobre su propio eje. Los puntos se ajustan con las medidas del modelo (CalcularPuntosCapsula).
float anguloPuertaCapsula = 0.0f;
glm::vec3 pivoteBisagraPuerta = glm::vec3(0.0932f, 0.4005f, 0.2868f);
glm::vec3 ejeBisagraPuerta = glm::vec3(0.9321f, 0.0f, -0.3623f);
glm::vec3 origenLocalCapsula = glm::vec3(0.0f, 0.1037f, 0.0f);
glm::vec3 bisagraDesdeMinimoPuerta = glm::vec3(0.1227f, 0.0f, 0.0460f);   // la bisagra medida desde la esquina mínima de la puerta

// Animación básica 2: paneles del satélite (tecla 3). Animación compleja: el satélite recorre una
// órbita en forma de ocho (dos lóbulos) y su aspa gira sin parar.
float factorAperturaPaneles = 1.0f;
const float duracionCicloDiaNoche = 120.0f;	// segundos que dura un día completo (máximo 2 minutos)
const float dosPi = 6.28318530718f;
const float guiaPaso = 3.0f;
const float guiaSateliteAncho = 0.28f;
const float guiaSateliteLargo = 1.5f;
const float satRadioX = 16.0f;
const float satRadioZ = 12.0f;
const float satAltura = 26.0f;
const float satOndulacion = 2.0f;
const float satOmega = 0.35f;

struct PuntoGuia
{
	glm::vec3 pos;
	glm::vec3 forward;
};
std::vector<PuntoGuia> guiaSatelite;

int estadoSatelite = 0;
float thetaSatelite = 0.0f;
float rotPanelDer = 0.0f;
float rotPanelIzq = 0.0f;
float rotacionRotor = 0.0f;

// dirección de la nave rival (se suaviza para que no gire de golpe en las curvas)
glm::vec3 naveForward = glm::vec3(1.0f, 0.0f, 0.0f);

// luminarias de los planetas (posiciones de sus luces puntuales)
glm::vec3 posLuminaria[2];


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

// NUEVO: base de la cápsula (centro en x, z y altura mínima) y bisagra de la puerta, calculadas con las
// medidas reales del modelo: así funcionan aunque la cápsula se vuelva a mover en Blender.
void CalcularPuntosCapsula()
{
	if (CapsulaCuerpo_M.estaCargado())
	{
		glm::vec3 c = CapsulaCuerpo_M.getCentro(), t = CapsulaCuerpo_M.getTamano();
		origenLocalCapsula = glm::vec3(c.x, c.y - 0.5f * t.y, c.z);
	}
	if (CapsulaPuerta_M.estaCargado())
	{
		glm::vec3 minimoPuerta = CapsulaPuerta_M.getCentro() - 0.5f * CapsulaPuerta_M.getTamano();
		pivoteBisagraPuerta = minimoPuerta + bisagraDesdeMinimoPuerta;
	}
}

// NUEVO: carga bajo demanda compartida por F5/F6 y por la entrada normal del jugador (E o atmósfera).
// Las funciones de Load.cpp y Superficie tienen guardas, así que llamarla varias veces es seguro.
void PrepararRecursosSuperficie(ZonaActiva destino)
{
	if (destino != ZONA_PLANETA_ROCOSO && destino != ZONA_PLANETA_SELVATICO)
		return;

	const auto inicio = std::chrono::steady_clock::now();
	const TipoSuperficie tipo = (destino == ZONA_PLANETA_SELVATICO)
		? SUPERFICIE_SELVATICA : SUPERFICIE_ROCOSA;

	LoadCapsulaModels();
	CalcularPuntosCapsula();
	if (tipo == SUPERFICIE_SELVATICA) LoadPlanetaSelvatico();
	else LoadPlanetaRocoso();
	superficie.PrepararModelosSuperficie(tipo);

	const bool planetaCargado = (tipo == SUPERFICIE_SELVATICA)
		? PlanetaSelvatico_M.estaCargado() : PlanetaRocoso_M.estaCargado();
	const double ms = std::chrono::duration<double, std::milli>(
		std::chrono::steady_clock::now() - inicio).count();
	printf("[QA] Recursos %s: capsula=%d puerta=%d planeta=%d total=%.1f ms\n",
		(tipo == SUPERFICIE_SELVATICA) ? "selvatica" : "rocosa",
		CapsulaCuerpo_M.estaCargado() ? 1 : 0,
		CapsulaPuerta_M.estaCargado() ? 1 : 0,
		planetaCargado ? 1 : 0,
		ms);
	fflush(stdout);
}

// NUEVO: matriz de modelo a partir de una posición y una dirección (como Orientar de la práctica 9-1).
// Se usa para que la nave rival mire hacia donde avanza en la pista.
glm::mat4 MatrizOrientacion(glm::vec3 pos, glm::vec3 forward, glm::vec3 upRef)
{
	glm::vec3 f = glm::normalize(forward);
	glm::vec3 r = glm::normalize(glm::cross(f, upRef));
	glm::vec3 u = glm::normalize(glm::cross(r, f));

	glm::mat4 m(1.0f);
	m[0] = glm::vec4(r, 0.0f);
	m[1] = glm::vec4(u, 0.0f);
	m[2] = glm::vec4(f, 0.0f);
	m[3] = glm::vec4(pos, 1.0f);
	return m;
}

// NUEVO (animación compleja): órbita del satélite en forma de ocho. Cada lóbulo es una elipse que
// pasa por el origen; al terminar una vuelta (theta = 2 pi) cambia al otro lado (estado 0 o 1):
//   x = lado * radioX * (1 - cos theta),  y = altura + ondulacion * sen(2 theta),  z = radioZ * sen theta
glm::vec3 PosicionSatelite(int estado, float theta)
{
	float lado = (estado == 0) ? -1.0f : 1.0f;
	return glm::vec3(
		lado * satRadioX * (1.0f - cosf(theta)),
		satAltura + satOndulacion * sinf(2.0f * theta),
		satRadioZ * sinf(theta));
}

// derivada de la posición: hacia dónde avanza el satélite
glm::vec3 TangenteSatelite(int estado, float theta)
{
	float lado = (estado == 0) ? -1.0f : 1.0f;
	return glm::normalize(glm::vec3(
		lado * satRadioX * sinf(theta),
		2.0f * satOndulacion * cosf(2.0f * theta),
		satRadioZ * cosf(theta)));
}

// puntos de la guía (marcas rosas) que muestran el recorrido del satélite, uno cada 'guiaPaso' unidades
void ConstruirGuiaSatelite()
{
	guiaSatelite.clear();
	for (int estado = 0; estado < 2; estado++)
	{
		float lado = (estado == 0) ? -1.0f : 1.0f;
		float theta = 0.0f;

		while (theta < dosPi)
		{
			glm::vec3 pos = PosicionSatelite(estado, theta);
			glm::vec3 tangente = TangenteSatelite(estado, theta);
			PuntoGuia punto;
			punto.pos = pos;
			punto.forward = glm::normalize(tangente);
			guiaSatelite.push_back(punto);

			// se avanza theta según la rapidez de la curva para que las marcas queden a la misma distancia
			float dx = lado * satRadioX * sinf(theta);
			float dy = 2.0f * satOndulacion * cosf(2.0f * theta);
			float dz = satRadioZ * cosf(theta);
			float rapidez = sqrtf(dx * dx + dy * dy + dz * dz);
			theta += guiaPaso / fmaxf(rapidez, 0.0001f);
		}
	}
}

void AnimarSatelite(GLfloat dt)
{
	// avanza por la órbita; al completar un lóbulo cambia al otro
	thetaSatelite += satOmega * dt;
	while (thetaSatelite >= dosPi)
	{
		thetaSatelite -= dosPi;
		estadoSatelite = 1 - estadoSatelite;
	}

	// tecla 3: los paneles se abren o se cierran poco a poco (el factor se acerca a 1 o a 0)
	float metaFactor = mainWindow.getPanelesSatelite() ? 1.0f : 0.0f;
	factorAperturaPaneles += (metaFactor - factorAperturaPaneles) * fminf(1.0f, 5.0f * dt);

	// abiertos se mecen con un seno; cerrados se pliegan a +-85 grados
	rotPanelDer = (25.0f * sinf(0.9f * tiempoAnimacion)) * factorAperturaPaneles + (1.0f - factorAperturaPaneles) * (-85.0f);
	rotPanelIzq = (-25.0f * sinf(0.9f * tiempoAnimacion)) * factorAperturaPaneles + (1.0f - factorAperturaPaneles) * (85.0f);
	rotacionRotor = fmodf(rotacionRotor + 180.0f * dt, 360.0f);
}

void AnimarCapsula(GLfloat dt)
{
	// tecla 2: la puerta se abre hasta 75 grados o se cierra (se acerca poco a poco al ángulo meta)
	float metaAngulo = mainWindow.getPuertaCapsula() ? 75.0f : 0.0f;
	anguloPuertaCapsula += (metaAngulo - anguloPuertaCapsula) * fminf(1.0f, 6.0f * dt);
}

// NUEVO: ciclo de día y noche (no viene en las prácticas).
// En la práctica la luz direccional (el sol) se declara una sola vez. Aquí se vuelve a crear en cada
// cuadro con una dirección y una intensidad que dependen del tiempo, así el sol "da la vuelta":
//   anguloSol = 2 pi * t / 120		una vuelta completa cada 120 s (el máximo que se pide: 2 minutos)
//   alturaSol = sen(anguloSol)		1 al mediodía, -1 a la medianoche
//   factorDia = (alturaSol + 0.25) / 1.25, limitado entre 0.08 y 1		qué tanto es de día
// La dirección de la luz sigue al sol y el color pasa de blanco cálido (día) a azul (noche).
// factorDia también decide cuándo se prenden las luminarias (factorDia < 0.45 = de noche).
float ActualizarCicloDiaNoche(ZonaActiva zonaVista)
{
	float anguloSol = (dosPi / duracionCicloDiaNoche) * tiempoAnimacion;
	float alturaSol = sinf(anguloSol);
	float factorDia = glm::clamp((alturaSol + 0.25f) / 1.25f, 0.08f, 1.0f);

	glm::vec3 dirLuz = glm::normalize(glm::vec3(cosf(anguloSol), -alturaSol, 0.35f));
	const bool vacio = zonaVista == ZONA_PLANETA_ROCOSO;
	const glm::vec3 noche = vacio ? glm::vec3(0.73f, 0.65f, 0.98f) : glm::vec3(1.0f, 0.35f, 0.65f);
	const glm::vec3 luz = glm::mix(noche, glm::vec3(1.0f, 0.96f, 0.90f), factorDia);
	mainLight = DirectionalLight(
		luz.r, luz.g, luz.b,
		glm::mix(vacio ? 0.32f : 0.12f, 0.40f, factorDia),
		0.15f + 0.75f * factorDia,
		dirLuz.x, dirLuz.y, dirLuz.z);

	return factorDia;
}

// Velocidad de movimiento y de giro de la cámara según el modo y el lugar
float VelocidadCamara()
{
	if (camaraLibre)
		return (zonaActual == ZONA_ESPACIO) ? 45.0f : 30.0f;
	return 14.0f;
}

float GiroCamara()
{
	return camaraLibre ? 0.50f : 0.35f;
}

LugarJugador LugarDelJugador()
{
	if (zonaActual == ZONA_PLANETA_ROCOSO) return LUGAR_ROCOSO;
	if (zonaActual == ZONA_PLANETA_SELVATICO) return LUGAR_SELVATICO;
	if (zonaActual == ZONA_PLANETA_OCEANO) return LUGAR_OCEANO;
	return LUGAR_ESPACIO;
}

// NUEVO: la clase Camera no tiene funciones para cambiar su velocidad ni su orientación, así que se
// vuelve a crear con camera = Camera(...) (igual que al inicio del main de la práctica).
void ReiniciarCamara(glm::vec3 posicion, float yaw, float pitch)
{
	camera = Camera(posicion, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch, VelocidadCamara(), GiroCamara());
}

// Tecla C: cambia entre la cámara del avatar y la cámara libre sin moverla de lugar
void CambiarModoCamara()
{
	camaraLibre = mainWindow.getCamaraLibre();
	glm::vec3 dir = camera.getCameraDirection();
	float yaw = glm::degrees(atan2f(dir.z, dir.x));
	float pitch = glm::degrees(asinf(glm::clamp(dir.y, -1.0f, 1.0f)));
	ReiniciarCamara(camera.getCameraPosition(), yaw, pitch);
	printf("Camara %s\n", camaraLibre ? "LIBRE (vuela por todo el recorrido)" : "DEL AVATAR");
}

// Nave rival dentro de un planeta o del océano (en el espacio se dibuja con sus llamas y luces)
void DibujarRivalEn(RivalCarrera rival, LugarJugador lugar, GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	glm::vec3 pos, adelante, arriba;
	LugarJugador lugarRival;
	jugador.EstadoRival(rival, tiempoAnimacion, lugarRival, pos, adelante, arriba);
	if (lugarRival != lugar)
		return;
	glm::mat4 model(1.0);
	model = MatrizOrientacion(pos, adelante, arriba);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	const glm::vec3 tinte = rival == RivalCarrera::Segundo ? glm::vec3(1.0f, 0.48f, 0.15f) : glm::vec3(1);
	glUniform3fv(uniformColor, 1, glm::value_ptr(tinte));
	plainTexture.UseTexture();
	Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	Nave_M.RenderModel();
	glUniform3f(uniformColor, 1, 1, 1);
}

// NUEVO: salir de la atmósfera del planeta actual. El jugador aparece sobre la pista de carreras, en
// el punto de salida de ese planeta (como en el croquis), mirando hacia donde avanza la carrera.
void SalirAlEspacio()
{
	if (zonaActual == ZONA_ESPACIO)
		return;
	int planeta = PLANETA_ROCOSO;
	if (zonaActual == ZONA_PLANETA_SELVATICO) planeta = PLANETA_SELVATICO;
	else if (zonaActual == ZONA_PLANETA_OCEANO) planeta = PLANETA_OCEANO;
	zonaActual = ZONA_ESPACIO;
	float yaw = jugador.ColocarJugadorAlSalir(planeta);   // en su nave sobre la pista (o en la base si aún no la aborda)
	ReiniciarCamara(jugador.PosicionJugador() + glm::vec3(0.0f, 4.0f, 0.0f), yaw, camaraLibre ? 0.0f : -10.0f);
}

// NUEVO: cambios de lugar y acciones de las teclas E, K y C.
//  - En el espacio: al cruzar la atmósfera de un planeta (o con E cerca de ella) se entra a su superficie.
//  - En un planeta: E o cruzar el portal en la nave regresan al espacio.
//  - En el océano: E o llegar a la cresta de la ola regresan al espacio.
//  - K: "entrar a la nave" en la base, o bajar / subir de ella en los planetas.
void ActualizarZona()
{
	// con la cámara del avatar, lo que entra a los planetas o cruza el portal es el avatar
	glm::vec3 camPos = camaraLibre ? camera.getCameraPosition() : jugador.PosicionJugador();
	bool pulsoE = mainWindow.getEntrarSalir();
	bool pulsoK = mainWindow.getAbordarNave();

	// tecla C (la bandera está en Window): si cambió, se vuelve a crear la cámara
	if (mainWindow.getCamaraLibre() != camaraLibre)
		CambiarModoCamara();

	// K junto a miNave (en la base): la nave pasa a ser el avatar, aparece en la meta y empieza la carrera
	if ((pulsoK || pulsoE) && zonaActual == ZONA_ESPACIO && !camaraLibre)
	{
		float yaw;
		if (jugador.IntentarAbordarNave(tiempoAnimacion, yaw)) {
			pulsoE = false;
			ReiniciarCamara(jugador.PosicionJugador() + glm::vec3(0.0f, 4.0f, 0.0f), yaw, -10.0f);
		}
	}
	// K en el planeta rocoso o selvático: bajar de la nave (queda estacionada) o volver a subir
	else if (pulsoK && !camaraLibre && (zonaActual == ZONA_PLANETA_ROCOSO || zonaActual == ZONA_PLANETA_SELVATICO))
	{
		jugador.BajarOSubirNave(LugarDelJugador());
	}

	if (zonaActual == ZONA_ESPACIO)
	{
		float distRocoso = glm::distance(camPos, posPlanetaRocoso);
		float distSelvatico = glm::distance(camPos, posPlanetaSelvatico);
		float distOceano = glm::distance(camPos, oceano.CentroPlanetaOceano());

		if (distRocoso < atmosferaRocoso || (pulsoE && distRocoso < atmosferaRocoso * 1.6f))
		{
			PrepararRecursosSuperficie(ZONA_PLANETA_ROCOSO);
			zonaActual = ZONA_PLANETA_ROCOSO;
			glm::vec3 inicio = superficie.PosicionInicio(SUPERFICIE_ROCOSA);
			float yaw = jugador.ColocarJugadorEnPlaneta(LUGAR_ROCOSO, inicio, superficie.YawInicio(SUPERFICIE_ROCOSA));   // sigue en su nave
			ReiniciarCamara(glm::vec3(inicio.x, alturaCamaraSuperficie, inicio.z), yaw, camaraLibre ? 0.0f : -12.0f);
		}
		else if (distSelvatico < atmosferaSelvatico || (pulsoE && distSelvatico < atmosferaSelvatico * 1.6f))
		{
			PrepararRecursosSuperficie(ZONA_PLANETA_SELVATICO);
			zonaActual = ZONA_PLANETA_SELVATICO;
			glm::vec3 inicio = superficie.PosicionInicio(SUPERFICIE_SELVATICA);
			float yaw = jugador.ColocarJugadorEnPlaneta(LUGAR_SELVATICO, inicio, superficie.YawInicio(SUPERFICIE_SELVATICA));   // sigue en su nave
			ReiniciarCamara(glm::vec3(inicio.x, alturaCamaraSuperficie, inicio.z), yaw, camaraLibre ? 0.0f : -12.0f);
		}
		else if (distOceano < oceano.RadioAtmosferaOceano() || (pulsoE && distOceano < oceano.RadioAtmosferaOceano() * 1.6f))
		{
			zonaActual = ZONA_PLANETA_OCEANO;
			float yaw = jugador.ColocarJugadorEnOceano();   // de espaldas a la ola, viendo el mar abierto
			ReiniciarCamara(jugador.PosicionJugador() + glm::vec3(0.0f, 5.0f, 12.0f), yaw, camaraLibre ? 0.0f : -6.0f);
		}
	}
	else if (zonaActual == ZONA_PLANETA_OCEANO)
	{
		// en el océano: al llegar a la cresta de la ola gigante (o con E) se sale a la pista
		if (pulsoE || oceano.LlegoALaCresta(camPos))
			SalirAlEspacio();
	}
	else
	{
		// en un planeta: E o cruzar el portal del final del camino regresan al espacio
		TipoSuperficie tipo = (zonaActual == ZONA_PLANETA_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
		if (pulsoE || (jugador.JugadorEnNave() && superficie.CruzandoPortal(tipo, camPos)))   // el portal se cruza en la nave
			SalirAlEspacio();
	}
}

// guía (marcas rosas) del recorrido del satélite: cuadros con transparencia aditiva
void DibujarGuiaSatelite(glm::vec3 color, GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	plainTexture.UseTexture();
	Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	glUniform3fv(uniformColor, 1, glm::value_ptr(color));

	glm::vec3 mundoArriba(0.0f, 1.0f, 0.0f);
	glm::mat4 model(1.0);
	for (size_t i = 0; i < guiaSatelite.size(); i++)
	{
		// cada marca se acuesta en la dirección del recorrido
		glm::vec3 forward = guiaSatelite[i].forward;
		glm::vec3 right = glm::normalize(glm::cross(forward, mundoArriba));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		model = glm::mat4(1.0f);
		model[0] = glm::vec4(right, 0.0f);
		model[1] = glm::vec4(forward, 0.0f);
		model[2] = glm::vec4(up, 0.0f);
		model[3] = glm::vec4(guiaSatelite[i].pos + up * 0.05f, 1.0f);
		model = glm::scale(model, glm::vec3(guiaSateliteAncho, guiaSateliteLargo, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshListModel[0]->RenderMeshModel();
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}


// Las paradas comparten las coordenadas del dibujo. La zona observada es
// independiente de la del jugador, que conserva su posición durante R4.
enum ParadaR4 { Lanzamiento, Hangar, Vacio, Shinobi, Satelite, CoheteBase, TotalParadas };
struct EncuadreR4 { const char* nombre; ZonaActiva zona; float distancia, minima, pitch, altura; };
const EncuadreR4 encuadres[TotalParadas] = {
	{"D1 SHINOBI", ZONA_PLANETA_SELVATICO, 45, 18, 1.15f, 7.5f},
	{"HANGAR", ZONA_ESPACIO, 65, 34, 0.24f, 3},
	{"VACIO", ZONA_ESPACIO, 55, 22, 0.35f, 0},
	{"SHINOBI", ZONA_ESPACIO, 60, 24, 0.35f, 0},
	{"SATELITE", ZONA_ESPACIO, 24, 10, 0.35f, 0},
	{"COHETE BASE", ZONA_ESPACIO, 55, 18, 0.12f, 8}
};
const glm::vec3 centroHangar(-55, 25, -25);

bool ParadaDisponible(ParadaR4 parada)
{
	switch (parada) {
	case Lanzamiento: case CoheteBase: return espacio.CoheteDisponible();
	case Vacio: return PlanetaRocoso_M.estaCargado() || Planeta_M.estaCargado();
	case Shinobi: return PlanetaSelvatico_M.estaCargado() || Planeta_M.estaCargado();
	case Satelite: return SateliteBase_M.estaCargado();
	default: return true; // El hangar es procedural y siempre está presente.
	}
}

ParadaR4 SiguienteParada(ParadaR4 actual)
{
	do { actual = static_cast<ParadaR4>((actual + 1) % TotalParadas); }
	while (!ParadaDisponible(actual));
	return actual;
}

glm::vec3 CentroObservado(ParadaR4 parada)
{
	switch (parada) {
	case Lanzamiento: return espacio.CentroCohete(COHETE_SHINOBI);
	case CoheteBase: return espacio.CentroCohete(COHETE_BASE);
	case Vacio: return posPlanetaRocoso;
	case Shinobi: return posPlanetaSelvatico;
	case Satelite: return PosicionSatelite(estadoSatelite, thetaSatelite);
	default: return centroHangar;
	}
}

void DibujarHangar(Sphere& esfera, GLuint uModelo, GLuint uColor, GLuint uEmision,
	GLuint uEspecular, GLuint uBrillo)
{
	Material_brillante.UseMaterial(uEspecular, uBrillo);
	plainTexture.UseTexture();
	auto pieza = [&](bool redonda, glm::vec3 local, glm::vec3 tam, glm::vec3 color, float luz = 0.20f) {
		glm::mat4 m = glm::translate(glm::mat4(1), centroHangar + local);
		m = glm::scale(m, tam);
		glUniformMatrix4fv(uModelo, 1, GL_FALSE, glm::value_ptr(m));
		glUniform3fv(uColor, 1, glm::value_ptr(color));
		glUniform3f(uEmision, luz * color.r, luz * color.g, luz * color.b);
		if (redonda) esfera.render(); else meshListModel[2]->RenderMeshModel();
	};
	const glm::vec3 casco(0.65f, 0.71f, 0.82f), baliza(0.1f, 0.85f, 1);
	pieza(true, {0,-3,0}, {22,2,17}, casco);     // disco de atraque
	pieza(true, {0,8,0}, {19,3,14}, casco);      // cúpula elevada sobre la boca (+Z)
	pieza(false, {0,2,-9}, {32,9,2}, casco);
	for (float lado : {-1.0f, 1.0f}) {
		pieza(false, {lado*15,2,0}, {3,9,18}, casco);
		pieza(false, {lado*13.4f,2,8}, {0.5f,7,1}, baliza, 0.9f);
	}
	// Dos cápsulas en carriles separados cruzan la boca en sentidos opuestos.
	for (int i = 0; i < 2; ++i) {
		float fase = 0.42f * tiempoAnimacion + i * 3.14159265f;
		glm::vec3 nave(i ? 5.0f : -5.0f, 1.2f, 12.0f + 17.0f * cosf(fase));
		pieza(true, nave, {1.8f,1.3f,3.3f}, i ? glm::vec3(0.9f,0.48f,0.18f) : casco);
		pieza(true, nave + glm::vec3(0,0.9f,0), {1.2f,0.7f,1.7f}, baliza, 0.45f);
	}
	glUniform3f(uEmision, 0,0,0);
	glUniform3f(uColor, 1,1,1);
}

int main(int argc, char** argv)
{
	const bool pruebaRelease = argc == 2 && strcmp(argv[1], "--probar-release") == 0;
	struct PasoRelease { int tecla; ZonaActiva zonaVista; };
	const PasoRelease pasosRelease[] = {{0, ZONA_ESPACIO}, {GLFW_KEY_F6, ZONA_PLANETA_ROCOSO},
		{GLFW_KEY_F5, ZONA_PLANETA_SELVATICO}, {GLFW_KEY_F7, ZONA_PLANETA_OCEANO}, {GLFW_KEY_F8, ZONA_ESPACIO},
		{GLFW_KEY_F2, ZONA_ESPACIO}, {GLFW_KEY_F4, ZONA_PLANETA_SELVATICO},
		{GLFW_KEY_1, ZONA_PLANETA_SELVATICO}, {GLFW_KEY_TAB, ZONA_ESPACIO}};
	const size_t totalPasos = sizeof(pasosRelease) / sizeof(pasosRelease[0]);
	size_t pasoPrueba = 0;
	if (!VerificarTexturasCriticas()) return 1;
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	if (mainWindow.Initialise()) return 1;
	cgame::InputCapture captura(mainWindow);

	CreateObjects();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 30.0f, -85.0f), glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, -8.0f, 20.0f, 0.25f);

	// Texturas, modelos y cubemaps del escenario.
	LoadTextures();
	LoadModels();
	if (!LoadSkybox()) return 1;

	// NUEVO: módulos del escenario (cada uno carga sus propios modelos y texturas)
	if (!superficie.InicializarSuperficies()) return 1;
	if (!espacio.InicializarEspacio(&oceano)) return 1;	// pista de carreras, cinturón de asteroides y base
	if (!reloj.InicializarReloj()) return 1;				// reloj en pantalla
	oceano.InicializarOceano();				// planeta océano (por fuera y por dentro)

	// el avatar empieza en la base, junto a la pista y antes de la meta, con miNave al lado
	jugador.InicializarJugador(&superficie, &espacio, &oceano);
	float yawInicio = jugador.ColocarJugadorEnBase();
	Scene escenaAvatar;
	cgame::Avatar avatar;
	for (const char* ruta : {"Models/reksai/reksai.obj", "Models/reksai.obj"}) {
		if (std::ifstream(ruta).good() && avatar.attach(escenaAvatar.root, ruta, "Textures/reksai.png",
			jugador.PosicionJugador(), 2.2f)) {
			jugador.avatarArticulado = true;
			break;
		}
	}
	if (!jugador.avatarArticulado) {
		fprintf(stderr, "[ERROR] No se pudo inicializar Rek'Sai desde Models/reksai/ ni Models/.\n");
		return 1;
	}
	SceneNode objetivo("objetivo");
	escenaAvatar.updateWorld();
	if (!VerificarEscenaIntegrada(*avatar.root(), objetivo)) return 1;
	CameraDirector director;
	director.setMode(CameraMode::Planeta);
	ParadaR4 observado = Lanzamiento;
	cgame::Hud hud;
	hud.initialize();
	ZonaActiva ultimoPlaneta = ZONA_PLANETA_ROCOSO;

	ReiniciarCamara(jugador.PosicionJugador() + glm::vec3(0.0f, 6.0f, 0.0f), yawInicio, -15.0f);
	printf("[QA] Inicio: jugador=(%.1f, %.1f, %.1f) camara=(%.1f, %.1f, %.1f) dRoca=%.1f dSelva=%.1f\n",
		jugador.PosicionJugador().x, jugador.PosicionJugador().y, jugador.PosicionJugador().z,
		camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z,
		glm::distance(jugador.PosicionJugador(), posPlanetaRocoso),
		glm::distance(jugador.PosicionJugador(), posPlanetaSelvatico));
	fflush(stdout);

	ConstruirGuiaSatelite();

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, sólo 1 y siempre debe de existir
	// (NUEVO: en el while se vuelve a crear en cada cuadro con el ciclo de día y noche)
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.30f, 0.70f,
		0.25f, 0.60f, 0.40f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Luz del motor izquierdo de la nave rival (solo en el espacio)
	pointLights[0] = PointLight(0.2f, 0.45f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//Luz del motor derecho de la nave rival
	pointLights[1] = PointLight(0.2f, 0.45f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//Luminaria 1 junto a la cápsula de los planetas (se prende de noche)
	pointLights[2] = PointLight(0.2f, 0.45f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//Luminaria 2
	pointLights[3] = PointLight(0.2f, 0.45f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	//Luz de la llama del cohete D1
	pointLights[4] = PointLight(1.0f, 0.5f, 0.15f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna (ligada a la cámara, tecla F)
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.02f, 0.004f,
		10.0f);
	spotLightCount++;

	//faro de miNave (va al frente de la nave del jugador, tecla L)
	spotLights[1] = SpotLight(1.0f, 0.95f, 0.8f,
		0.0f, 4.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		1.0f, 0.02f, 0.002f,
		22.0f);
	spotLightCount++;

	//foco fijo de la base de inicio: alumbra hacia abajo a miNave estacionada (tecla J)
	glm::vec3 centroBase, adelanteBase, derechaBase;
	float radioBase;
	espacio.DatosBase(centroBase, adelanteBase, derechaBase, radioBase);
	spotLights[2] = SpotLight(1.0f, 0.85f, 0.6f,
		0.1f, 1.2f,
		centroBase.x, centroBase.y + 18.0f, centroBase.z,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.01f, 0.001f,
		35.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset = 0;
	GLuint uniformColor = 0;
	GLuint uniformNiebla = 0, uniformEmision = 0;   // NUEVO
	// lejano en 1800 (en la práctica 1000) porque la ola gigante y las montañas están muy lejos
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1800.0f);

	//esfera para el foco de las luminarias (como el foco de la pinza en la práctica 9)
	Sphere sp = Sphere(1.0f, 20, 20);
	sp.init();
	sp.load();

	lastTime = (GLfloat)glfwGetTime();

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = (GLfloat)glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;
		// NUEVO: en la práctica se hacía deltaTime += (now - lastTime) / limitFPS (velocidades por
		// cuadro). Aquí todas las velocidades están en unidades por SEGUNDO, por eso se usa el tiempo
		// real; se limita a 0.05 s para que no haya un brinco después de cargar los modelos.
		if (deltaTime > 0.05f)
			deltaTime = 0.05f;
		if (deltaTime < 0.0f)
			deltaTime = 0.0f;

		//Recibir eventos del usuario
		glfwPollEvents();
		auto entrada = captura.update(deltaTime);
		if (pruebaRelease && pasosRelease[pasoPrueba].tecla)
			entrada.pressed[pasosRelease[pasoPrueba].tecla] = true;
		const auto control = cgame::mapControls(entrada);
		const glm::vec3 anterior = jugador.PosicionJugador();
		const auto zonaAnterior = zonaActual;
		const auto modoAnterior = director.mode();
		ZonaActiva destino = zonaActual;
		if (control.planetMode) {
			director.setMode(CameraMode::Planeta);
			if (zonaActual == ZONA_ESPACIO || zonaActual == ZONA_PLANETA_OCEANO) destino = ultimoPlaneta;
		}
		if (control.raceMode || control.flightMode) {
			if (control.raceMode) mainWindow.SalirDeCamaraLibre();
			director.setMode(control.raceMode ? CameraMode::Carrera : CameraMode::Vuelo);
			if (control.raceMode) {
				auto* seguimiento = static_cast<Chase*>(director.rig());
				seguimiento->distancia = 17.0f;
				seguimiento->altura = 8.0f;
			}
			destino = ZONA_ESPACIO;
		}
		if (control.observationMode) director.setMode(CameraMode::Observacion);
		if (entrada.wasPressed(GLFW_KEY_F5)) destino = ZONA_PLANETA_SELVATICO;
		if (entrada.wasPressed(GLFW_KEY_F6)) destino = ZONA_PLANETA_ROCOSO;
		if (entrada.wasPressed(GLFW_KEY_F7)) destino = ZONA_PLANETA_OCEANO;
		if (entrada.wasPressed(GLFW_KEY_F8)) destino = ZONA_ESPACIO;
		if (director.mode() == CameraMode::Observacion &&
			(entrada.wasPressed(GLFW_KEY_F5) || entrada.wasPressed(GLFW_KEY_F6) ||
			 entrada.wasPressed(GLFW_KEY_F7) || entrada.wasPressed(GLFW_KEY_F8)))
			director.setMode(CameraMode::Planeta);
		if (destino != zonaActual) {
			if (destino == ZONA_ESPACIO) SalirAlEspacio();
			else {
				PrepararRecursosSuperficie(destino);
				zonaActual = destino;
				auto tipo = destino == ZONA_PLANETA_SELVATICO ? SUPERFICIE_SELVATICA : SUPERFICIE_ROCOSA;
				float yaw = destino == ZONA_PLANETA_OCEANO ? jugador.ColocarJugadorEnOceano()
					: jugador.ColocarJugadorEnPlaneta(LugarDelJugador(), superficie.PosicionInicio(tipo), superficie.YawInicio(tipo));
				ReiniciarCamara(jugador.PosicionJugador() + glm::vec3(0,4,0), yaw, -10);
			}
		}
		if (control.planetMode && jugador.JugadorEnNave() && (zonaActual == ZONA_PLANETA_ROCOSO || zonaActual == ZONA_PLANETA_SELVATICO))
			jugador.BajarOSubirNave(LugarDelJugador());
		if (control.raceMode || control.flightMode) jugador.SeleccionarNave(tiempoAnimacion);
		if (camaraLibre)
			camera.keyControl(mainWindow.getsKeys(), deltaTime);   // con la cámara del avatar, W A S D mueven al avatar
		camera.mouseControl(control.mouseDeltaX, control.mouseDeltaY);

		// NUEVO: entrar y salir de los planetas, subir a la nave y cambio de cámara (teclas E, K y C)
		const ParadaR4 previa = observado;
		bool enR4 = director.mode() == CameraMode::Observacion;
		if (enR4 && modoAnterior != director.mode())
			observado = ParadaDisponible(Lanzamiento) ? Lanzamiento : Hangar;
		if (enR4 && control.nextObservationTarget) observado = SiguienteParada(observado);
		if (!enR4) ActualizarZona();
		ZonaActiva zonaVista = enR4 ? encuadres[observado].zona : zonaActual;

		if (control.launchRocket)
		{
			const bool mirandoCohete = observado == Lanzamiento || observado == CoheteBase;
			if ((!enR4 || mirandoCohete) && espacio.CoheteDisponible() &&
				(zonaVista == ZONA_ESPACIO || zonaVista == ZONA_PLANETA_SELVATICO)) {
				bool shinobi = zonaVista == ZONA_PLANETA_SELVATICO;
				if (espacio.LanzarCohete(shinobi ? COHETE_SHINOBI : COHETE_BASE)
					&& !jugador.CarreraIniciada() && !camaraLibre) {
					observado = shinobi ? Lanzamiento : CoheteBase;
					director.setMode(CameraMode::Observacion);
					enR4 = true;
				}
			}
		}
		if (enR4 && (modoAnterior != director.mode() || previa != observado)) {
			const auto& cuadro = encuadres[observado];
			zonaVista = cuadro.zona;
			PrepararRecursosSuperficie(zonaVista);
			auto* orbita = static_cast<Orbit*>(director.rig());
			orbita->distancia = cuadro.distancia;
			orbita->pitch = cuadro.pitch;
			orbita->alturaObjetivo = cuadro.altura;
			orbita->velocidadAngular = 0.04f;
			orbita->yaw = 0.2f;
		}

		if (!enR4 && !camaraLibre)
		{
			// el avatar se mueve con W A S D y la cámara lo sigue en tercera persona
			jugador.ActualizarJugador(mainWindow.getsKeys(), deltaTime, LugarDelJugador(), camera, tiempoAnimacion, director.mode() == CameraMode::Vuelo);
		}
		else if (!enR4 && zonaActual == ZONA_PLANETA_OCEANO)
		{
			// cámara libre en el océano: no se mete bajo el agua ni dentro de la ola
			camera.setPosition(oceano.AjustarCamaraOceano(camera.getCameraPosition()));
		}
		else if (!enR4 && zonaActual != ZONA_ESPACIO && camera.getCameraPosition().y < 1.5f)
		{
			// cámara libre en un planeta: vuela por donde quiera, pero no atraviesa el suelo
			glm::vec3 p = camera.getCameraPosition();
			p.y = 1.5f;
			camera.setPosition(p);
		}

		// animaciones
		tiempoAnimacion += deltaTime;
		espacio.ActualizarCohetes(deltaTime);
		AnimarSatelite(deltaTime);
		AnimarCapsula(deltaTime);

		//textura con movimiento: el agua del océano corre despacio (u, v aumentan y se reinician en 1)
		toffsetaguav += 0.03f * deltaTime;
		if (toffsetaguav > 1.0f)
			toffsetaguav = 0.0f;

		// NUEVO: ciclo de día y noche (cambia la luz direccional)
		float factorDia = ActualizarCicloDiaNoche(zonaVista);
		bool esDeNoche = (factorDia < 0.45f);

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// R4 puede dibujar una zona distinta de la que ocupa el jugador.
		AmbienteSuperficie ambiente;
		ambiente.colorNiebla = glm::vec3(0.0f);
		ambiente.densidadNiebla = 0.0f;
		ambiente.tinteCielo = glm::vec3(1.0f);
		ambiente.brumaHorizonte = glm::vec4(0.0f);
		skybox.SetCieloSolido(glm::vec3(0.0f), 0.0f);   // solo el océano usa cielo despejado
		if (zonaVista == ZONA_PLANETA_OCEANO)
		{
			// cielo despejado y gris azulado
			AmbienteOceano ao = oceano.ObtenerAmbienteOceano(factorDia);
			ambiente.colorNiebla = ao.colorNiebla;
			ambiente.densidadNiebla = ao.densidadNiebla;
			ambiente.tinteCielo = ao.tinteCielo;
			ambiente.brumaHorizonte = ao.brumaHorizonte;
			skybox.SetCieloSolido(ao.colorCenit, ao.cieloSolido);
		}
		else if (zonaVista != ZONA_ESPACIO)
		{
			ambiente = superficie.ObtenerAmbiente(zonaVista == ZONA_PLANETA_ROCOSO ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA, factorDia);
		}
		skybox.SetAmbiente(ambiente.tinteCielo, ambiente.brumaHorizonte);
		if (zonaActual == ZONA_PLANETA_ROCOSO || zonaActual == ZONA_PLANETA_SELVATICO) ultimoPlaneta = zonaActual;
		glm::vec3 pos = jugador.PosicionJugador();
		glm::vec3 paso = pos - anterior;
		float recorrido = glm::length(paso);
		avatar.setPosition(pos);
		avatar.update(deltaTime, recorrido > 0.0001f ? paso / recorrido : glm::vec3(0), recorrido > 0.0001f ? 1.0f : 0.0f);
		avatar.root()->visible = !jugador.JugadorEnNave();
		escenaAvatar.updateWorld();
		objetivo.position = pos;
		objetivo.rotationEuler.y = jugador.Rumbo();
		if (enR4) {
			const auto& cuadro = encuadres[observado];
			auto* orbita = static_cast<Orbit*>(director.rig());
			orbita->distancia = glm::clamp(orbita->distancia - control.observationZoom * 2.0f, cuadro.minima, 180.0f);
			objetivo.position = CentroObservado(observado);
		}
		objetivo.updateWorld();
		if (director.mode() == CameraMode::Carrera) {
			static_cast<Chase*>(director.rig())->Orbitar(control.mouseDeltaX, control.mouseDeltaY);
		} else if (director.mode() != CameraMode::Observacion) {
			director.rig()->miradaMouse = camera.getCameraDirection();
		}
		director.update(deltaTime, objetivo);
		const glm::mat4 vista = camaraLibre && !enR4 ? camera.calculateViewMatrix() : director.view();
		projection = director.projection(float(mainWindow.getBufferWidth()) / mainWindow.getBufferHeight());
		if (zonaAnterior != zonaActual || modoAnterior != director.mode() || control.nextObservationTarget) {
			printf("Zona=%d camara=%d objetivo=%d nave=%d\n", int(zonaActual), int(director.mode()), observado, jugador.JugadorEnNave());
			if (enR4) printf("R4 %s zonaVista=%d distancia=%.1f\n", encuadres[observado].nombre, int(zonaVista), static_cast<Orbit*>(director.rig())->distancia);
			fflush(stdout);
		}
		const glm::mat4 inversa = glm::inverse(vista);
		const glm::vec3 ojo(inversa[3]), mirada(-glm::vec3(inversa[2]));
		Fondo fondo = Fondo::Espacio;
		if (zonaVista == ZONA_PLANETA_ROCOSO) fondo = Fondo::Roca;
		if (zonaVista == ZONA_PLANETA_SELVATICO) fondo = Fondo::Selva;
		skybox.DrawSkybox(fondo, vista, projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation(); // para la textura con movimiento
		uniformNiebla = shaderList[0].getNieblaLocation();		// NUEVO: niebla de los planetas
		uniformEmision = shaderList[0].getEmisionLocation();	// NUEVO: brillo propio

		//información en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(vista));
		glUniform3f(uniformEyePosition, ojo.x, ojo.y, ojo.z);

		// NUEVO: niebla (solo en los planetas) y brillo propio en cero
		glUniform4f(uniformNiebla, ambiente.colorNiebla.x, ambiente.colorNiebla.y, ambiente.colorNiebla.z, ambiente.densidadNiebla);
		glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);

		// luz ligada a la cámara de tipo flash (linterna, tecla F)
		//sirve para que en tiempo de ejecución (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = ojo;
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, mirada);
		if (mainWindow.getLinterna())
			spotLights[0].SetColor(glm::vec3(1.0f, 1.0f, 1.0f));
		else
			spotLights[0].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));

		// faro de miNave (tecla L): solo cuando el jugador va en su nave
		if (mainWindow.getLuzNave() && zonaVista == zonaActual && jugador.JugadorEnNave())
		{
			glm::vec3 posFaro, dirFaro;
			jugador.PuntoFaroNave(posFaro, dirFaro);
			spotLights[1].SetFlash(posFaro, dirFaro);
			spotLights[1].SetColor(glm::vec3(1.0f, 0.95f, 0.8f));
		}
		else
			spotLights[1].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));

		// foco de la base (tecla J): la base solo existe en el espacio
		if (mainWindow.getFocoBase() && zonaVista == ZONA_ESPACIO)
			spotLights[2].SetColor(glm::vec3(1.0f, 0.85f, 0.6f));
		else
			spotLights[2].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));

		// las luces puntuales se apagan con SetColor(0): con SetIntensity(0, 0) todavía dejan brillo especular
		for (unsigned int i = 0; i < pointLightCount; i++)
			pointLights[i].SetColor(glm::vec3(0.0f, 0.0f, 0.0f));

		//Reinicializando variables cada ciclo de reloj
		glm::mat4 model(1.0);
		glm::mat4 modelaux(1.0);
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		if (zonaVista == ZONA_ESPACIO)
		{
			//=========================== ESPACIO ===========================
			// nave rival: espera en la meta hasta que el jugador sube a miNave (tecla K)
			glm::vec3 posNave, tanNave, arribaRival;
			LugarJugador lugarRival;
			jugador.EstadoRival(RivalCarrera::Primero, tiempoAnimacion, lugarRival, posNave, tanNave, arribaRival);
			bool naveVisible = (lugarRival == LUGAR_ESPACIO);   // si no, va corriendo dentro de un planeta

			glm::vec3 objetivo = glm::normalize(tanNave);
			naveForward = glm::normalize(glm::mix(naveForward, objetivo, fminf(1.0f, 8.0f * deltaTime)));
			glm::mat4 modelNave = MatrizOrientacion(posNave, naveForward, glm::vec3(0.0f, 1.0f, 0.0f));

			// luces de los motores de la nave rival (posición en el modelo de la nave)
			glm::vec3 motor1Pos = glm::vec3(modelNave * glm::vec4(0.5f, 0.10f, -2.72f, 1.0f));
			glm::vec3 motor2Pos = glm::vec3(modelNave * glm::vec4(-0.5f, 0.10f, -2.72f, 1.0f));
			pointLights[0].SetPos(motor1Pos);
			pointLights[1].SetPos(motor2Pos);
			if (naveVisible)
			{
				pointLights[0].SetColor(glm::vec3(0.2f, 0.45f, 1.0f));
				pointLights[1].SetColor(glm::vec3(0.2f, 0.45f, 1.0f));
			}
			pointLights[4].SetPos(espacio.PosicionLlama(COHETE_BASE));
			if (espacio.CoheteEncendido(COHETE_BASE))
				pointLights[4].SetColor(glm::vec3(1.0f, 0.45f, 0.10f));

			//información al shader de fuentes de iluminación
			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(pointLights, pointLightCount);
			shaderList[0].SetSpotLights(spotLights, spotLightCount);

			// Planeta rocoso (Vacío): gira sobre su eje. Si planetaRoca.obj no se pudo cargar se usa
			// el planeta estilizado. NUEVO: el modelo se centra y se escala con sus medidas (Model.h)
			model = glm::mat4(1.0);
			model = glm::translate(model, posPlanetaRocoso);
			model = glm::rotate(model, 0.12f * tiempoAnimacion, glm::vec3(0.0f, 1.0f, 0.0f));   // ángulo en radianes
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (PlanetaRocoso_M.estaCargado())
			{
				model = glm::scale(model, glm::vec3(radioPlanetaRocoso / PlanetaRocoso_M.getRadio()));
				model = glm::translate(model, -PlanetaRocoso_M.getCentro());
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				PlanetaRocoso_M.RenderModel();
			}
			else
			{
				model = glm::scale(model, glm::vec3(radioPlanetaRocoso / Planeta_M.getRadio()));
				model = glm::translate(model, -Planeta_M.getCentro());
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.62f, 0.52f, 0.38f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				Planeta_M.RenderModel();
			}

			// Planeta selvático (Shinobi)
			model = glm::mat4(1.0);
			model = glm::translate(model, posPlanetaSelvatico);
			model = glm::rotate(model, 0.18f * tiempoAnimacion, glm::vec3(0.0f, 1.0f, 0.0f));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (PlanetaSelvatico_M.estaCargado())
			{
				model = glm::scale(model, glm::vec3(radioPlanetaSelvatico / PlanetaSelvatico_M.getRadio()));
				model = glm::translate(model, -PlanetaSelvatico_M.getCentro());
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				PlanetaSelvatico_M.RenderModel();
			}
			else
			{
				model = glm::scale(model, glm::vec3(radioPlanetaSelvatico / Planeta_M.getRadio()));
				model = glm::translate(model, -Planeta_M.getCentro());
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				color = glm::vec3(0.30f, 0.65f, 0.26f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				plainTexture.UseTexture();
				Planeta_M.RenderModel();
			}

			// NUEVO: cinturón de asteroides, planeta océano, pista de carreras y base (ver Espacio.h y Oceano.h)
			espacio.DibujarCinturon(ojo, mirada, tiempoAnimacion,
				uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, uniformEmision);
			oceano.DibujarPlanetaOceano(tiempoAnimacion, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, uniformEmision);
			espacio.DibujarPista(tiempoAnimacion, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, uniformEmision);
			espacio.DibujarBase(tiempoAnimacion, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, uniformEmision);
			DibujarHangar(sp, uniformModel, uniformColor, uniformEmision, uniformSpecularIntensity, uniformShininess);
			espacio.DibujarCohete(COHETE_BASE, ojo, tiempoAnimacion, uniformModel, uniformColor,
				uniformSpecularIntensity, uniformShininess, uniformEmision);

			// avatar (Paul en la base o miNave en la pista) y miNave estacionada en la base
			if (zonaVista == zonaActual)
				jugador.DibujarJugador(LUGAR_ESPACIO, tiempoAnimacion, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);
			DibujarGuiaSatelite(glm::vec3(1.0f, 0.35f, 0.85f), uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);

			// nave rival
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelNave));
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (naveVisible)
				Nave_M.RenderModel();
			DibujarRivalEn(RivalCarrera::Segundo, LUGAR_ESPACIO, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);

			// Satélite (jerarquía): la base recorre la órbita y los paneles y el aspa heredan su matriz
			glm::vec3 satPos = PosicionSatelite(estadoSatelite, thetaSatelite);
			glm::vec3 satTan = TangenteSatelite(estadoSatelite, thetaSatelite);
			float satYaw = atan2f(satTan.x, satTan.z);
			model = glm::mat4(1.0);
			model = glm::translate(model, satPos);
			model = glm::rotate(model, satYaw, glm::vec3(0.0f, 1.0f, 0.0f));   // ángulo en radianes
			modelaux = model;

			// NUEVO: el satélite se ilumina con una luz direccional que va de la cámara hacia él, para que
			// siempre se vea claro aunque sea de noche; después se regresa la luz del sol
			glm::vec3 dirSat = glm::normalize(ojo - satPos);
			DirectionalLight luzSatelite(1.0f, 1.0f, 1.0f, 0.8f, 0.9f, dirSat.x, dirSat.y, dirSat.z);
			shaderList[0].SetDirectionalLight(&luzSatelite);

			// base del satélite
			color = glm::vec3(1.2f, 1.2f, 1.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			SateliteBase_M.RenderModel();

			// panel derecho: gira desde su unión con la base (tecla 3)
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 1.2f, 4.0f));
			model = glm::rotate(model, rotPanelDer * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			color = glm::vec3(0.15f, 0.4f, 1.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			SatelitePanelDer_M.RenderModel();

			// panel izquierdo
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, -1.2f, 4.0f));
			model = glm::rotate(model, rotPanelIzq * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			SatelitePanelIzq_M.RenderModel();

			// aspa: gira sin parar sobre el eje Z
			model = modelaux;
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
			model = glm::rotate(model, rotacionRotor * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
			color = glm::vec3(1.2f, 1.2f, 1.2f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			SateliteAspa_M.RenderModel();

			shaderList[0].SetDirectionalLight(&mainLight);

			// llamas de los motores (blending aditivo: se suman al color de atrás, como fuego)
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			glDepthMask(GL_FALSE);

			llamaTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// NUEVO (animación continua): las llamas "laten" con un seno, escala = 1 + 0.15 sen(20 t)
			float pulso = 1.0f + 0.15f * sinf(20.0f * tiempoAnimacion);

			model = modelNave;
			model = glm::translate(model, glm::vec3(0.5f, 0.10f, -2.92f));
			model = glm::scale(model, glm::vec3(pulso, pulso, pulso));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (naveVisible)
				meshListModel[1]->RenderMeshModel();

			model = modelNave;
			model = glm::translate(model, glm::vec3(-0.5f, 0.10f, -2.92f));
			model = glm::scale(model, glm::vec3(pulso, pulso, pulso));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			if (naveVisible)
				meshListModel[1]->RenderMeshModel();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
		else if (zonaVista == ZONA_PLANETA_OCEANO)
		{
			//=========================== PLANETA OCÉANO ===========================
			// mar con la ola gigante; las dos naves vuelan sobre el agua (sin luces puntuales)
			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(pointLights, pointLightCount);
			shaderList[0].SetSpotLights(spotLights, spotLightCount);

			toffset = glm::vec2(toffsetaguau, toffsetaguav);
			oceano.DibujarOceano(ojo, toffset, uniformModel, uniformColor, uniformTextureOffset,
				uniformSpecularIntensity, uniformShininess, uniformEmision);
			for (auto rival : {RivalCarrera::Primero, RivalCarrera::Segundo})
				DibujarRivalEn(rival, LUGAR_OCEANO, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);
			jugador.DibujarJugador(LUGAR_OCEANO, tiempoAnimacion, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);
		}
		else
		{
			//=========================== PLANETA ROCOSO O SELVÁTICO ===========================
			TipoSuperficie tipoSuperficie = (zonaVista == ZONA_PLANETA_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
			glm::vec3 posCapsula = superficie.PosicionCapsula(tipoSuperficie);

			// luminarias junto a la cápsula, del lado contrario al camino (el camino pasa por x = 0).
			// Se prenden y se apagan al mismo tiempo según el ciclo de día y noche.
			float ladoCapsula = (posCapsula.x >= 0.0f) ? 1.0f : -1.0f;
			posLuminaria[0] = posCapsula + glm::vec3(6.0f * ladoCapsula, 4.0f, 5.0f);
			posLuminaria[1] = posCapsula + glm::vec3(6.0f * ladoCapsula, 4.0f, -5.0f);
			pointLights[2].SetPos(posLuminaria[0]);
			pointLights[3].SetPos(posLuminaria[1]);
			if (esDeNoche)
			{
				pointLights[2].SetColor(glm::vec3(0.2f, 0.45f, 1.0f));
				pointLights[3].SetColor(glm::vec3(0.2f, 0.45f, 1.0f));
			}
			pointLights[4].SetPos(espacio.PosicionLlama(COHETE_SHINOBI));
			if (zonaVista == ZONA_PLANETA_SELVATICO && espacio.CoheteEncendido(COHETE_SHINOBI))
				pointLights[4].SetColor(glm::vec3(1.0f, 0.45f, 0.10f));

			//información al shader de fuentes de iluminación
			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(pointLights, pointLightCount);
			shaderList[0].SetSpotLights(spotLights, spotLightCount);

			// NUEVO: piso, camino, árboles, rocas, cristales, NPCs y portal (ver Superficie.h)
			superficie.DibujarSuperficie(tipoSuperficie, ojo, mirada, factorDia, tiempoAnimacion,
				uniformModel, uniformColor, uniformTextureOffset, uniformSpecularIntensity, uniformShininess, uniformEmision);
			if (zonaVista == ZONA_PLANETA_SELVATICO)
				espacio.DibujarCohete(COHETE_SHINOBI, ojo, tiempoAnimacion, uniformModel, uniformColor,
					uniformSpecularIntensity, uniformShininess, uniformEmision);
			if (zonaVista == zonaActual)
				jugador.DibujarJugador(LugarDelJugador(), tiempoAnimacion, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);   // la nave del jugador sobre el camino
			for (auto rival : {RivalCarrera::Primero, RivalCarrera::Segundo})
				DibujarRivalEn(rival, tipoSuperficie == SUPERFICIE_ROCOSA ? LUGAR_ROCOSO : LUGAR_SELVATICO,
					uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);

			// Cápsula de regreso (jerarquía): la puerta hereda la matriz del cuerpo y gira sobre su bisagra.
			// La cápsula no trae texturas, solo colores en su .mtl: RenderModel(uniformColor, tinte) manda
			// el color de cada material (ver Model.h).
			model = glm::mat4(1.0);
			model = glm::translate(model, posCapsula);
			model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
			model = glm::translate(model, -origenLocalCapsula);   // la base del modelo queda en el piso
			modelaux = model;
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			CapsulaCuerpo_M.RenderModel(uniformColor, glm::vec3(1.0f, 1.0f, 1.0f));

			// puerta (tecla 2): T(bisagra) * R(ángulo, eje de la bisagra) * T(-bisagra)
			model = modelaux;
			model = glm::translate(model, pivoteBisagraPuerta);
			model = glm::rotate(model, anguloPuertaCapsula * toRadians, ejeBisagraPuerta);
			model = glm::translate(model, -pivoteBisagraPuerta);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			CapsulaPuerta_M.RenderModel(uniformColor, glm::vec3(1.0f, 1.0f, 1.0f));

			// luminarias: poste (cubo de CreateObjects) y foco (esfera). De noche el foco brilla.
			for (int i = 0; i < 2; i++)
			{
				// poste
				model = glm::mat4(1.0);
				model = glm::translate(model, glm::vec3(posLuminaria[i].x, 0.5f * (posLuminaria[i].y - 0.45f), posLuminaria[i].z));
				model = glm::scale(model, glm::vec3(0.25f, posLuminaria[i].y - 0.45f, 0.25f));
				color = glm::vec3(0.25f, 0.25f, 0.3f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				plainTexture.UseTexture();
				Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshListModel[2]->RenderMeshModel();

				// foco
				model = glm::mat4(1.0);
				model = glm::translate(model, posLuminaria[i]);
				model = glm::scale(model, glm::vec3(0.45f, 0.45f, 0.45f));
				color = glm::vec3(0.6f, 0.8f, 1.0f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				if (esDeNoche)
					glUniform3f(uniformEmision, 0.4f, 0.7f, 1.0f);   // NUEVO: brillo propio del foco encendido
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				sp.render();
				glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
			}
		}

		// NUEVO: reloj en pantalla. 120 s del ciclo = 24 horas; el ciclo empieza al amanecer (6:00)
		float horaDelDia = fmodf(6.0f + 24.0f * tiempoAnimacion / duracionCicloDiaNoche, 24.0f);
		int dia = 1 + (int)((tiempoAnimacion + 0.25f * duracionCicloDiaNoche) / duracionCicloDiaNoche);   // cambia a medianoche
		int clima = 0;
		if (zonaVista == ZONA_PLANETA_SELVATICO) clima = 1;
		else if (zonaVista == ZONA_PLANETA_ROCOSO) clima = 2;
		else if (zonaVista == ZONA_PLANETA_OCEANO) clima = 3;   // despejado
		glUniform2f(uniformTextureOffset, 0, 0);
		glUniform3f(uniformEmision, 0, 0, 0);
		if (zonaVista == zonaActual) escenaAvatar.render(shaderList[0], vista, projection);
		const char* modos[] = {"PLANETA", "CARRERA", "VUELO", "OBSERVACION"};
		const char* zonas[] = {"ESPACIO", "ROCA", "SELVA", "OCEANO"};
		char estadoHud[80];
		const char* estadoCohete = "";
		if (zonaVista == ZONA_ESPACIO && (!enR4 || observado == CoheteBase))
			estadoCohete = espacio.TextoEstadoCohete(COHETE_BASE);
		else if (zonaVista == ZONA_PLANETA_SELVATICO)
			estadoCohete = espacio.TextoEstadoCohete(COHETE_SHINOBI);
		snprintf(estadoHud, sizeof(estadoHud), "%s  %s  DIA %d  %02d:%02d", enR4 ? encuadres[observado].nombre : (jugador.JugadorEnNave() ? "NAVE" : "REKSAI"),
			estadoCohete, dia, int(horaDelDia), int(horaDelDia * 60) % 60);
		hud.draw(mainWindow.getBufferWidth(), mainWindow.getBufferHeight(), camaraLibre && !enR4 ? "LIBRE" : modos[static_cast<int>(director.mode())], zonas[zonaVista], estadoHud);
		if (!enR4) reloj.DibujarReloj((int)mainWindow.getBufferWidth(), (int)mainWindow.getBufferHeight(), horaDelDia, dia, clima);

		glUseProgram(0);

		mainWindow.swapBuffers();
		if (pruebaRelease) {
			const int tecla = pasosRelease[pasoPrueba].tecla;
			if (zonaVista != pasosRelease[pasoPrueba].zonaVista ||
				(tecla == GLFW_KEY_F2 && (!jugador.JugadorEnNave() || director.mode() != CameraMode::Carrera)) ||
				(tecla == GLFW_KEY_1 && !espacio.CoheteEncendido(COHETE_SHINOBI)) ||
				(tecla == GLFW_KEY_TAB && (!enR4 || observado != Hangar))) {
				fprintf(stderr, "[Release] Fallo en el paso %zu (tecla %d).\n", pasoPrueba, tecla);
				return 2;
			}
			if (++pasoPrueba == totalPasos) {
				puts("[Release] OK: espacio, roca, selva, oceano, R2, lanzamiento y hangar.");
				break;
			}
		}
	}

	return pruebaRelease && pasoPrueba != totalPasos ? 2 : 0;
}
