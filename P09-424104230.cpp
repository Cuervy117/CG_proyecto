#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <math.h>
#include <vector>

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
#include "Model.h"
#include "Skybox.h"

#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include "Create.hpp"
#include "Load.hpp"

Window mainWindow;
std::vector<Shader> shaderList;

Camera camera;

Material materialBrillante;
Material materialOpaco;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

DirectionalLight mainLight;
PointLight pointLights[2];

static const char* vShader = "shaders/shader_light.vert";
static const char* fShader = "shaders/shader_light.frag";

static const float pistaLado = 34.0f;
static const float pistaRadio = 10.0f;
static const float pistaRectaInicio = pistaLado - pistaRadio;
static const float pistaRecta = 2.0f * pistaRectaInicio;
static const float pistaAlturaInicio = 0.0f;
static const float pistaAlturaSubida = 12.0f;
static const float pistaAlturaSuperior = -12.0f;
static const float pistaVelocidad = 12.0f;

static const float pi = 3.14159265359f;
static const float cuartoPi = 1.57079632679f;
static const float dosPi = 6.28318530718f;
static const float cuartoLongitud = pistaRadio * cuartoPi;

static const float guiaAncho = 0.35f;
static const float guiaLargo = 1.8f;
static const float guiaPaso = 3.0f;
static const float guiaSateliteAncho = 0.28f;
static const float guiaSateliteLargo = 1.5f;

static const float satRadioX = 16.0f;
static const float satRadioZ = 12.0f;
static const float satAltura = 26.0f;
static const float satOndulacion = 2.0f;
static const float satOmega = 0.35f;

enum TramoPista
{
	RectaInferior = 0,
	CuartoInferiorDerecho,
	RectaDerecha,
	CuartoSuperiorDerecho,
	RectaSuperior,
	CuartoSuperiorIzquierdo,
	RectaFinal,
	CuartoInferiorIzquierdo,
	NumTramos
};

struct PuntoGuia
{
	glm::vec3 pos;
	glm::vec3 forward;
};

static std::vector<PuntoGuia> guiaPuntos;
static std::vector<PuntoGuia> guiaSatelite;

static int tramoActual = RectaInferior;
static float offsetTramo = 0.0f;
static glm::vec3 naveForward(1.0f, 0.0f, 0.0f);

static int estadoSatelite = 0;
static float thetaSatelite = 0.0f;
static float rotPanelDer = 0.0f;
static float rotPanelIzq = 0.0f;
static float rotacionRotor = 0.0f;

static float tAnim = 0.0f;

static void createShaders()
{
	shaderList.reserve(1);

	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);
}

static float longitudTramo(int tramo)
{
	if (tramo == RectaInferior || tramo == RectaDerecha ||
		tramo == RectaSuperior || tramo == RectaFinal)
		return pistaRecta;

	return cuartoLongitud;
}

static void evaluarTramo(int tramo, float s, glm::vec3& pos, glm::vec3& tangente)
{
	switch (tramo)
	{
	case RectaInferior:
		pos = glm::vec3(-pistaRectaInicio, pistaAlturaInicio, -pistaLado)
			+ glm::vec3(s, 0.0f, 0.0f);
		tangente = glm::vec3(1.0f, 0.0f, 0.0f);
		break;

	case CuartoInferiorDerecho:
	{
		float theta = -cuartoPi + s / pistaRadio;
		glm::vec3 centro(pistaRectaInicio, pistaAlturaInicio, -pistaRectaInicio);
		pos = centro + glm::vec3(pistaRadio * cosf(theta), 0.0f, pistaRadio * sinf(theta));
		tangente = glm::vec3(-sinf(theta), 0.0f, cosf(theta));
		break;
	}

	case RectaDerecha:
	{
		float t = s / pistaRecta;
		float altura = pistaAlturaInicio + (pistaAlturaSubida - pistaAlturaInicio) * t;
		pos = glm::vec3(pistaLado, altura, -pistaRectaInicio) + glm::vec3(0.0f, 0.0f, s);
		tangente = glm::normalize(glm::vec3(0.0f,
			(pistaAlturaSubida - pistaAlturaInicio) / pistaRecta, 1.0f));
		break;
	}

	case CuartoSuperiorDerecho:
	{
		float theta = s / pistaRadio;
		float t = s / cuartoLongitud;
		float altura = pistaAlturaSubida + (pistaAlturaSuperior - pistaAlturaSubida) * t;
		glm::vec3 centro(pistaRectaInicio, pistaAlturaSuperior, pistaRectaInicio);
		pos = glm::vec3(centro.x + pistaRadio * cosf(theta), altura,
			centro.z + pistaRadio * sinf(theta));
		float pendiente = (pistaAlturaSuperior - pistaAlturaSubida) / cuartoLongitud;
		tangente = glm::normalize(glm::vec3(-sinf(theta), pendiente, cosf(theta)));
		break;
	}

	case RectaSuperior:
		pos = glm::vec3(pistaRectaInicio, pistaAlturaSuperior, pistaLado)
			+ glm::vec3(-s, 0.0f, 0.0f);
		tangente = glm::vec3(-1.0f, 0.0f, 0.0f);
		break;

	case CuartoSuperiorIzquierdo:
	{
		float theta = cuartoPi + s / pistaRadio;
		glm::vec3 centro(-pistaRectaInicio, pistaAlturaSuperior, pistaRectaInicio);
		pos = centro + glm::vec3(pistaRadio * cosf(theta), 0.0f, pistaRadio * sinf(theta));
		tangente = glm::vec3(-sinf(theta), 0.0f, cosf(theta));
		break;
	}

	case RectaFinal:
	{
		float mitad = pistaRecta * 0.5f;
		float altura = pistaAlturaSuperior;
		float pendiente = 0.0f;

		if (s > mitad)
		{
			altura = pistaAlturaSuperior
				+ (pistaAlturaInicio - pistaAlturaSuperior) * (s - mitad) / mitad;
			pendiente = (pistaAlturaInicio - pistaAlturaSuperior) / mitad;
		}

		pos = glm::vec3(-pistaLado, altura, pistaRectaInicio) + glm::vec3(0.0f, 0.0f, -s);
		tangente = glm::normalize(glm::vec3(0.0f, pendiente, -1.0f));
		break;
	}

	case CuartoInferiorIzquierdo:
	{
		float theta = pi + s / pistaRadio;
		glm::vec3 centro(-pistaRectaInicio, pistaAlturaInicio, -pistaRectaInicio);
		pos = centro + glm::vec3(pistaRadio * cosf(theta), 0.0f, pistaRadio * sinf(theta));
		tangente = glm::vec3(-sinf(theta), 0.0f, cosf(theta));
		break;
	}

	default:
		pos = glm::vec3(-pistaRectaInicio, pistaAlturaInicio, -pistaLado);
		tangente = glm::vec3(1.0f, 0.0f, 0.0f);
		break;
	}
}

static void construirGuia()
{
	guiaPuntos.clear();

	for (int tramo = 0; tramo < NumTramos; tramo++)
	{
		float longitud = longitudTramo(tramo);

		for (float s = 0.0f; s < longitud; s += guiaPaso)
		{
			glm::vec3 pos;
			glm::vec3 tangente;
			evaluarTramo(tramo, s, pos, tangente);
			guiaPuntos.push_back(PuntoGuia{ pos, glm::normalize(tangente) });
		}
	}
}

static glm::mat4 matrizOrientacion(const glm::vec3& pos, const glm::vec3& forward,
	const glm::vec3& upRef)
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

static void dibujarGuia(const std::vector<PuntoGuia>& puntos, const glm::vec3& color,
	float ancho, float largo, GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	recursos.plainTexture.UseTexture();
	materialOpaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
	glUniform3f(uniformColor, color.x, color.y, color.z);

	glm::vec3 mundoArriba(0.0f, 1.0f, 0.0f);

	for (size_t i = 0; i < puntos.size(); i++)
	{
		glm::vec3 forward = puntos[i].forward;
		glm::vec3 right = glm::normalize(glm::cross(forward, mundoArriba));
		glm::vec3 up = glm::normalize(glm::cross(right, forward));

		glm::mat4 m(1.0f);
		m[0] = glm::vec4(right, 0.0f);
		m[1] = glm::vec4(forward, 0.0f);
		m[2] = glm::vec4(up, 0.0f);
		m[3] = glm::vec4(puntos[i].pos + up * 0.05f, 1.0f);
		m = m * glm::scale(glm::mat4(1.0f), glm::vec3(ancho, largo, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
		geometria.quadMesh->RenderMeshModel();
	}

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

static void animarNave(GLfloat dt)
{
	offsetTramo += pistaVelocidad * dt;

	while (offsetTramo >= longitudTramo(tramoActual))
	{
		offsetTramo -= longitudTramo(tramoActual);
		tramoActual = (tramoActual + 1) % NumTramos;
	}
}

static glm::vec3 posicionSatelite(int estado, float theta)
{
	float lado = (estado == 0) ? -1.0f : 1.0f;

	return glm::vec3(
		lado * satRadioX * (1.0f - cosf(theta)),
		satAltura + satOndulacion * sinf(2.0f * theta),
		satRadioZ * sinf(theta));
}

static glm::vec3 tangenteSatelite(int estado, float theta)
{
	float lado = (estado == 0) ? -1.0f : 1.0f;

	return glm::normalize(glm::vec3(
		lado * satRadioX * sinf(theta),
		2.0f * satOndulacion * cosf(2.0f * theta),
		satRadioZ * cosf(theta)));
}

static void construirGuiaSatelite()
{
	guiaSatelite.clear();

	for (int estado = 0; estado < 2; estado++)
	{
		float lado = (estado == 0) ? -1.0f : 1.0f;
		float theta = 0.0f;

		while (theta < dosPi)
		{
			glm::vec3 pos = posicionSatelite(estado, theta);
			glm::vec3 tangente = tangenteSatelite(estado, theta);
			guiaSatelite.push_back(PuntoGuia{ pos, glm::normalize(tangente) });

			float dx = lado * satRadioX * sinf(theta);
			float dy = 2.0f * satOndulacion * cosf(2.0f * theta);
			float dz = satRadioZ * cosf(theta);
			float rapidez = sqrtf(dx * dx + dy * dy + dz * dz);
			theta += guiaPaso / fmaxf(rapidez, 0.0001f);
		}
	}
}

static void animarSatelite(GLfloat dt)
{
	thetaSatelite += satOmega * dt;

	while (thetaSatelite >= dosPi)
	{
		thetaSatelite -= dosPi;
		estadoSatelite = 1 - estadoSatelite;
	}

	rotPanelDer = 25.0f * sinf(0.9f * tAnim);
	rotPanelIzq = -25.0f * sinf(0.9f * tAnim);
	rotacionRotor = fmodf(rotacionRotor + 180.0f * dt, 360.0f);
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CrearQuad();
	CrearLlamas();
	createShaders();

	camera = Camera(glm::vec3(0.0f, 30.0f, -85.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		90.0f, -8.0f, 20.0f, 0.25f);

	LoadNave();
	LoadLlamas();
	LoadPlaneta();
	LoadSatelite();
	LoadSkyboxP1();

	recursos.plainTexture = Texture("Textures/plain.png");
	recursos.plainTexture.LoadTextureA();

	construirGuia();
	construirGuiaSatelite();

	materialBrillante = Material(4.0f, 256);
	materialOpaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.30f, 0.70f,
		0.25f, 0.60f, 0.40f);

	pointLights[0] = PointLight(0.2f, 0.45f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);

	pointLights[1] = PointLight(0.2f, 0.45f, 1.0f,
		0.0f, 1.5f,
		0.0f, 0.0f, 0.0f,
		1.0f, 0.09f, 0.032f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformColor = 0,
		uniformTextureOffset = 0;

	glm::mat4 projection = glm::perspective(45.0f,
		(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::vec3 mundoArriba(0.0f, 1.0f, 0.0f);

	lastTime = (GLfloat)glfwGetTime();

	while (!mainWindow.getShouldClose())
	{
		GLfloat now = (GLfloat)glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		if (deltaTime > 0.05f)
			deltaTime = 0.05f;
		if (deltaTime < 0.0f)
			deltaTime = 0.0f;

		glfwPollEvents();

		bool* teclas = mainWindow.getsKeys();

		camera.keyControl(teclas, deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		tAnim += deltaTime;
		animarNave(deltaTime);
		animarSatelite(deltaTime);

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

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x,
			camera.getCameraPosition().y, camera.getCameraPosition().z);
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);

		glm::vec3 posNave;
		glm::vec3 tanNave;
		evaluarTramo(tramoActual, offsetTramo, posNave, tanNave);

		glm::vec3 objetivo = glm::normalize(tanNave);
		naveForward = glm::normalize(glm::mix(naveForward, objetivo,
			fminf(1.0f, 8.0f * deltaTime)));

		glm::mat4 naveModelo = matrizOrientacion(
			posNave + glm::vec3(0.0f, 1.3f, 0.0f), naveForward, mundoArriba);

		glm::vec3 motor1Pos = glm::vec3(naveModelo * glm::vec4(0.5f, 0.10f, -2.72f, 1.0f));
		glm::vec3 motor2Pos = glm::vec3(naveModelo * glm::vec4(-0.5f, 0.10f, -2.72f, 1.0f));

		pointLights[0].SetPos(motor1Pos);
		pointLights[1].SetPos(motor2Pos);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, 2);
		shaderList[0].SetSpotLights(nullptr, 0);

		glm::mat4 planetaModelo = glm::scale(glm::mat4(1.0f), glm::vec3(7.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(planetaModelo));
		glUniform3f(uniformColor, 0.62f, 0.52f, 0.38f);
		recursos.plainTexture.UseTexture();
		materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		recursos.planetaModel.RenderModel();

		dibujarGuia(guiaPuntos, glm::vec3(0.35f, 0.85f, 1.0f), guiaAncho, guiaLargo,
			uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);
		dibujarGuia(guiaSatelite, glm::vec3(1.0f, 0.35f, 0.85f),
			guiaSateliteAncho, guiaSateliteLargo,
			uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(naveModelo));
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		recursos.plainTexture.UseTexture();
		materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		recursos.naveModel.RenderModel();

		glm::vec3 satPos = posicionSatelite(estadoSatelite, thetaSatelite);
		glm::vec3 satTan = tangenteSatelite(estadoSatelite, thetaSatelite);
		float satYaw = atan2f(satTan.x, satTan.z);
		glm::mat4 satBase = glm::translate(glm::mat4(1.0f), satPos)
			* glm::rotate(glm::mat4(1.0f), satYaw, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

		glm::mat4 panelDer = satBase
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.2f, 4.0f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotPanelDer),
				glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 panelIzq = satBase
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.2f, 4.0f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotPanelIzq),
				glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 matrizRotor = satBase
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f))
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotacionRotor),
				glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec3 dirSat = glm::normalize(camera.getCameraPosition() - satPos);
		DirectionalLight luzSatelite(1.0f, 1.0f, 1.0f,
			0.8f, 0.9f,
			dirSat.x, dirSat.y, dirSat.z);
		shaderList[0].SetDirectionalLight(&luzSatelite);

		glUniform3f(uniformColor, 1.2f, 1.2f, 1.2f);
		recursos.plainTexture.UseTexture();
		materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(satBase));
		recursos.satelite_base.RenderModel();

		glUniform3f(uniformColor, 0.15f, 0.4f, 1.2f);
		materialOpaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(panelDer));
		recursos.satelite_panel_der.RenderModel();
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(panelIzq));
		recursos.satelite_panel_izq.RenderModel();

		glUniform3f(uniformColor, 1.2f, 1.2f, 1.2f);
		materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizRotor));
		recursos.satelite_aspa.RenderModel();

		shaderList[0].SetDirectionalLight(&mainLight);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glDepthMask(GL_FALSE);

		recursos.llamaTexture.UseTexture();
		materialOpaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		float pulso = 1.0f + 0.15f * sinf(20.0f * tAnim);

		glm::mat4 llama1 = naveModelo
			* glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.10f, -2.92f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(pulso));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llama1));
		geometria.llamaMesh->RenderMeshModel();

		glm::mat4 llama2 = naveModelo
			* glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.10f, -2.92f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(pulso));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llama2));
		geometria.llamaMesh->RenderMeshModel();

		glDepthMask(GL_TRUE);
		glDisable(GL_BLEND);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
