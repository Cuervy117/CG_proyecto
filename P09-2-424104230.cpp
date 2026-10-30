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

static const glm::vec3 COHETE_INICIO(92.0f, PISO_Y, 30.0f);
static const float COHETE_ESCALA = 0.4f;

static const float REPOSO_DURACION = 1.5f;
static const float DESPEGUE_DURACION = 1.5f;
static const float DESPEGUE_ACELERACION = 8.0f;
static const glm::vec3 ASCENSO_RECTO_FIN(97.0f, 20.0f, 30.0f);
static const float ASCENSO_RECTO_DURACION = 1.2f;
static const float GRAVEDAD = 9.81f;
static const float CAIDA_LIMITE_Y = -30.0f;
static const float REINICIO_DURACION = 1.0f;

static const glm::vec3 HOLOCRON_POS(55.0f, 6.0f, 45.0f);
static const float HOLOCRON_ESCALA = 0.65f;
static const float HOLOCRON_DISTANCIA_MAX = 3.5f;
static const float HOLOCRON_TIEMPO_SEPARACION = 2.5f;
static const float HOLOCRON_TIEMPO_RAPIDO = 2.0f;
static const float HOLOCRON_TIEMPO_REGRESO = 2.5f;
static const float HOLOCRON_TIEMPO_CERRADO = 0.5f;
static const float HOLOCRON_PERIODO = HOLOCRON_TIEMPO_SEPARACION
	+ HOLOCRON_TIEMPO_RAPIDO + HOLOCRON_TIEMPO_REGRESO
	+ HOLOCRON_TIEMPO_CERRADO;
static const float HOLOCRON_DESFASE = 0.08f;
static const float HOLOCRON_GIRO_LENTO = 20.0f * aRad;
static const float HOLOCRON_GIRO_RAPIDO = 130.0f * aRad;
static const float HOLOCRON_GIRO = 20.0f * aRad;

static const glm::vec3 HOLOCRON_OFFSETS[8] = {
	glm::vec3(-5.17947f, 5.2f, -5.05f),
	glm::vec3(5.17947f, 5.2f, -5.05f),
	glm::vec3(-5.17947f, 5.0f, 5.2f),
	glm::vec3(5.17947f, 5.0f, 5.2f),
	glm::vec3(-5.2f, -5.1f, -5.2f),
	glm::vec3(5.1f, -5.1f, -5.2f),
	glm::vec3(-5.17947f, -5.22f, 5.0f),
	glm::vec3(5.2f, -5.2f, 5.0f)
};

static const glm::vec3 HOLOCRON_EJES[8] = {
	glm::vec3(-1.0f, 1.0f, -1.0f),
	glm::vec3(1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3(1.0f, 1.0f, 1.0f),
	glm::vec3(-1.0f, -1.0f, -1.0f),
	glm::vec3(1.0f, -1.0f, -1.0f),
	glm::vec3(-1.0f, -1.0f, 1.0f),
	glm::vec3(1.0f, -1.0f, 1.0f)
};

enum FaseCohete
{
	CoheteReposo = 0,
	CoheteDespegue,
	CoheteAscensoRecto,
	CoheteCaida,
	CoheteReinicio
};

static FaseCohete faseCohete = CoheteReposo;
static float tFaseCohete = 0.0f;
static float tAnim = 0.0f;
static glm::vec3 cohetePos = COHETE_INICIO;
static glm::vec3 coheteForward(0.0f, 1.0f, 0.0f);

static float suavizar01(float t)
{
	t = fminf(fmaxf(t, 0.0f), 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

static glm::vec3 inicioAscensoRecto()
{
	float altura = 0.5f * DESPEGUE_ACELERACION
		* DESPEGUE_DURACION * DESPEGUE_DURACION;

	return glm::vec3(COHETE_INICIO.x, PISO_Y + altura, COHETE_INICIO.z);
}

static glm::vec3 velocidadAscensoRecto()
{
	return (ASCENSO_RECTO_FIN - inicioAscensoRecto())
		/ ASCENSO_RECTO_DURACION;
}

static void crearShaders()
{
	shaderList.reserve(1);

	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);
}

static glm::mat4 orientacionCohete(const glm::vec3& pos, const glm::vec3& forward)
{
	glm::vec3 f = glm::normalize(forward);
	glm::vec3 mundoArriba(0.0f, 1.0f, 0.0f);
	glm::vec3 r;

	if (fabsf(glm::dot(f, mundoArriba)) > 0.999f)
		r = glm::vec3(1.0f, 0.0f, 0.0f);
	else
		r = glm::normalize(glm::cross(f, mundoArriba));

	glm::vec3 u = glm::normalize(glm::cross(r, f));

	glm::mat4 m(1.0f);
	m[0] = glm::vec4(r, 0.0f);
	m[1] = glm::vec4(f, 0.0f);
	m[2] = glm::vec4(u, 0.0f);
	m[3] = glm::vec4(pos, 1.0f);
	return m;
}

static void reiniciarCohete()
{
	faseCohete = CoheteReposo;
	tFaseCohete = 0.0f;
	cohetePos = COHETE_INICIO;
	coheteForward = glm::vec3(0.0f, 1.0f, 0.0f);
}

static void actualizarCohete(float dt)
{
	if (faseCohete == CoheteReposo)
	{
		cohetePos = COHETE_INICIO + glm::vec3(
			0.03f * sinf(40.0f * tAnim),
			0.0f,
			0.03f * cosf(37.0f * tAnim));
		coheteForward = glm::vec3(0.0f, 1.0f, 0.0f);

		if (tFaseCohete >= REPOSO_DURACION)
		{
			tFaseCohete = 0.0f;
			faseCohete = CoheteDespegue;
		}
	}
	else if (faseCohete == CoheteDespegue)
	{
		float t = tFaseCohete;
		float altura = 0.5f * DESPEGUE_ACELERACION * t * t;

		cohetePos = glm::vec3(COHETE_INICIO.x,
			PISO_Y + altura,
			COHETE_INICIO.z);
		coheteForward = glm::vec3(0.0f, 1.0f, 0.0f);

		if (tFaseCohete >= DESPEGUE_DURACION)
		{
			tFaseCohete = 0.0f;
			faseCohete = CoheteAscensoRecto;
		}
	}
	else if (faseCohete == CoheteAscensoRecto)
	{
		float t = tFaseCohete;
		glm::vec3 inicio = inicioAscensoRecto();
		glm::vec3 velocidad = velocidadAscensoRecto();

		cohetePos = inicio + velocidad * t;
		coheteForward = glm::normalize(glm::mix(coheteForward,
			glm::normalize(velocidad), fminf(1.0f, 10.0f * dt)));

		if (tFaseCohete >= ASCENSO_RECTO_DURACION)
		{
			tFaseCohete = 0.0f;
			faseCohete = CoheteCaida;
		}
	}
	else if (faseCohete == CoheteCaida)
	{
		float t = tFaseCohete;
		glm::vec3 velocidadInicial = velocidadAscensoRecto();
		glm::vec3 aceleracion(0.0f, -GRAVEDAD, 0.0f);

		cohetePos = ASCENSO_RECTO_FIN + velocidadInicial * t
			+ 0.5f * aceleracion * t * t;

		glm::vec3 velocidad = velocidadInicial + aceleracion * t;
		glm::vec3 objetivo = glm::normalize(velocidad);
		coheteForward = glm::normalize(glm::mix(coheteForward, objetivo,
			fminf(1.0f, 8.0f * dt)));

		if (cohetePos.y < CAIDA_LIMITE_Y)
		{
			tFaseCohete = 0.0f;
			faseCohete = CoheteReinicio;
		}
	}
	else
	{
		if (tFaseCohete >= REINICIO_DURACION)
			reiniciarCohete();
	}

	tFaseCohete += dt;
}

static glm::mat4 matrizHolocron()
{
	float flotacion = 0.35f * sinf(1.2f * tAnim);
	glm::vec3 pos = HOLOCRON_POS + glm::vec3(0.0f, flotacion, 0.0f);

	return glm::translate(glm::mat4(1.0f), pos)
		* glm::rotate(glm::mat4(1.0f), HOLOCRON_GIRO * tAnim,
			glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(HOLOCRON_ESCALA));
}

static void dibujarHolocron(GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	glm::mat4 base = matrizHolocron();
	float pulso = 0.85f + 0.15f * sinf(2.4f * tAnim);

	materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

	glUniform3f(uniformColor, 0.12f * pulso, 0.55f * pulso, 1.15f * pulso);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(base));
	recursos.holocronBase.RenderModel();

	glUniform3f(uniformColor, 1.0f * pulso, 0.82f * pulso, 0.22f * pulso);

	for (int i = 0; i < 8; i++)
	{
		float fase = fmodf(tAnim + (float)i * HOLOCRON_DESFASE,
			HOLOCRON_PERIODO);
		float separacion;
		float angulo;
		float finSeparacion = HOLOCRON_TIEMPO_SEPARACION;
		float finRapido = finSeparacion + HOLOCRON_TIEMPO_RAPIDO;
		float finRegreso = finRapido + HOLOCRON_TIEMPO_REGRESO;

		if (fase < finSeparacion)
		{
			float p = fase / HOLOCRON_TIEMPO_SEPARACION;
			separacion = HOLOCRON_DISTANCIA_MAX * suavizar01(p);
			angulo = HOLOCRON_GIRO_LENTO * fase;
		}
		else if (fase < finRapido)
		{
			separacion = HOLOCRON_DISTANCIA_MAX;
			angulo = HOLOCRON_GIRO_LENTO * finSeparacion
				+ HOLOCRON_GIRO_RAPIDO * (fase - finSeparacion);
		}
		else if (fase < finRegreso)
		{
			float p = (fase - finRapido) / HOLOCRON_TIEMPO_REGRESO;
			separacion = HOLOCRON_DISTANCIA_MAX * (1.0f - suavizar01(p));
			angulo = HOLOCRON_GIRO_LENTO * finSeparacion
				+ HOLOCRON_GIRO_RAPIDO * HOLOCRON_TIEMPO_RAPIDO
				+ HOLOCRON_GIRO_LENTO * (fase - finRapido);
		}
		else
		{
			separacion = 0.0f;
			angulo = 2.0f * pi;
		}

		glm::vec3 eje = glm::normalize(HOLOCRON_EJES[i]);
		glm::mat4 esquina = base
			* glm::translate(glm::mat4(1.0f),
				HOLOCRON_OFFSETS[i] + eje * separacion)
			* glm::rotate(glm::mat4(1.0f), angulo, eje);

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(esquina));
		recursos.holocronEsquinas[i].RenderModel();
	}
}

static void dibujarCohete(GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	if (faseCohete == CoheteReinicio)
		return;

	glm::mat4 coheteModelo = orientacionCohete(cohetePos, coheteForward)
		* glm::scale(glm::mat4(1.0f), glm::vec3(COHETE_ESCALA));

	glUniform3f(uniformColor, 1.1f, 1.1f, 1.1f);
	materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(coheteModelo));
	recursos.coheteModel.RenderModel();
}

static void dibujarLlama(GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	if (faseCohete != CoheteDespegue && faseCohete != CoheteAscensoRecto)
		return;

	float pulso = 1.0f + 0.18f * sinf(24.0f * tAnim);
	glm::vec3 flamaPos = cohetePos - coheteForward * 2.2f;
	glm::vec3 normal = glm::normalize(camera.getCameraPosition() - flamaPos);
	glm::vec3 ejeY = -coheteForward;
	glm::vec3 ejeX = glm::normalize(glm::cross(ejeY, normal));
	ejeY = glm::normalize(glm::cross(normal, ejeX));

	glm::mat4 billboard(1.0f);
	billboard[0] = glm::vec4(ejeX, 0.0f);
	billboard[1] = glm::vec4(ejeY, 0.0f);
	billboard[2] = glm::vec4(normal, 0.0f);
	billboard[3] = glm::vec4(flamaPos, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	recursos.llamaTexture.UseTexture();
	materialOpaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

	glm::mat4 modeloExterior = billboard
		* glm::scale(glm::mat4(1.0f), glm::vec3(1.9f, 5.8f * pulso, 1.0f));
	glUniform3f(uniformColor, 1.4f, 0.55f, 0.12f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
		glm::value_ptr(modeloExterior));
	geometria.llamaMesh->RenderMeshModel();

	glm::mat4 modeloInterior = billboard
		* glm::scale(glm::mat4(1.0f), glm::vec3(0.9f, 3.0f * pulso, 1.0f));
	glUniform3f(uniformColor, 1.9f, 1.0f, 0.35f);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE,
		glm::value_ptr(modeloInterior));
	geometria.llamaMesh->RenderMeshModel();

	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
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
	CrearLlamas();
	crearShaders();

	camera = Camera(glm::vec3(58.0f, 22.0f, -30.0f),
		glm::vec3(0.0f, 1.0f, 0.0f), 68.0f, -4.0f, 25.0f, 0.25f);

	LoadTextures();
	LoadCohete();
	LoadHolocronP05();
	LoadLlamas();
	LoadSkyboxCupertin();

	materialBrillante = Material(4.0f, 256);
	materialOpaco = Material(0.3f, 4);

	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.45f, 0.85f,
		0.35f, 0.75f, 0.40f);

	luzHolocron = PointLight(0.25f, 0.55f, 1.0f,
		0.0f, 1.2f,
		HOLOCRON_POS.x, HOLOCRON_POS.y, HOLOCRON_POS.z,
		1.0f, 0.09f, 0.032f);

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0,
		uniformEyePosition = 0, uniformSpecularIntensity = 0,
		uniformShininess = 0, uniformColor = 0, uniformTextureOffset = 0;

	glm::mat4 projection = glm::perspective(45.0f,
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

		tAnim += dtAnim;
		actualizarCohete(dtAnim);

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

		float flotacion = 0.35f * sinf(1.2f * tAnim);
		luzHolocron.SetPos(HOLOCRON_POS + glm::vec3(0.0f, flotacion, 0.0f));

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

		dibujarHolocron(uniformModel, uniformColor,
			uniformSpecularIntensity, uniformShininess);

		dibujarCohete(uniformModel, uniformColor,
			uniformSpecularIntensity, uniformShininess);

		dibujarLlama(uniformModel, uniformColor,
			uniformSpecularIntensity, uniformShininess);

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
