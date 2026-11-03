#define STB_IMAGE_IMPLEMENTATION

#include "Superficie.h"
#include "Espacio.h"
#include "Oceano.h"
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
#include "src/game/avatar/Avatar.h"
#include "src/ui/Hud.h"

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

static const glm::vec3 REKSAI_POS(0.0f, PISO_Y + 0.12f, 0.0f);
static const float REKSAI_ESCALA = 2.2f;

static void crearShaders()
{
	shaderList.reserve(1);

	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);
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

	printf("A4.1 Chase RekSai: %s (avatar %.3f, %.3f, %.3f; camara %.3f, %.3f, %.3f)\n",
		ok ? "OK" : "FALLO",
		base.x, base.y, base.z,
		rig.posicion().x, rig.posicion().y, rig.posicion().z);
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

	const bool ok = planetaOk && mismoModo && mismoRig && carreraOk && vueloOk &&
		observacionOk;

	printf("A4.4 CameraDirector: %s (rigs Planeta/Carrera/Vuelo/Observacion)\n",
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
	cgame::InputCapture inputCapture(mainWindow);
	cgame::Hud hud;
	if (!hud.initialize())
		printf("No se pudo iniciar el HUD de la demo.\n");

	CreateObjects();
	crearShaders();

	LoadTextures();
	LoadLlamas();
	LoadSkyboxCupertin();
	LoadNave();
	InicializarSuperficies();
	InicializarOceano();
	InicializarEspacio();

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.45f, 0.85f,
		0.35f, 0.75f, 0.40f);

	Scene escena;
	escena.setFallbackTexture(&recursos.plainTexture);

	cgame::Avatar rekSaiAvatar;
	if (!rekSaiAvatar.attach(escena.root, "Models/reksai.obj",
		"Textures/reksai.png", REKSAI_POS, REKSAI_ESCALA))
	{
		printf("No se pudo cargar el avatar articulado de Rek'Sai.\n");
		return 1;
	}
	SceneNode* nodoAvatar = rekSaiAvatar.root();

	SceneNode* nodoNave = escena.root.add(std::make_unique<SceneNode>("nave_prueba"));
	nodoNave->position = glm::vec3(18.0f, 8.0f, 0.0f);
	nodoNave->rotationEuler.y = glm::radians(30.0f);
	nodoNave->rotationEuler.x = glm::radians(-10.0f);
	nodoNave->scale = glm::vec3(0.5f);
	if (recursos.naveModel.getMeshCount() > 0)
	{
		nodoNave->setModel(&recursos.naveModel);
	}
	nodoNave->visible = false;

	escena.updateWorld();
	probarChaseRekSai(*nodoAvatar);
	probarFlightNave(*nodoNave);
	probarOrbitNave(*nodoNave);

	CameraDirector director;
	director.setMode(CameraMode::Planeta);
	enum class Zona { Selva, Roca, Oceano, Pista };
	Zona zona = Zona::Selva;
	glm::vec3 posicionAvatar = PosicionInicio(SUPERFICIE_SELVATICA);
	bool enNave = false;
	int tramo = 0;
	float recorrido = DistanciaMeta(), carril = 0.0f;
	glm::vec2 sobreOla(0.0f);
	SceneNode* rival = escena.root.add(std::make_unique<SceneNode>("nave_rival"));
	rival->setModel(&recursos.naveModel);
	rival->scale = nodoNave->scale;
	auto entrar = [&](Zona destino) {
		zona = destino;
		printf("Zona: %s\n", zona == Zona::Selva ? "Selva" : zona == Zona::Roca ? "Roca" : zona == Zona::Oceano ? "Oceano" : "Pista");
		fflush(stdout);
		enNave = zona == Zona::Pista || zona == Zona::Oceano;
		if (zona == Zona::Selva || zona == Zona::Roca)
			posicionAvatar = PosicionInicio(zona == Zona::Selva ? SUPERFICIE_SELVATICA : SUPERFICIE_ROCOSA);
		if (zona == Zona::Oceano) {
			float yaw;
			InicioOceano(sobreOla.x, sobreOla.y, yaw);
		}
		recorrido = tramo == 0 ? DistanciaMeta() : 0.0f;
		carril = 0.0f;
		nodoNave->position = posicionAvatar + glm::vec3(3, 0.7f, 0);
	};
	entrar(zona);
	const float velocidadAvatar = 2.5f;

	const float aspecto = (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight();
	const glm::mat4 projection = director.projection(aspecto);

	Shader& shader = shaderList[0];
	shader.UseShader();
	GLint programa;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programa);
	const UniformsEspacio espacio = {shader.GetModelLocation(), shader.getColorLocation(),
		shader.GetSpecularIntensityLocation(), shader.GetShininessLocation(),
		static_cast<GLuint>(glGetUniformLocation(programa, "emision"))};
	const UniformsSuperficie terreno = {espacio.model, espacio.color, shader.getOffsetLocation(),
		espacio.especular, espacio.brillo, espacio.emision};
	const GLint niebla = glGetUniformLocation(programa, "niebla");
	GLfloat lastTime = (GLfloat)glfwGetTime();

	while (!mainWindow.getShouldClose())
	{
		float now = (float)glfwGetTime();
		float dt = now - lastTime;
		lastTime = now;
		if (dt > 0.05f) dt = 0.05f;
		if (dt < 0.0f) dt = 0.0f;

		glfwPollEvents();

		const cgame::InputFrame& input = inputCapture.update(dt);
		const cgame::ControlFrame controls = cgame::mapControls(input);
		if (input.wasPressed(GLFW_KEY_F5)) entrar(Zona::Selva);
		if (input.wasPressed(GLFW_KEY_F6)) entrar(Zona::Roca);
		if (input.wasPressed(GLFW_KEY_F7)) entrar(Zona::Oceano);
		if (input.wasPressed(GLFW_KEY_F8)) { tramo = 0; entrar(Zona::Pista); }
		glm::vec3 avatarDirection(controls.movementXZ.x, 0.0f,
			controls.movementXZ.y);
		const float directionLength = glm::length(avatarDirection);
		if (directionLength > 1.0f)
			avatarDirection /= directionLength;
		const TipoSuperficie superficie = zona == Zona::Selva ? SUPERFICIE_SELVATICA : SUPERFICIE_ROCOSA;
		glm::vec3 frente(0,0,1), arriba(0,1,0);
		if (zona == Zona::Pista) {
			recorrido = glm::max(0.0f, recorrido + controls.movementXZ.y * 55.0f * dt);
			carril = glm::clamp(carril + controls.movementXZ.x * 8.0f * dt, -3.0f, 3.0f);
			PuntoEnPista(tramo, recorrido, carril, 1.0f, posicionAvatar, frente, arriba);
			glm::vec3 rumbo;
			PuntoEnPista(tramo, fmodf(now * 18.0f, LargoTramo(tramo)), -2, 1, rival->position, rumbo, arriba);
			rival->rotationEuler.y = atan2f(rumbo.x, rumbo.z);
			if (recorrido >= LargoTramo(tramo))
				entrar(tramo == 0 ? Zona::Roca : tramo == 1 ? Zona::Selva : Zona::Oceano);
		} else if (zona == Zona::Oceano) {
			sobreOla += controls.movementXZ * (24.0f * dt);
			sobreOla = LimitarOceano(sobreOla.x, sobreOla.y);
			PuntoOla(sobreOla.x, sobreOla.y, 1.0f, posicionAvatar, frente, arriba);
			if (LlegoALaCresta(posicionAvatar)) { tramo = 0; entrar(Zona::Pista); }
		} else {
			if (controls.ascendOrInteractPressed && (enNave || glm::distance(posicionAvatar, nodoNave->position) < 5.0f))
				{
				enNave = !enNave;
				printf("Jugador: %s\n", enNave ? "a bordo" : "a pie");
				fflush(stdout);
			}
			posicionAvatar = LimitarPosicion(superficie, posicionAvatar + avatarDirection * (enNave ? 18.0f : velocidadAvatar) * dt);
			if (directionLength > 0.001f) frente = avatarDirection;
			if (CruzandoPortal(superficie, posicionAvatar)) { tramo = zona == Zona::Roca ? 1 : 2; entrar(Zona::Pista); }
		}
		nodoAvatar->visible = !enNave;
		nodoNave->visible = true;
		rival->visible = zona == Zona::Pista;
		if (enNave) {
			nodoNave->position = posicionAvatar + arriba * 0.7f;
			nodoNave->rotationEuler.y = atan2f(frente.x, frente.z);
		}
		rekSaiAvatar.setPosition(posicionAvatar);
		rekSaiAvatar.update(dt, avatarDirection,
			directionLength > 0.001f ? velocidadAvatar : 0.0f);

		escena.updateWorld();
		director.update(dt, enNave ? *nodoNave : *nodoAvatar);
		const glm::mat4 vista = director.view();
		const glm::vec3 ojo = glm::vec3(glm::inverse(vista)[3]);
		const char* accion = directionLength > 0.001f ? "CAMINANDO" : "WASD MOVER";

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

		const bool enSuperficie = zona == Zona::Selva || zona == Zona::Roca;
		const TipoSuperficie actual = zona == Zona::Selva ? SUPERFICIE_SELVATICA : SUPERFICIE_ROCOSA;
		const float dia = 0.5f + 0.5f * cosf(now * 0.05236f);
		if (enSuperficie) {
			const auto ambiente = ObtenerAmbiente(actual, dia);
			glUniform4f(niebla, ambiente.colorNiebla.x, ambiente.colorNiebla.y, ambiente.colorNiebla.z, ambiente.densidadNiebla);
		} else glUniform4f(niebla, 0, 0, 0, 0);
		glUniform3f(espacio.emision, 0, 0, 0);
		escena.render(shader, vista, projection);
		glm::vec3 mirada = -glm::vec3(glm::inverse(vista)[2]);
		if (enSuperficie) DibujarSuperficie(actual, ojo, mirada, dia, now, terreno);
		else if (zona == Zona::Oceano) DibujarOceano(ojo, espacio);
		else {
			DibujarPista(now, espacio);
			DibujarBase(now, espacio);
			DibujarCinturon(ojo, mirada, now, espacio);
			DibujarPlanetaOceano(now, espacio);
		}
		hud.draw(static_cast<int>(mainWindow.getBufferWidth()),
			static_cast<int>(mainWindow.getBufferHeight()),
			zona == Zona::Selva ? "SELVA" : zona == Zona::Roca ? "ROCA" : zona == Zona::Oceano ? "OCEANO" : "PISTA", "F5-F8 ZONA", enNave ? "WASD NAVE" : accion);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
