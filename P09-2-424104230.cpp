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
#include "FollowXZ.h"
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
	probarPivotes();

	Window mainWindow(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	crearShaders();

	LoadTextures();
	LoadLlamas();
	LoadSkyboxCupertin();
	cargarAvatar();

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

	escena.updateWorld();

	CameraDirector director;
	director.setRig(std::make_unique<FollowXZ>());

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

		const float t = (float)glfwGetTime();
		nodoAvatar->position.x = 4.0f * sinf(t * 0.5f);
		nodoAvatar->position.z = 4.0f * cosf(t * 0.5f);
		nodoAvatar->rotationEuler.y = glm::radians(REKSAI_GIRO_Y) + 0.5f * sinf(t * 0.25f);
		pivotePrueba->rotationEuler.y = 1.2f * sinf(t * 1.5f);

		escena.updateWorld();
		director.update(dt, *nodoAvatar);

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
