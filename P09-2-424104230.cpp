#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>

#include "Window.h"
#include "Mesh_tn.h"
#include "Shader_light.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "Create.hpp"
#include "Load.hpp"
#include "Input.hpp"

#include "CameraDirector.h"
#include "CameraRig.h"
#include "Chase.h"
#include "Flight.h"
#include "FollowXZ.h"
#include "Orbit.h"
#include "Scene.h"
#include "SceneNode.h"

static const float pi = 3.14159265358979323846f;
static const float aRad = pi / 180.0f;

static glm::vec3 posicionNodo(const SceneNode& nodo)
{
	const glm::vec4 columna = nodo.world[3];
	return glm::vec3(columna.x, columna.y, columna.z);
}

static bool cerca(const glm::vec3& a, const glm::vec3& b)
{
	return glm::length(a - b) < 1e-4f;
}

std::vector<Shader> shaderList;

DirectionalLight mainLight;
PointLight luzHolocron;

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

static const float PISO_Y = -2.0f;
static const float PISO_ESCALA = 10.0f;

static const glm::vec3 REKSAI_POS(0.0f, PISO_Y + 0.12f, 0.0f);
static const float REKSAI_ESCALA = 2.2f;
static const float REKSAI_GIRO_Y = 0.0f;
static Model rekSaiModel;
static bool rekSaiCargado = false;

static void crearShaders()
{
	shaderList.reserve(1);

	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);
}

static void cargarAvatar()
{
	rekSaiModel.LoadModel("Models/avatar/reksai.obj");
	rekSaiCargado = rekSaiModel.getMeshCount() > 0;
	printf("RekSai: %d mallas cargadas\n", rekSaiModel.getMeshCount());
	fflush(stdout);
}

static bool probarJerarquiaEscena()
{
	Scene escena;
	SceneNode* padre = escena.root.add(std::make_unique<SceneNode>("padre"));
	padre->position = glm::vec3(1.0f, 2.0f, 3.0f);
	padre->rotationEuler.y = glm::radians(90.0f);

	SceneNode* hijo = padre->add(std::make_unique<SceneNode>("hijo"));
	hijo->position = glm::vec3(1.0f, 0.0f, 0.0f);

	escena.updateWorld();

	const glm::vec4 columnaHijo = hijo->world[3];
	const glm::vec3 posicionHijo(columnaHijo.x, columnaHijo.y, columnaHijo.z);
	const glm::vec3 posicionEsperada(1.0f, 2.0f, 2.0f);
	const bool transformacionOk = glm::length(posicionHijo - posicionEsperada) < 1e-4f;

	const bool visibleOk = hijo->visibleInHierarchy();
	padre->visible = false;
	const bool ocultoOk = !hijo->visibleInHierarchy();
	padre->visible = true;

	const bool busquedaOk = escena.find("hijo") == hijo;

	const bool ok = transformacionOk && visibleOk && ocultoOk && busquedaOk;
	printf("H-A1 jerarquia: %s (hijo %.3f, %.3f, %.3f)\n",
		ok ? "OK" : "FALLO", posicionHijo.x, posicionHijo.y, posicionHijo.z);
	fflush(stdout);
	return ok;
}

static bool probarFollowXZ()
{
	Scene escena;
	SceneNode* objetivo = escena.root.add(std::make_unique<SceneNode>("objetivo"));
	objetivo->position = glm::vec3(3.0f, 0.0f, -2.0f);
	escena.updateWorld();

	FollowXZ rig(4.0f, 10.0f, 2.0f);
	rig.update(0.016f, *objetivo);

	const glm::vec3 posicionEsperada(3.0f, 4.0f, -12.0f);
	const glm::vec3 objetivoEsperado(3.0f, 2.0f, -2.0f);
	const bool posicionOk = glm::length(rig.posicion() - posicionEsperada) < 1e-4f;
	const bool objetivoOk = glm::length(rig.objetivo() - objetivoEsperado) < 1e-4f;
	const bool ok = posicionOk && objetivoOk;

	printf("A3.2 FollowXZ: %s (camara %.3f, %.3f, %.3f; objetivo %.3f, %.3f, %.3f)\n",
		ok ? "OK" : "FALLO",
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		rig.objetivo().x, rig.objetivo().y, rig.objetivo().z);
	fflush(stdout);
	return ok;
}

static bool probarChase()
{
	Scene escena;
	SceneNode* objetivo = escena.root.add(std::make_unique<SceneNode>("objetivo"));
	objetivo->position = glm::vec3(3.0f, 0.0f, -2.0f);
	objetivo->rotationEuler.y = glm::radians(90.0f);
	escena.updateWorld();

	Chase rig(10.0f, 5.0f, 2.0f, 2.0f);
	rig.update(0.016f, *objetivo);

	const glm::vec3 posicionEsperada(-7.0f, 5.0f, -2.0f);
	const glm::vec3 objetivoEsperado(5.0f, 2.0f, -2.0f);
	const bool posicionOk = cerca(rig.posicion(), posicionEsperada);
	const bool objetivoOk = cerca(rig.objetivo(), objetivoEsperado);
	const bool ok = posicionOk && objetivoOk;

	printf("A4.1 Chase: %s (camara %.3f, %.3f, %.3f; objetivo %.3f, %.3f, %.3f)\n",
		ok ? "OK" : "FALLO",
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		rig.objetivo().x, rig.objetivo().y, rig.objetivo().z);
	fflush(stdout);
	return ok;
}

static bool probarChaseRekSai(const SceneNode& avatar)
{
	Chase rig(10.0f, 5.0f, 2.0f, 2.0f);
	rig.update(0.016f, avatar);

	const glm::vec4 columna = avatar.world[3];
	const glm::vec3 base(columna.x, columna.y, columna.z);
	glm::vec3 frente(avatar.world[2]);
	frente.y = 0.0f;
	frente = glm::normalize(frente);

	const glm::vec3 desplazamiento = rig.posicion() - base;
	const bool detras = glm::dot(glm::vec3(desplazamiento.x, 0.0f, desplazamiento.z), frente) < 0.0f;
	const bool separada = glm::length(rig.objetivo() - rig.posicion()) > 1e-3f;
	const bool finita =
		std::isfinite(rig.posicion().x) && std::isfinite(rig.posicion().y) &&
		std::isfinite(rig.posicion().z) && std::isfinite(rig.objetivo().x) &&
		std::isfinite(rig.objetivo().y) && std::isfinite(rig.objetivo().z);
	const bool ok = detras && separada && finita;

	printf("A4.1 Chase RekSai: %s (avatar %.3f, %.3f, %.3f; camara %.3f, %.3f, %.3f; mallas %d)\n",
		ok ? "OK" : "FALLO",
		base.x, base.y, base.z,
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		rekSaiModel.getMeshCount());
	fflush(stdout);
	return ok;
}

static bool probarFlight()
{
	Scene escena;
	SceneNode* objetivo = escena.root.add(std::make_unique<SceneNode>("objetivo_flight"));
	objetivo->position = glm::vec3(5.0f, 10.0f, -3.0f);
	objetivo->rotationEuler.y = glm::radians(90.0f);
	escena.updateWorld();

	Flight rig(16.0f, 6.0f, 2.0f, 4.0f);
	rig.update(0.016f, *objetivo);

	const glm::vec3 posicionEsperada(-11.0f, 16.0f, -3.0f);
	const glm::vec3 objetivoEsperado(9.0f, 12.0f, -3.0f);
	const bool rectoOk =
		cerca(rig.posicion(), posicionEsperada) &&
		cerca(rig.objetivo(), objetivoEsperado);

	const glm::mat4 vista = rig.view();
	const glm::vec3 ojo = glm::vec3(glm::inverse(vista)[3]);
	const bool vistaOk =
		std::isfinite(ojo.x) && std::isfinite(ojo.y) && std::isfinite(ojo.z) &&
		glm::length(ojo - rig.posicion()) < 1e-3f;

	objetivo->rotationEuler.x = glm::radians(-20.0f);
	escena.updateWorld();

	Flight inclinado(16.0f, 6.0f, 2.0f, 4.0f);
	inclinado.update(0.016f, *objetivo);
	const bool inclinacionOk =
		std::fabs(inclinado.posicion().y - 16.0f) > 0.5f &&
		std::fabs(inclinado.objetivo().y - 12.0f) > 0.5f;

	const bool ok = rectoOk && vistaOk && inclinacionOk;
	printf("A4.2 Flight: %s (camara %.3f, %.3f, %.3f; objetivo %.3f, %.3f, %.3f; inclinada y %.3f/%.3f)\n",
		ok ? "OK" : "FALLO",
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		rig.objetivo().x, rig.objetivo().y, rig.objetivo().z,
		inclinado.posicion().y, inclinado.objetivo().y);
	fflush(stdout);
	return ok;
}

static bool probarFlightNave(const SceneNode& nave)
{
	Flight rig(16.0f, 6.0f, 2.0f, 4.0f);
	rig.update(0.016f, nave);

	const glm::vec3 base = posicionNodo(nave);
	glm::vec3 frente(nave.world[2]);
	const float longitudCuadrada = glm::dot(frente, frente);
	frente = (longitudCuadrada > 1e-6f)
		? frente / std::sqrt(longitudCuadrada)
		: glm::vec3(0.0f, 0.0f, 1.0f);

	const glm::vec3 desplazamiento = rig.posicion() - base;
	const glm::vec3 direccionObjetivo = rig.objetivo() - base;
	const bool detras = glm::dot(glm::normalize(desplazamiento), frente) < 0.0f;
	const bool delante = glm::dot(glm::normalize(direccionObjetivo), frente) > 0.0f;
	const bool separada = glm::length(rig.objetivo() - rig.posicion()) > 1e-3f;
	const bool modeloOk = recursos.naveModel.getMeshCount() > 0;
	const bool finita =
		std::isfinite(rig.posicion().x) && std::isfinite(rig.posicion().y) &&
		std::isfinite(rig.posicion().z) && std::isfinite(rig.objetivo().x) &&
		std::isfinite(rig.objetivo().y) && std::isfinite(rig.objetivo().z);
	const bool ok = detras && delante && separada && modeloOk && finita;

	printf("A4.2 Flight nave: %s (nave %.3f, %.3f, %.3f; camara %.3f, %.3f, %.3f; mallas %d)\n",
		ok ? "OK" : "FALLO",
		base.x, base.y, base.z,
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		recursos.naveModel.getMeshCount());
	fflush(stdout);
	return ok;
}

static bool probarOrbit()
{
	Scene escena;
	SceneNode* objetivo = escena.root.add(std::make_unique<SceneNode>("objetivo_orbit"));
	objetivo->position = glm::vec3(5.0f, 10.0f, -3.0f);
	escena.updateWorld();

	Orbit rig(20.0f, 0.0f, glm::radians(30.0f), 2.0f, 0.0f);
	rig.update(0.016f, *objetivo);

	const glm::vec3 centroEsperado(5.0f, 12.0f, -3.0f);
	const glm::vec3 posicionEsperada(5.0f, 22.0f, 14.320508f);
	const bool rectoOk =
		cerca(rig.posicion(), posicionEsperada) &&
		cerca(rig.objetivo(), centroEsperado);

	const float distancia = glm::length(rig.posicion() - centroEsperado);
	const bool distanciaOk = std::fabs(distancia - 20.0f) < 1e-3f;

	const glm::mat4 vista = rig.view();
	const glm::vec3 ojo = glm::vec3(glm::inverse(vista)[3]);
	const bool vistaOk =
		std::isfinite(ojo.x) && std::isfinite(ojo.y) && std::isfinite(ojo.z) &&
		glm::length(ojo - rig.posicion()) < 1e-3f;

	Orbit girando(20.0f, 0.0f, glm::radians(30.0f), 2.0f, 0.12f);
	girando.update(1.0f, *objetivo);
	const bool giroOk =
		std::fabs(girando.yaw - 0.12f) < 1e-5f &&
		!cerca(girando.posicion(), rig.posicion());

	const bool ok = rectoOk && distanciaOk && vistaOk && giroOk;
	printf("A4.3 Orbit: %s (camara %.3f, %.3f, %.3f; distancia %.3f; yaw %.3f)\n",
		ok ? "OK" : "FALLO",
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		distancia, girando.yaw);
	fflush(stdout);
	return ok;
}

static bool probarOrbitNave(SceneNode& nave)
{
	Orbit rig(20.0f, 0.0f, glm::radians(30.0f), 2.0f, 0.0f);
	rig.update(0.016f, nave);

	const glm::vec3 base = posicionNodo(nave);
	const glm::vec3 centro = base + glm::vec3(0.0f, 2.0f, 0.0f);
	const glm::vec3 posicionInicial = rig.posicion();
	const float distancia = glm::length(posicionInicial - centro);
	const bool distanciaOk = std::fabs(distancia - 20.0f) < 1e-3f;
	const bool separada = glm::length(rig.objetivo() - rig.posicion()) > 1e-3f;
	const bool modeloOk = recursos.naveModel.getMeshCount() > 0;

	const glm::mat4 vista = rig.view();
	const glm::vec3 ojo = glm::vec3(glm::inverse(vista)[3]);
	const bool vistaOk =
		std::isfinite(ojo.x) && std::isfinite(ojo.y) && std::isfinite(ojo.z) &&
		glm::length(ojo - rig.posicion()) < 1e-3f;

	const glm::vec3 rotacionOriginal = nave.rotationEuler;
	nave.rotationEuler.y += glm::radians(45.0f);
	nave.rotationEuler.x += glm::radians(15.0f);
	nave.updateWorld();
	rig.update(0.016f, nave);
	const bool independiente = cerca(rig.posicion(), posicionInicial);
	nave.rotationEuler = rotacionOriginal;
	nave.updateWorld();

	const bool ok = distanciaOk && separada && modeloOk && vistaOk && independiente;
	printf("A4.3 Orbit nave: %s (nave %.3f, %.3f, %.3f; camara %.3f, %.3f, %.3f; mallas %d)\n",
		ok ? "OK" : "FALLO",
		base.x, base.y, base.z,
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		recursos.naveModel.getMeshCount());
	fflush(stdout);
	return ok;
}

static bool vistaEn(const CameraDirector& director, const glm::vec3& esperada)
{
	const glm::mat4 vista = director.view();
	const glm::vec3 ojo = glm::vec3(glm::inverse(vista)[3]);
	return cerca(ojo, esperada) &&
		std::isfinite(ojo.x) && std::isfinite(ojo.y) && std::isfinite(ojo.z);
}

static bool probarCameraDirector()
{
	Scene escena;
	SceneNode* objetivo = escena.root.add(std::make_unique<SceneNode>("objetivo_director"));
	objetivo->position = glm::vec3(3.0f, 0.0f, -2.0f);
	objetivo->rotationEuler.y = glm::radians(90.0f);
	escena.updateWorld();

	CameraDirector director;
	const bool cambioPlaneta = director.setMode(CameraMode::Planeta);
	director.update(0.0f, *objetivo);
	const bool planetaOk =
		cambioPlaneta &&
		director.mode() == CameraMode::Planeta &&
		director.rig() != nullptr &&
		vistaEn(director, glm::vec3(3.0f, 6.0f, -14.0f));

	const CameraRig* rigPlaneta = director.rig();
	const bool mismoModo = !director.setMode(CameraMode::Planeta);
	const bool mismoRig = director.rig() == rigPlaneta;

	const bool cambioCarrera = director.setMode(CameraMode::Carrera);
	director.update(0.0f, *objetivo);
	const bool carreraOk =
		cambioCarrera &&
		director.mode() == CameraMode::Carrera &&
		director.rig() != nullptr &&
		vistaEn(director, glm::vec3(-7.0f, 5.0f, -2.0f));

	const bool cambioVuelo = director.setMode(CameraMode::Vuelo);
	director.update(0.0f, *objetivo);
	const bool vueloOk =
		cambioVuelo &&
		director.mode() == CameraMode::Vuelo &&
		director.rig() != nullptr &&
		vistaEn(director, glm::vec3(-13.0f, 6.0f, -2.0f));

	const bool cambioObservacion = director.setMode(CameraMode::Observacion);
	director.update(0.0f, *objetivo);
	const bool observacionOk =
		cambioObservacion &&
		director.mode() == CameraMode::Observacion &&
		director.rig() != nullptr &&
		vistaEn(director, glm::vec3(3.0f, 12.0f, 15.320508f));

	const bool f1Ok =
		director.setModeForKey(GLFW_KEY_F1) &&
		director.mode() == CameraMode::Planeta;
	const bool f2Ok =
		director.setModeForKey(GLFW_KEY_F2) &&
		director.mode() == CameraMode::Carrera;
	const bool f3Ok =
		director.setModeForKey(GLFW_KEY_F3) &&
		director.mode() == CameraMode::Vuelo;
	const bool f4Ok =
		director.setModeForKey(GLFW_KEY_F4) &&
		director.mode() == CameraMode::Observacion;

	const CameraMode modoAnterior = director.mode();
	const bool teclaInvalidaOk =
		!director.setModeForKey(GLFW_KEY_F5) &&
		director.mode() == modoAnterior;

	const bool ok = planetaOk && mismoModo && mismoRig && carreraOk && vueloOk &&
		observacionOk && f1Ok && f2Ok && f3Ok && f4Ok && teclaInvalidaOk;

	printf("A4.4 CameraDirector: %s (Planeta/Carrera/Vuelo/Observacion; F1-F4)\n",
		ok ? "OK" : "FALLO");
	fflush(stdout);
	return ok;
}

static bool probarPivotes()
{
	Scene escena;
	SceneNode* raiz = escena.root.add(std::make_unique<SceneNode>("raiz_avatar"));
	raiz->position = glm::vec3(2.0f, 0.0f, 3.0f);
	raiz->rotationEuler.y = glm::radians(90.0f);

	SceneNode* pivote = raiz->add(std::make_unique<SceneNode>("pivote"));
	pivote->position = glm::vec3(1.0f, 0.0f, 0.0f);

	SceneNode* hoja = pivote->add(std::make_unique<SceneNode>("hoja"));
	hoja->position = glm::vec3(1.0f, 0.0f, 1.0f);

	escena.updateWorld();

	const glm::vec3 pivoteInicial = posicionNodo(*pivote);
	const glm::vec3 hojaInicial = posicionNodo(*hoja);
	const bool transformacionOk =
		cerca(pivoteInicial, glm::vec3(2.0f, 0.0f, 2.0f)) &&
		cerca(hojaInicial, glm::vec3(3.0f, 0.0f, 1.0f));

	pivote->rotationEuler.y = glm::radians(90.0f);
	escena.updateWorld();

	const glm::vec3 raizRotada = posicionNodo(*raiz);
	const glm::vec3 hojaRotada = posicionNodo(*hoja);
	const bool rotacionOk =
		cerca(hojaRotada, glm::vec3(1.0f, 0.0f, 1.0f)) &&
		cerca(raizRotada, glm::vec3(2.0f, 0.0f, 3.0f));

	const bool visibleOk = hoja->visibleInHierarchy();
	pivote->visible = false;
	const bool ocultoOk = !hoja->visibleInHierarchy();
	pivote->visible = true;

	const bool busquedaOk = escena.find("hoja") == hoja;
	const bool pivotesOk = transformacionOk && rotacionOk && visibleOk &&
		ocultoOk && busquedaOk;

	printf("A3.3 pivotes: %s (pivote %.3f, %.3f, %.3f; hoja %.3f, %.3f, %.3f)\n",
		pivotesOk ? "OK" : "FALLO",
		pivoteInicial.x, pivoteInicial.y, pivoteInicial.z,
		hojaRotada.x, hojaRotada.y, hojaRotada.z);
	fflush(stdout);

	raiz->position.x = 5.0f;
	escena.updateWorld();

	FollowXZ rig;
	rig.update(0.016f, *raiz);

	const bool camaraOk =
		cerca(rig.posicion(), glm::vec3(5.0f, 6.0f, -9.0f)) &&
		cerca(rig.objetivo(), glm::vec3(5.0f, 2.0f, 3.0f));

	printf("A3.3 camara-raiz: %s (camara %.3f, %.3f, %.3f; objetivo %.3f, %.3f, %.3f)\n",
		camaraOk ? "OK" : "FALLO",
		rig.posicion().x, rig.posicion().y, rig.posicion().z,
		rig.objetivo().x, rig.objetivo().y, rig.objetivo().z);
	fflush(stdout);

	return pivotesOk && camaraOk;
}

static const SceneNode* actualizarCamaraPrueba(Window& ventana, CameraDirector& director,
	const SceneNode& avatar, const SceneNode& nave)
{
	static bool anterior[4] = { false, false, false, false };
	static const SceneNode* objetivo = &avatar;

	bool* teclas = ventana.getsKeys();
	const int teclasModo[4] = { GLFW_KEY_F1, GLFW_KEY_F2, GLFW_KEY_F3, GLFW_KEY_F4 };
	const SceneNode* objetivos[4] = { &avatar, &avatar, &nave, &nave };
	const char* nombres[4] = { "Planeta", "Carrera", "Vuelo", "Observacion" };

	for (int i = 0; i < 4; ++i)
	{
		const bool presionada = teclas[teclasModo[i]];
		if (presionada && !anterior[i] && director.setModeForKey(teclasModo[i]))
		{
			objetivo = objetivos[i];
			printf("Camara: %s\n", nombres[i]);
			fflush(stdout);
		}
		anterior[i] = presionada;
	}

	return objetivo;
}

int main()
{
	probarJerarquiaEscena();
	probarFollowXZ();
	probarChase();
	probarFlight();
	probarOrbit();
	probarCameraDirector();
	probarPivotes();

	Window mainWindow(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	crearShaders();

	LoadTextures();
	LoadLlamas();
	LoadSkyboxCupertin();
	cargarAvatar();
	LoadNave();

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.45f, 0.85f,
		0.35f, 0.75f, 0.40f);

	Scene escena;
	escena.setFallbackTexture(&recursos.plainTexture);

	SceneNode* nodoPiso = escena.root.add(std::make_unique<SceneNode>("piso"));
	nodoPiso->setMesh(geometria.pisoMesh);
	nodoPiso->setTexture(&recursos.pisoTexture);
	nodoPiso->position = glm::vec3(0.0f, PISO_Y, 0.0f);
	nodoPiso->scale = glm::vec3(PISO_ESCALA, 1.0f, PISO_ESCALA);

	SceneNode* nodoAvatar = escena.root.add(std::make_unique<SceneNode>("avatar"));
	nodoAvatar->position = REKSAI_POS;
	nodoAvatar->rotationEuler.y = glm::radians(REKSAI_GIRO_Y);
	nodoAvatar->scale = glm::vec3(REKSAI_ESCALA);
	nodoAvatar->specular = 4.0f;
	nodoAvatar->shininess = 256.0f;
	if (rekSaiCargado)
	{
		nodoAvatar->setModel(&rekSaiModel);
	}

	SceneNode* pivotePrueba = nodoAvatar->add(std::make_unique<SceneNode>("pivote_prueba"));
	pivotePrueba->position = glm::vec3(0.0f, 1.0f, 0.0f);

	SceneNode* marcadorPrueba = pivotePrueba->add(std::make_unique<SceneNode>("marcador_prueba"));
	marcadorPrueba->position = glm::vec3(0.0f, 0.0f, 0.8f);
	marcadorPrueba->scale = glm::vec3(0.35f);
	marcadorPrueba->tint = glm::vec3(1.0f, 0.0f, 1.0f);
	marcadorPrueba->setMesh(geometria.vegetacionMesh);
	marcadorPrueba->setTexture(&recursos.plainTexture);

	SceneNode* nodoNave = escena.root.add(std::make_unique<SceneNode>("nave_prueba"));
	nodoNave->position = glm::vec3(18.0f, 8.0f, 0.0f);
	nodoNave->rotationEuler.y = glm::radians(30.0f);
	nodoNave->rotationEuler.x = glm::radians(-10.0f);
	nodoNave->scale = glm::vec3(0.5f);
	if (recursos.naveModel.getMeshCount() > 0)
	{
		nodoNave->setModel(&recursos.naveModel);
	}

	escena.updateWorld();
	probarChaseRekSai(*nodoAvatar);
	probarFlightNave(*nodoNave);
	probarOrbitNave(*nodoNave);

	CameraDirector director;
	director.setMode(CameraMode::Planeta);
	const SceneNode* objetivoCamara = nodoAvatar;

	const float aspecto = (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight();
	const glm::mat4 projection = director.projection(aspecto);

	GLfloat lastTime = 0.0f;

	while (!mainWindow.getShouldClose())
	{
		float now = (float)glfwGetTime();
		float dt = now - lastTime;
		lastTime = now;
		if (dt > 0.05f) dt = 0.05f;
		if (dt < 0.0f) dt = 0.0f;

		glfwPollEvents();
		objetivoCamara = actualizarCamaraPrueba(mainWindow, director, *nodoAvatar, *nodoNave);

		const float t = (float)glfwGetTime();
		nodoAvatar->position.x = 4.0f * sinf(t * 0.5f);
		nodoAvatar->position.z = 4.0f * cosf(t * 0.5f);
		nodoAvatar->rotationEuler.y = glm::radians(REKSAI_GIRO_Y) + 0.5f * sinf(t * 0.25f);
		pivotePrueba->rotationEuler.y = 1.2f * sinf(t * 1.5f);

		escena.updateWorld();
		director.update(dt, *objetivoCamara);

		const glm::mat4 vista = director.view();
		const glm::vec3 ojo = glm::vec3(glm::inverse(vista)[3]);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		recursos.skybox.DrawSkybox(vista, projection);

		shaderList[0].UseShader();
		const GLuint uniformEyePosition = shaderList[0].GetEyePositionLocation();
		const GLuint uniformTextureOffset = shaderList[0].getOffsetLocation();

		glUniform3f(uniformEyePosition, ojo.x, ojo.y, ojo.z);
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(&luzHolocron, 1);
		shaderList[0].SetSpotLights(nullptr, 0);

		escena.render(shaderList[0], vista, projection);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
