#define STB_IMAGE_IMPLEMENTATION

#include <cmath>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

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
#include "Superficie.h"   // src/game/world: selva y desierto de los planetas
#include "Espacio.h"      // src/game/world: pista de carreras, cinturon de asteroides y salidas de los portales
#include "Reloj.h"        // src/game/world: reloj en pantalla
#include "Jugador.h"      // src/game/world: avatar en tercera persona (Paul / miNave)
#include "Oceano.h"       // src/game/world: planeta oceano (por fuera y por dentro, con la ola gigante)

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

// Rutas reales dentro de Models/: cada modelo vive en su propia subcarpeta junto con su .mtl
// y su textura. Antes apuntaban directo a Models/ y a un nombre que no existe
// (planetaRocoso.obj), por eso estos modelos nunca se cargaban.
static const char* RUTA_PLANETA_SELVATICO = "Models/planetaSelva/planetaSelvatico.obj";
static const char* RUTA_PLANETA_ROCOSO = "Models/planetaRoca/planetaRoca.obj";
static const char* RUTA_CAPSULA_CUERPO = "Models/capsula/capsulaCuerpo.obj";
static const char* RUTA_CAPSULA_PUERTA = "Models/capsula/capsulaPuerta.obj";
static const char* RUTA_CAPSULA_CUERPO_ANTERIOR = "Models/Capsula/capsula_cuerpo.obj";   // nombres viejos, por si acaso
static const char* RUTA_CAPSULA_PUERTA_ANTERIOR = "Models/Capsula/capsula_puerta.obj";

Model planetaSelvaticoModel;
Model planetaRocosoModel;
Model capsulaCuerpoModel;
Model capsulaPuertaModel;
Model alaDerModel;
Model alaIzqModel;

static bool tienePlanetaRocosoObj = false;
static bool tienePlanetaSelvaticoObj = false;

// Uniforms extra de shader_light.frag para las superficies (niebla y brillo propio)
static GLuint uniformNiebla = 0;
static GLuint uniformEmision = 0;

// Texturas de los planetas (se renombraron: planetaSelvaTexture.png y planetaRocaTexture.png)
static Texture texturaPlanetaSelva;
static Texture texturaPlanetaRoca;

Skybox skyboxRocoso;
Skybox skyboxSelvatico;
static bool skyboxExtraCargados = false;

enum ZonaActiva
{
	ZONA_ESPACIO = 0,
	ZONA_PLANETA_ROCOSO,
	ZONA_PLANETA_SELVATICO,
	ZONA_PLANETA_OCEANO
};

static ZonaActiva zonaActual = ZONA_ESPACIO;
static bool teclaE_Presionada = false;
static bool tecla2_Presionada = false;
static bool tecla3_Presionada = false;
static bool teclaC_Presionada = false;
static bool teclaK_Presionada = false;

// Camara: la del avatar (tercera persona, lo sigue por la pista y por los planetas) o la "libre",
// que vuela por todo el recorrido, mas rapida y sin limites. La tecla C alterna entre las dos.
static bool camaraLibre = false;

static const glm::vec3 posPlanetaRocosoEspacio(0.0f, 0.0f, 0.0f);
static const float radioMundoPlanetaRocoso = 14.0f;
static const float radioAtmosferaRocoso = 22.0f;

static const glm::vec3 posPlanetaSelvaticoEspacio(120.0f, 10.0f, 90.0f);
static const float radioMundoPlanetaSelvatico = 16.0f;
static const float radioAtmosferaSelvatico = 24.0f;

static const float alturaCamaraSuperficie = 3.5f;

static bool puertaCapsulaAbierta = false;
static float anguloPuertaCapsula = 0.0f;
// Bisagra de la puerta: es su borde inferior, que no es paralelo al eje X (esta girado ~21 grados
// alrededor de Y), por eso se gira sobre su propio eje. La capsula se re-exporto centrada en
// Blender, asi que la base y la bisagra se calculan con las medidas del modelo al cargarlo
// (calcularPuntosCapsula); estos valores son los de la version centrada.
static glm::vec3 pivoteBisagraPuerta(0.0932f, 0.4005f, 0.2868f);
static const glm::vec3 ejeBisagraPuerta(0.9321f, 0.0f, -0.3623f);
static glm::vec3 origenLocalCapsula(0.0f, 0.1037f, 0.0f);
// la bisagra medida desde la esquina minima de la puerta (no cambia si se mueve el modelo)
static const glm::vec3 bisagraDesdeMinimoPuerta(0.1227f, 0.0f, 0.0460f);

static bool panelesAbiertos = true;
static float factorAperturaPaneles = 1.0f;
static const float duracionCicloDiaNoche = 120.0f;

// La pista de carreras ahora esta en Espacio.cpp (curva 3D que pasa por detras de los dos planetas)

static const float dosPi = 6.28318530718f;

static const float guiaPaso = 3.0f;
static const float guiaSateliteAncho = 0.28f;
static const float guiaSateliteLargo = 1.5f;

static const float satRadioX = 16.0f;
static const float satRadioZ = 12.0f;
static const float satAltura = 26.0f;
static const float satOndulacion = 2.0f;
static const float satOmega = 0.35f;

struct PuntoGuia
{
	glm::vec3 pos;
	glm::vec3 forward;
};

static std::vector<PuntoGuia> guiaSatelite;

static glm::vec3 naveForward(1.0f, 0.0f, 0.0f);

static int estadoSatelite = 0;
static float thetaSatelite = 0.0f;
static float rotPanelDer = 0.0f;
static float rotPanelIzq = 0.0f;
static float rotacionRotor = 0.0f;
static float tAnim = 0.0f;

static bool existeArchivo(const std::string& ruta)
{
	std::ifstream f(ruta.c_str());
	return f.good();
}

// Carga un modelo con Assimp y deja un mensaje claro en la consola.
// Regresa false si el archivo no existe o si Assimp no pudo leerlo.
static bool cargarModelo(Model& modelo, const char* ruta)
{
	if (!existeArchivo(ruta))
	{
		printf("[ERROR] No existe el archivo %s (revisa carpeta y nombre)\n", ruta);
		return false;
	}

	printf("Cargando %s ...\n", ruta);
	modelo = Model();
	modelo.LoadModel(ruta);

	if (!modelo.estaCargado())
	{
		printf("[ERROR] Assimp no pudo leer %s\n", ruta);
		return false;
	}

	printf("[OK] %s: %d mallas, %d triangulos\n", ruta, modelo.getMeshCount(), modelo.getNumTriangulos());
	return true;
}

// Base de la capsula (centro en x,z y altura minima) y bisagra de la puerta, con las medidas
// reales del modelo: funcionan aunque la capsula se vuelva a mover en Blender.
static void calcularPuntosCapsula()
{
	if (capsulaCuerpoModel.estaCargado())
	{
		glm::vec3 c = capsulaCuerpoModel.getCentro(), t = capsulaCuerpoModel.getTamano();
		origenLocalCapsula = glm::vec3(c.x, c.y - 0.5f * t.y, c.z);
	}
	if (capsulaPuertaModel.estaCargado())
	{
		glm::vec3 minimoPuerta = capsulaPuertaModel.getCentro() - 0.5f * capsulaPuertaModel.getTamano();
		pivoteBisagraPuerta = minimoPuerta + bisagraDesdeMinimoPuerta;
	}
}

// Si el .mtl del modelo todavia dice "texture_diffuse.png" (nombre con el que exporta Blender)
// y esa imagen ya no existe, se le asigna la textura renombrada que esta en Textures/.
static void asignarTexturaRespaldo(Model& modelo, Texture& textura, const char* ruta)
{
	if (!modelo.estaCargado() || modelo.usaTexturaPropia() || !existeArchivo(ruta))
		return;
	textura = Texture(ruta);
	if (textura.LoadTextureA())
	{
		modelo.SetTexturaRespaldo(&textura);
		printf("     se usa %s como textura del planeta\n", ruta);
	}
}

// Matriz que lleva el centro del modelo al origen y lo escala para que su radio mida
// 'radioDeseado'. Asi no importa en que posicion ni a que tamano se exporto desde Blender.
// (Sustituye a calcularCentroYRadioOBJ: las medidas salen de la malla ya cargada y ya no
// se vuelve a leer el .obj de 100 MB linea por linea.)
static glm::mat4 matrizNormalizada(const Model& modelo, float radioDeseado)
{
	return glm::scale(glm::mat4(1.0f), glm::vec3(radioDeseado / modelo.getRadio()))
		* glm::translate(glm::mat4(1.0f), -modelo.getCentro());
}

static void inicializarSkyboxesPlanetas()
{
	std::vector<std::string> carasBase = {
		"Textures/skybox/right.png",
		"Textures/skybox/left.png",
		"Textures/skybox/top.png",
		"Textures/skybox/bottom.png",
		"Textures/skybox/back.png",
		"Textures/skybox/front.png"
	};

	if (existeArchivo(carasBase[0]))
	{
		skyboxRocoso = Skybox(carasBase);
		skyboxSelvatico = Skybox(carasBase);
		skyboxExtraCargados = true;
	}
}

static void createShaders()
{
	shaderList.reserve(1);
	Shader* shader = new Shader();
	shader->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader);
}

static glm::mat4 matrizOrientacion(const glm::vec3& pos, const glm::vec3& forward, const glm::vec3& upRef)
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

	float metaFactor = panelesAbiertos ? 1.0f : 0.0f;
	factorAperturaPaneles += (metaFactor - factorAperturaPaneles) * fminf(1.0f, 5.0f * dt);

	rotPanelDer = (25.0f * sinf(0.9f * tAnim)) * factorAperturaPaneles + (1.0f - factorAperturaPaneles) * (-85.0f);
	rotPanelIzq = (-25.0f * sinf(0.9f * tAnim)) * factorAperturaPaneles + (1.0f - factorAperturaPaneles) * (85.0f);
	rotacionRotor = fmodf(rotacionRotor + 180.0f * dt, 360.0f);
}

static void animarCapsula(GLfloat dt)
{
	float metaAngulo = puertaCapsulaAbierta ? 75.0f : 0.0f;
	anguloPuertaCapsula += (metaAngulo - anguloPuertaCapsula) * fminf(1.0f, 6.0f * dt);
}

static float actualizarCicloDiaNoche()
{
	float anguloSol = (dosPi / duracionCicloDiaNoche) * tAnim;
	float alturaSol = sinf(anguloSol);
	float factorDia = glm::clamp((alturaSol + 0.25f) / 1.25f, 0.08f, 1.0f);

	glm::vec3 dirLuz = glm::normalize(glm::vec3(cosf(anguloSol), -alturaSol, 0.35f));
	mainLight = DirectionalLight(
		1.0f, 0.96f * factorDia + 0.35f * (1.0f - factorDia), 0.90f * factorDia + 0.65f * (1.0f - factorDia),
		0.12f + 0.28f * factorDia,
		0.15f + 0.75f * factorDia,
		dirLuz.x, dirLuz.y, dirLuz.z);

	return factorDia;
}

static void dibujarCapsulaRetorno(const glm::vec3& posMundo, float escala,
	GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	glm::mat4 baseCapsula = glm::translate(glm::mat4(1.0f), posMundo)
		* glm::scale(glm::mat4(1.0f), glm::vec3(escala))
		* glm::translate(glm::mat4(1.0f), -origenLocalCapsula);

	// La capsula no trae texturas, solo colores Kd en su .mtl (casco blanco, vidrio azul,
	// franja oscura...). RenderModel(uniformColor, tinte) manda ese color por cada material;
	// con RenderModel() normal toda la capsula saldria de un solo color.
	materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(baseCapsula));
	capsulaCuerpoModel.RenderModel(uniformColor, glm::vec3(1.0f));

	// Jerarquia: la puerta hereda la matriz de la capsula y gira sobre su bisagra
	glm::mat4 matrizPuerta = baseCapsula
		* glm::translate(glm::mat4(1.0f), pivoteBisagraPuerta)
		* glm::rotate(glm::mat4(1.0f), glm::radians(anguloPuertaCapsula), ejeBisagraPuerta)
		* glm::translate(glm::mat4(1.0f), -pivoteBisagraPuerta);

	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(matrizPuerta));
	capsulaPuertaModel.RenderModel(uniformColor, glm::vec3(1.0f));
}

// Velocidad de movimiento y de giro de la camara segun el modo y el lugar
static float velocidadCamara()
{
	if (camaraLibre)
		return (zonaActual == ZONA_ESPACIO) ? 45.0f : 30.0f;
	return 14.0f;
}

static float giroCamara()
{
	return camaraLibre ? 0.50f : 0.35f;   // el cursor esta capturado: se puede girar sin limite
}

static LugarJugador lugarJugador()
{
	if (zonaActual == ZONA_PLANETA_ROCOSO) return LUGAR_ROCOSO;
	if (zonaActual == ZONA_PLANETA_SELVATICO) return LUGAR_SELVATICO;
	if (zonaActual == ZONA_PLANETA_OCEANO) return LUGAR_OCEANO;
	return LUGAR_ESPACIO;
}

// Vuelve a crear la camara (la clase Camera no permite cambiar su velocidad ni su orientacion)
static void reiniciarCamara(const glm::vec3& posicion, float yaw, float pitch)
{
	camera = Camera(posicion, glm::vec3(0.0f, 1.0f, 0.0f), yaw, pitch, velocidadCamara(), giroCamara());
}

// Tecla C: alterna entre la camara sobre la pista y la camara libre, sin moverla de lugar
static void cambiarModoCamara()
{
	camaraLibre = !camaraLibre;
	glm::vec3 dir = camera.getCameraDirection();
	float yaw = glm::degrees(atan2f(dir.z, dir.x));
	float pitch = glm::degrees(asinf(glm::clamp(dir.y, -1.0f, 1.0f)));
	reiniciarCamara(camera.getCameraPosition(), yaw, pitch);
	printf("Camara %s\n", camaraLibre ? "LIBRE (vuela por todo el recorrido)" : "SOBRE LA PISTA");
}

// Nave rival dentro de un planeta o del oceano (en el espacio se dibuja con sus llamas y luces)
static void dibujarRivalEn(LugarJugador lugar, GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecularIntensity, GLuint uniformShininess)
{
	glm::vec3 pos, adelante, arriba;
	LugarJugador lugarRival;
	EstadoRival(tAnim, lugarRival, pos, adelante, arriba);
	if (lugarRival != lugar)
		return;
	glm::mat4 modelo = matrizOrientacion(pos, adelante, arriba);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(modelo));
	glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
	recursos.plainTexture.UseTexture();
	materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
	recursos.naveModel.RenderModel();
}

// Sale de la atmosfera del planeta actual: la camara aparece sobre la pista de carreras, en el
// punto de salida de ese planeta (como en el croquis), mirando hacia donde avanza la carrera.
static void salirAlEspacio()
{
	if (zonaActual == ZONA_ESPACIO)
		return;
	int planeta = PLANETA_ROCOSO;
	if (zonaActual == ZONA_PLANETA_SELVATICO) planeta = PLANETA_SELVATICO;
	else if (zonaActual == ZONA_PLANETA_OCEANO) planeta = PLANETA_OCEANO;
	zonaActual = ZONA_ESPACIO;
	float yaw = ColocarJugadorAlSalir(planeta);   // en su nave sobre la pista (o en la base si aun no la aborda)
	reiniciarCamara(PosicionJugador() + glm::vec3(0.0f, 4.0f, 0.0f), yaw, camaraLibre ? 0.0f : -10.0f);
}

static void actualizarTransicionAtmosfera(bool* teclas)
{
	// con la camara del avatar, lo que entra a los planetas o cruza el portal es el avatar
	glm::vec3 camPos = camaraLibre ? camera.getCameraPosition() : PosicionJugador();
	bool pulsoE = teclas[GLFW_KEY_E] && !teclaE_Presionada;
	teclaE_Presionada = teclas[GLFW_KEY_E];

	if (teclas[GLFW_KEY_2] && !tecla2_Presionada)
		puertaCapsulaAbierta = !puertaCapsulaAbierta;
	tecla2_Presionada = teclas[GLFW_KEY_2];

	if (teclas[GLFW_KEY_3] && !tecla3_Presionada)
		panelesAbiertos = !panelesAbiertos;
	tecla3_Presionada = teclas[GLFW_KEY_3];

	if (teclas[GLFW_KEY_C] && !teclaC_Presionada)
		cambiarModoCamara();
	teclaC_Presionada = teclas[GLFW_KEY_C];

	// K junto a miNave (en la base): la nave pasa a ser el avatar, aparece en la meta y empieza la carrera
	if (teclas[GLFW_KEY_K] && !teclaK_Presionada && zonaActual == ZONA_ESPACIO && !camaraLibre)
	{
		float yaw;
		if (IntentarAbordarNave(tAnim, yaw))
			reiniciarCamara(PosicionJugador() + glm::vec3(0.0f, 4.0f, 0.0f), yaw, -10.0f);
	}
	// K en el planeta rocoso o selvatico: bajar de la nave (queda estacionada) o volver a subir
	else if (teclas[GLFW_KEY_K] && !teclaK_Presionada && !camaraLibre
		&& (zonaActual == ZONA_PLANETA_ROCOSO || zonaActual == ZONA_PLANETA_SELVATICO))
	{
		BajarOSubirNave(lugarJugador());
	}
	teclaK_Presionada = teclas[GLFW_KEY_K];

	if (zonaActual == ZONA_ESPACIO)
	{
		float distRocoso = glm::distance(camPos, posPlanetaRocosoEspacio);
		float distSelvatico = glm::distance(camPos, posPlanetaSelvaticoEspacio);
		float distOceano = glm::distance(camPos, CentroPlanetaOceano());

		if (distRocoso < radioAtmosferaRocoso || (pulsoE && distRocoso < radioAtmosferaRocoso * 1.6f))
		{
			zonaActual = ZONA_PLANETA_ROCOSO;
			glm::vec3 inicio = PosicionInicio(SUPERFICIE_ROCOSA);
			float yaw = ColocarJugadorEnPlaneta(LUGAR_ROCOSO, inicio, YawInicio(SUPERFICIE_ROCOSA));   // sigue en su nave
			reiniciarCamara(glm::vec3(inicio.x, alturaCamaraSuperficie, inicio.z), yaw, camaraLibre ? 0.0f : -12.0f);
		}
		else if (distSelvatico < radioAtmosferaSelvatico || (pulsoE && distSelvatico < radioAtmosferaSelvatico * 1.6f))
		{
			zonaActual = ZONA_PLANETA_SELVATICO;
			glm::vec3 inicio = PosicionInicio(SUPERFICIE_SELVATICA);
			float yaw = ColocarJugadorEnPlaneta(LUGAR_SELVATICO, inicio, YawInicio(SUPERFICIE_SELVATICA));   // sigue en su nave
			reiniciarCamara(glm::vec3(inicio.x, alturaCamaraSuperficie, inicio.z), yaw, camaraLibre ? 0.0f : -12.0f);
		}
		else if (distOceano < RadioAtmosferaOceano() || (pulsoE && distOceano < RadioAtmosferaOceano() * 1.6f))
		{
			zonaActual = ZONA_PLANETA_OCEANO;
			float yaw = ColocarJugadorEnOceano();   // de espaldas a la ola, viendo el mar abierto
			reiniciarCamara(PosicionJugador() + glm::vec3(0.0f, 5.0f, 12.0f), yaw, camaraLibre ? 0.0f : -6.0f);
		}
	}
	else if (zonaActual == ZONA_PLANETA_OCEANO)
	{
		// en el oceano: al llegar a la cresta de la ola gigante (o con E) se sale a la pista
		if (pulsoE || LlegoALaCresta(camPos))
			salirAlEspacio();
	}
	else
	{
		// en un planeta: E o cruzar el portal del final del camino regresan al espacio
		TipoSuperficie zona = (zonaActual == ZONA_PLANETA_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
		if (pulsoE || (JugadorEnNave() && CruzandoPortal(zona, camPos)))   // el portal se cruza en la nave
			salirAlEspacio();
	}
}

int main()
{
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	CrearQuad();
	CrearLlamas();
	createShaders();
	uniformNiebla = glGetUniformLocation(shaderList[0].GetShaderID(), "niebla");
	uniformEmision = glGetUniformLocation(shaderList[0].GetShaderID(), "emision");

	camera = Camera(glm::vec3(0.0f, 30.0f, -85.0f), glm::vec3(0.0f, 1.0f, 0.0f),
		90.0f, -8.0f, 20.0f, 0.25f);

	LoadNave();
	LoadLlamas();
	LoadPlaneta();
	LoadSatelite();
	LoadSkyboxP1();
	inicializarSkyboxesPlanetas();

	// Planetas y capsula. Cada modelo carga su propia textura desde su carpeta
	// (Models/planetaSelva/texture_diffuse.png), ya no hace falta cargarla a mano aqui.
	// Los .obj de 100 MB tardan varios segundos: la consola va mostrando el avance.
	tienePlanetaSelvaticoObj = cargarModelo(planetaSelvaticoModel, RUTA_PLANETA_SELVATICO);
	tienePlanetaRocosoObj = cargarModelo(planetaRocosoModel, RUTA_PLANETA_ROCOSO);
	asignarTexturaRespaldo(planetaSelvaticoModel, texturaPlanetaSelva, "Textures/planetaSelvaTexture.png");
	asignarTexturaRespaldo(planetaRocosoModel, texturaPlanetaRoca, "Textures/planetaRocaTexture.png");
	if (!cargarModelo(capsulaCuerpoModel, RUTA_CAPSULA_CUERPO))
		cargarModelo(capsulaCuerpoModel, RUTA_CAPSULA_CUERPO_ANTERIOR);
	if (!cargarModelo(capsulaPuertaModel, RUTA_CAPSULA_PUERTA))
		cargarModelo(capsulaPuertaModel, RUTA_CAPSULA_PUERTA_ANTERIOR);
	calcularPuntosCapsula();

	// Selva y desierto de los planetas (arboles, rocas, cristales, camino y pisos)
	InicializarSuperficies();
	// Pista de carreras y cinturon de asteroides
	InicializarEspacio();
	// Reloj en pantalla
	InicializarReloj();

	// Planeta oceano (por fuera y por dentro)
	InicializarOceano();

	// Avatar: empieza en la base, junto a la pista y antes de la meta, como Paul y con miNave al lado
	InicializarJugador();
	{
		float yaw = ColocarJugadorEnBase();
		reiniciarCamara(PosicionJugador() + glm::vec3(0.0f, 6.0f, 0.0f), yaw, -15.0f);
	}

	if (existeArchivo("Models/AlaDer.obj"))
	{
		alaDerModel = Model();
		alaDerModel.LoadModel("Models/AlaDer.obj");
	}
	if (existeArchivo("Models/AlaIzq.obj"))
	{
		alaIzqModel = Model();
		alaIzqModel.LoadModel("Models/AlaIzq.obj");
	}

	recursos.plainTexture = Texture("Textures/plain.png");
	recursos.plainTexture.LoadTextureA();

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
		(GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1800.0f);

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

		if (camaraLibre)
			camera.keyControl(teclas, deltaTime);   // con la camara del avatar, W A S D mueven al avatar
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		actualizarTransicionAtmosfera(teclas);

		if (!camaraLibre)
		{
			// el avatar se mueve con W A S D y la camara lo sigue en tercera persona
			ActualizarJugador(teclas, deltaTime, lugarJugador(), camera, tAnim);
		}
		else if (zonaActual == ZONA_PLANETA_OCEANO)
		{
			// camara libre en el oceano: no se mete bajo el agua ni dentro de la ola
			camera.setPosition(AjustarCamaraOceano(camera.getCameraPosition()));
		}
		else if (zonaActual != ZONA_ESPACIO && camera.getCameraPosition().y < 1.5f)
		{
			// camara libre en un planeta: vuela por donde quiera, pero no atraviesa el suelo
			glm::vec3 p = camera.getCameraPosition();
			p.y = 1.5f;
			camera.setPosition(p);
		}

		tAnim += deltaTime;
		animarSatelite(deltaTime);
		animarCapsula(deltaTime);

		float factorDia = actualizarCicloDiaNoche();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 viewMatrix = camera.calculateViewMatrix();
		glm::vec3 eyePos = camera.getCameraPosition();
		// cielo de cada lugar: el espacio sin cambios; en los planetas, tinte y bruma en el horizonte
		AmbienteSuperficie ambiente = {};
		recursos.skybox.SetCieloSolido(glm::vec3(0.0f), 0.0f);   // solo el oceano usa cielo despejado
		if (zonaActual == ZONA_PLANETA_OCEANO)
		{
			// cielo nublado y gris azulado
			AmbienteOceano ao = ObtenerAmbienteOceano(factorDia);
			ambiente.colorNiebla = ao.colorNiebla;
			ambiente.densidadNiebla = ao.densidadNiebla;
			ambiente.tinteCielo = ao.tinteCielo;
			ambiente.brumaHorizonte = ao.brumaHorizonte;
			recursos.skybox.SetAmbiente(ambiente.tinteCielo, ambiente.brumaHorizonte);
			recursos.skybox.SetCieloSolido(ao.colorCenit, ao.cieloSolido);   // cielo despejado azul claro
		}
		else if (zonaActual != ZONA_ESPACIO)
		{
			ambiente = ObtenerAmbiente(zonaActual == ZONA_PLANETA_ROCOSO ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA, factorDia);
			recursos.skybox.SetAmbiente(ambiente.tinteCielo, ambiente.brumaHorizonte);
			skyboxRocoso.SetAmbiente(ambiente.tinteCielo, ambiente.brumaHorizonte);
			skyboxSelvatico.SetAmbiente(ambiente.tinteCielo, ambiente.brumaHorizonte);
		}
		else
		{
			recursos.skybox.SetAmbiente(glm::vec3(1.0f), glm::vec4(0.0f));
		}

		if (zonaActual == ZONA_PLANETA_ROCOSO && skyboxExtraCargados)
			skyboxRocoso.DrawSkybox(viewMatrix, projection);
		else if (zonaActual == ZONA_PLANETA_SELVATICO && skyboxExtraCargados)
			skyboxSelvatico.DrawSkybox(viewMatrix, projection);
		else
			recursos.skybox.DrawSkybox(viewMatrix, projection);

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
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniform3f(uniformEyePosition, eyePos.x, eyePos.y, eyePos.z);
		glUniform2f(uniformTextureOffset, 0.0f, 0.0f);
		glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
		if (zonaActual == ZONA_ESPACIO)
			glUniform4f(uniformNiebla, 0.0f, 0.0f, 0.0f, 0.0f);   // en el espacio no hay niebla
		else
			glUniform4f(uniformNiebla, ambiente.colorNiebla.x, ambiente.colorNiebla.y, ambiente.colorNiebla.z, ambiente.densidadNiebla);

		if (zonaActual == ZONA_ESPACIO)
		{
			glm::vec3 posNave, tanNave, arribaRival;
			LugarJugador lugarRival;
			// la nave rival espera en la meta hasta que el jugador sube a miNave (tecla K)
			EstadoRival(tAnim, lugarRival, posNave, tanNave, arribaRival);
			bool naveVisible = (lugarRival == LUGAR_ESPACIO);   // si no, va corriendo dentro de un planeta

			glm::vec3 objetivo = glm::normalize(tanNave);
			naveForward = glm::normalize(glm::mix(naveForward, objetivo, fminf(1.0f, 8.0f * deltaTime)));

			glm::mat4 naveModelo = matrizOrientacion(posNave, naveForward, mundoArriba);

			glm::vec3 motor1Pos = glm::vec3(naveModelo * glm::vec4(0.5f, 0.10f, -2.72f, 1.0f));
			glm::vec3 motor2Pos = glm::vec3(naveModelo * glm::vec4(-0.5f, 0.10f, -2.72f, 1.0f));

			pointLights[0].SetPos(motor1Pos);
			pointLights[1].SetPos(motor2Pos);

			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(pointLights, 2);
			shaderList[0].SetSpotLights(nullptr, 0);

			// Si planetaRoca.obj no se pudo cargar se usa el planeta estilizado como respaldo
			Model& modeloRocoso = tienePlanetaRocosoObj ? planetaRocosoModel : recursos.planetaModel;
			glm::mat4 planetaRocosoMat = glm::translate(glm::mat4(1.0f), posPlanetaRocosoEspacio)
				* glm::rotate(glm::mat4(1.0f), 0.12f * tAnim, glm::vec3(0.0f, 1.0f, 0.0f))
				* matrizNormalizada(modeloRocoso, radioMundoPlanetaRocoso);

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(planetaRocosoMat));
			materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (tienePlanetaRocosoObj)
			{
				glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
				planetaRocosoModel.RenderModel();
			}
			else
			{
				glUniform3f(uniformColor, 0.62f, 0.52f, 0.38f);
				recursos.plainTexture.UseTexture();
				recursos.planetaModel.RenderModel();
			}

			if (tienePlanetaSelvaticoObj)
			{
				glm::mat4 planetaSelvaticoMat = glm::translate(glm::mat4(1.0f), posPlanetaSelvaticoEspacio)
					* glm::rotate(glm::mat4(1.0f), 0.18f * tAnim, glm::vec3(0.0f, 1.0f, 0.0f))
					* matrizNormalizada(planetaSelvaticoModel, radioMundoPlanetaSelvatico);

				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(planetaSelvaticoMat));
				glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
				materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				planetaSelvaticoModel.RenderModel();   // enlaza su textura (la del .mtl o planetaSelvaTexture.png)
			}

			// pista de carreras (marcas con luces que corren) y cinturon de asteroides
			UniformsEspacio ue;
			ue.model = uniformModel;
			ue.color = uniformColor;
			ue.especular = uniformSpecularIntensity;
			ue.brillo = uniformShininess;
			ue.emision = uniformEmision;
			DibujarCinturon(eyePos, camera.getCameraDirection(), tAnim, ue);
			DibujarPlanetaOceano(tAnim, ue);
			DibujarPista(tAnim, ue);
			DibujarBase(tAnim, ue);

			// avatar (Paul en la base o miNave en la pista) y miNave estacionada en la base
			UniformsJugador uj;
			uj.model = uniformModel;
			uj.color = uniformColor;
			uj.especular = uniformSpecularIntensity;
			uj.brillo = uniformShininess;
			DibujarJugador(LUGAR_ESPACIO, tAnim, uj);
			dibujarGuia(guiaSatelite, glm::vec3(1.0f, 0.35f, 0.85f), guiaSateliteAncho, guiaSateliteLargo,
				uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(naveModelo));
			glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
			recursos.plainTexture.UseTexture();
			materialBrillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			if (naveVisible)
				recursos.naveModel.RenderModel();

			glm::vec3 satPos = posicionSatelite(estadoSatelite, thetaSatelite);
			glm::vec3 satTan = tangenteSatelite(estadoSatelite, thetaSatelite);
			float satYaw = atan2f(satTan.x, satTan.z);
			glm::mat4 satBase = glm::translate(glm::mat4(1.0f), satPos)
				* glm::rotate(glm::mat4(1.0f), satYaw, glm::vec3(0.0f, 1.0f, 0.0f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

			glm::mat4 panelDer = satBase
				* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.2f, 4.0f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotPanelDer), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 panelIzq = satBase
				* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.2f, 4.0f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotPanelIzq), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 matrizRotor = satBase
				* glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f))
				* glm::rotate(glm::mat4(1.0f), glm::radians(rotacionRotor), glm::vec3(0.0f, 0.0f, 1.0f));

			glm::vec3 dirSat = glm::normalize(eyePos - satPos);
			DirectionalLight luzSatelite(1.0f, 1.0f, 1.0f, 0.8f, 0.9f, dirSat.x, dirSat.y, dirSat.z);
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
			if (naveVisible)
				geometria.llamaMesh->RenderMeshModel();

			glm::mat4 llama2 = naveModelo
				* glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.10f, -2.92f))
				* glm::scale(glm::mat4(1.0f), glm::vec3(pulso));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(llama2));
			if (naveVisible)
				geometria.llamaMesh->RenderMeshModel();

			glDepthMask(GL_TRUE);
			glDisable(GL_BLEND);
		}
		else if (zonaActual == ZONA_PLANETA_OCEANO)
		{
			// dentro del planeta oceano: mar con olas y la ola gigante; las dos naves vuelan sobre el agua
			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetPointLights(nullptr, 0);
			shaderList[0].SetSpotLights(nullptr, 0);
			UniformsEspacio ue;
			ue.model = uniformModel;
			ue.color = uniformColor;
			ue.especular = uniformSpecularIntensity;
			ue.brillo = uniformShininess;
			ue.emision = uniformEmision;
			DibujarOceano(eyePos, ue);
			dibujarRivalEn(LUGAR_OCEANO, uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);
			UniformsJugador uj;
			uj.model = uniformModel;
			uj.color = uniformColor;
			uj.especular = uniformSpecularIntensity;
			uj.brillo = uniformShininess;
			DibujarJugador(LUGAR_OCEANO, tAnim, uj);
		}
		else
		{
			TipoSuperficie superficie = (zonaActual == ZONA_PLANETA_ROCOSO) ? SUPERFICIE_ROCOSA : SUPERFICIE_SELVATICA;
			glm::vec3 posCapsula = PosicionCapsula(superficie);

			bool esDeNoche = (factorDia < 0.45f);
			if (esDeNoche)
			{
				// luminarias a los lados de la capsula
				pointLights[0].SetPos(posCapsula + glm::vec3(7.0f, 4.0f, 6.0f));
				pointLights[1].SetPos(posCapsula + glm::vec3(-7.0f, 4.0f, 6.0f));
				shaderList[0].SetPointLights(pointLights, 2);
			}
			else
			{
				shaderList[0].SetPointLights(nullptr, 0);
			}

			shaderList[0].SetDirectionalLight(&mainLight);
			shaderList[0].SetSpotLights(nullptr, 0);

			UniformsSuperficie u;
			u.model = uniformModel;
			u.color = uniformColor;
			u.offsetTextura = uniformTextureOffset;
			u.especular = uniformSpecularIntensity;
			u.brillo = uniformShininess;
			u.emision = uniformEmision;
			DibujarSuperficie(superficie, eyePos, camera.getCameraDirection(), factorDia, tAnim, u);
			UniformsJugador uj;
			uj.model = uniformModel;
			uj.color = uniformColor;
			uj.especular = uniformSpecularIntensity;
			uj.brillo = uniformShininess;
			DibujarJugador(lugarJugador(), tAnim, uj);   // la nave del jugador sobre el camino
			dibujarRivalEn(lugarJugador(), uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);   // y la rival

			dibujarCapsulaRetorno(posCapsula, 4.0f,
				uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess);
		}

		// reloj en pantalla: 120 s del ciclo = 24 horas; el ciclo empieza al amanecer (6:00)
		float horaDelDia = fmodf(6.0f + 24.0f * tAnim / duracionCicloDiaNoche, 24.0f);
		int dia = 1 + (int)((tAnim + 0.25f * duracionCicloDiaNoche) / duracionCicloDiaNoche);   // cambia a medianoche
		int clima = 0;
		if (zonaActual == ZONA_PLANETA_SELVATICO) clima = 1;
		else if (zonaActual == ZONA_PLANETA_ROCOSO) clima = 2;
		else if (zonaActual == ZONA_PLANETA_OCEANO) clima = 3;   // despejado
		DibujarReloj(mainWindow.getBufferWidth(), mainWindow.getBufferHeight(), horaDelDia, dia, clima);

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}