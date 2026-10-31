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
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"

#include "Create.hpp"
#include "Load.hpp"
#include "Input.hpp"

#include "CameraDirector.h"
#include "CameraRig.h"
#include "Scene.h"
#include "SceneNode.h"

static const float pi = 3.14159265358979323846f;
static const float aRad = pi / 180.0f;

std::vector<Shader> shaderList;
Camera camera;

Material materialBrillante;
Material materialOpaco;

DirectionalLight mainLight;
PointLight luzHolocron;

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

static const float PISO_Y = -2.0f;
static const float PISO_ESCALA = 10.0f;

static const glm::vec3 REKSAI_POS(0.0f, PISO_Y + 0.12f, 0.0f);
static const float REKSAI_ESCALA = 2.2f;
static const float REKSAI_GIRO_Y = 180.0f;
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

static glm::mat4 transformacionAvatar()
{
	return glm::translate(glm::mat4(1.0f), REKSAI_POS)
		* glm::rotate(glm::mat4(1.0f), glm::radians(REKSAI_GIRO_Y),
			glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(REKSAI_ESCALA));
}

static void dibujarAvatar(GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	if (!rekSaiCargado)
		return;

	glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
	materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	recursos.plainTexture.UseTexture();
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
		glm::value_ptr(transformacionAvatar()));
	rekSaiModel.RenderModel();
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

int main()
{
	probarJerarquiaEscena();

	Window mainWindow(1366, 768);
	mainWindow.Initialise();

	CreateObjects();
	crearShaders();

	camera = Camera(glm::vec3(0.0f, 6.0f, -14.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), 90.0f, -14.0f, 25.0f, 0.25f);

	LoadTextures();
	LoadLlamas();
	LoadSkyboxCupertin();
	cargarAvatar();

	materialBrillante = Material(4.0f, 256);
	materialOpaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.45f, 0.85f,
		0.35f, 0.75f, 0.40f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
		uniformEyePosition = 0, uniformSpecularIntensity = 0,
		uniformShininess = 0, uniformColor = 0, uniformTextureOffset = 0;

	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(),
		0.1f, 1000.0f);

	GLfloat lastTime = 0.0f;
	bool pausado = false;
	bool pAntes = false;

	while (!mainWindow.getShouldClose())
	{
		float now = (float)glfwGetTime();
		float dt = now - lastTime;
		lastTime = now;
		if (dt > 0.05f) dt = 0.05f;
		if (dt < 0.0f) dt = 0.0f;

		glfwPollEvents();

		bool* teclas = mainWindow.getsKeys();
		bool pAhora = teclas[GLFW_KEY_P];
		if (pAhora && !pAntes)
			pausado = !pausado;
		pAntes = pAhora;

		camera.keyControl(teclas, dt);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		float dtAnim = pausado ? 0.0f : dt;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		recursos.skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE,
			glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE,
			glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition,
			camera.getCameraPosition().x,
			camera.getCameraPosition().y,
			camera.getCameraPosition().z);
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(&luzHolocron, 1);
		shaderList[0].SetSpotLights(nullptr, 0);

		glm::mat4 pisoModelo = glm::translate(glm::mat4(1.0f),
			glm::vec3(0.0f, PISO_Y, 0.0f))
			* glm::scale(glm::mat4(1.0f),
				glm::vec3(PISO_ESCALA, 1.0f, PISO_ESCALA));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
			glm::value_ptr(pisoModelo));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		recursos.pisoTexture.UseTexture();
		materialOpaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		geometria.pisoMesh->RenderMeshModel();

		dibujarAvatar(uniformModel, uniformColor,
			uniformSpecularIntensity, uniformShininess);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
