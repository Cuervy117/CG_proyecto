#include "Oceano.h"

#include <cmath>
#include <cstdio>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// ---------------- por fuera ----------------
const float PI_OCEANO = 3.14159265f;
const glm::vec3 CENTRO_OCEANO(150.0f, -30.0f, -40.0f);
const float RADIO_OCEANO = 18.0f;
const float ATMOSFERA_OCEANO = 27.0f;

// ---------------- por dentro ----------------
const float INICIO_S = -520.0f;		// la nave aparece 520 unidades antes de donde empieza la ola
const float RADIO_CURVA = 45.0f;	// el agua pasa de horizontal a vertical en esta curva
const float ALTO_PARED = 110.0f;	// pared vertical de agua
const float RADIO_CRESTA = 10.0f;	// la cresta se enrosca hacia atrás
const float BAJADA_ATRAS = 140.0f;	// del otro lado baja hasta el nivel del mar
const float ANCHO_OLA = 5000.0f;	// de horizonte a horizonte
const float MITAD_MAR = 3000.0f;	// el mar plano (sigue a la cámara)
const float MOSAICO = 60.0f;		// unidades por cada repetición de la textura del agua
const float LIMITE_X = 1500.0f;
const float LIMITE_ATRAS = -1800.0f;

const float S_CURVA = RADIO_CURVA * PI_OCEANO * 0.5f;				// fin de la curva
const float S_PARED = S_CURVA + ALTO_PARED;							// fin de la pared
const float S_CIMA = S_PARED + RADIO_CRESTA * PI_OCEANO * 0.5f;		// punto más alto
const float S_CRESTA = S_PARED + RADIO_CRESTA * PI_OCEANO;			// fin de la cresta
const float Z_PARED = RADIO_CURVA;
const float ARRIBA_PARED = RADIO_CURVA + ALTO_PARED;


Oceano::Oceano()
{
	hayPlaneta = false;
	mar = nullptr;
	ola = nullptr;
	inicializado = false;
	Material_planeta = Material(4.0f, 256.0f);	// muy brillante: el reflejo del sol sobre el agua
	Material_agua = Material(1.2f, 64.0f);
}

// Perfil de la ola en el plano (z, y): punto, tangente (hacia donde crece s) y normal (hacia el aire)
void Oceano::Perfil(float s, glm::vec2& p, glm::vec2& tangente, glm::vec2& normal)
{
	if (s <= 0.0f)
	{
		// mar plano
		p = glm::vec2(s, 0.0f);
		tangente = glm::vec2(1.0f, 0.0f);
		normal = glm::vec2(0.0f, 1.0f);
	}
	else if (s <= S_CURVA)
	{
		// cuarto de círculo: de horizontal a vertical
		float a = s / RADIO_CURVA;
		p = glm::vec2(RADIO_CURVA * sinf(a), RADIO_CURVA - RADIO_CURVA * cosf(a));
		tangente = glm::vec2(cosf(a), sinf(a));
		normal = glm::vec2(-sinf(a), cosf(a));
	}
	else if (s <= S_PARED)
	{
		// pared vertical
		p = glm::vec2(Z_PARED, RADIO_CURVA + (s - S_CURVA));
		tangente = glm::vec2(0.0f, 1.0f);
		normal = glm::vec2(-1.0f, 0.0f);
	}
	else
	{
		// la cresta se enrosca hacia atrás
		float b = fminf((s - S_PARED) / RADIO_CRESTA, PI_OCEANO);
		p = glm::vec2(Z_PARED + RADIO_CRESTA - RADIO_CRESTA * cosf(b), ARRIBA_PARED + RADIO_CRESTA * sinf(b));
		tangente = glm::vec2(sinf(b), cosf(b));
		normal = glm::vec2(-cosf(b), sinf(b));
	}
}

glm::vec3 Oceano::En3D(glm::vec2 v)
{
	return glm::vec3(0.0f, v.y, v.x);   // (z, y) -> (0, y, z)
}

void Oceano::AgregarVertice(std::vector<GLfloat>& v, glm::vec3 p, float s, float t, glm::vec3 n)
{
	// x y z u v nx ny nz, con las normales invertidas como en las mallas de la práctica
	GLfloat d[] = { p.x, p.y, p.z, s, t, -n.x, -n.y, -n.z };
	v.insert(v.end(), d, d + 8);
}

// La ola: el perfil (curva, pared, cresta y bajada del otro lado) repetido a lo ancho
void Oceano::ConstruirOla()
{
	std::vector<glm::vec2> puntos, normales;
	for (int k = 0; k <= 70; k++)
	{
		float s = S_CRESTA * k / 70.0f;
		glm::vec2 p, t, n;
		Perfil(s, p, t, n);
		puntos.push_back(p);
		normales.push_back(n);
	}
	glm::vec2 inicioBajada = puntos.back();
	for (int k = 1; k <= 24; k++)
	{
		float u = k / 24.0f;   // bajada suave (medio coseno) del otro lado
		float y = ARRIBA_PARED * (0.5f + 0.5f * cosf(PI_OCEANO * u));
		float dy = -ARRIBA_PARED * 0.5f * PI_OCEANO * sinf(PI_OCEANO * u) / BAJADA_ATRAS;
		puntos.push_back(glm::vec2(inicioBajada.x + BAJADA_ATRAS * u, y));
		normales.push_back(glm::normalize(glm::vec2(-dy, 1.0f)));
	}
	std::vector<float> largo(puntos.size(), 0.0f);
	for (size_t i = 1; i < puntos.size(); i++)
		largo[i] = largo[i - 1] + glm::length(puntos[i] - puntos[i - 1]);

	const int columnas = 100;
	std::vector<GLfloat> v;
	std::vector<unsigned int> indices;
	for (int c = 0; c <= columnas; c++)
	{
		float x = -0.5f * ANCHO_OLA + ANCHO_OLA * c / columnas;
		for (size_t i = 0; i < puntos.size(); i++)
			AgregarVertice(v, glm::vec3(x, puntos[i].y, puntos[i].x), x / MOSAICO, largo[i] / MOSAICO, En3D(normales[i]));
	}
	unsigned int filas = (unsigned int)puntos.size();
	for (int c = 0; c < columnas; c++)
	{
		for (unsigned int i = 0; i + 1 < filas; i++)
		{
			unsigned int a = c * filas + i, b = a + 1, d = a + filas, e = d + 1;
			unsigned int t[] = { a, d, b, b, d, e };
			indices.insert(indices.end(), t, t + 6);
		}
	}
	ola = new MeshModel();
	ola->CreateMeshModel(v.data(), indices.data(), (unsigned int)v.size(), (unsigned int)indices.size());
}

// El mar plano: un cuadro enorme con la textura repetida (se dibuja centrado bajo la cámara)
void Oceano::ConstruirMar()
{
	std::vector<GLfloat> v;
	float m = MITAD_MAR;
	glm::vec3 arriba(0.0f, 1.0f, 0.0f);
	AgregarVertice(v, glm::vec3(-m, 0.0f, -m), -m / MOSAICO, -m / MOSAICO, arriba);
	AgregarVertice(v, glm::vec3(m, 0.0f, -m), m / MOSAICO, -m / MOSAICO, arriba);
	AgregarVertice(v, glm::vec3(m, 0.0f, m), m / MOSAICO, m / MOSAICO, arriba);
	AgregarVertice(v, glm::vec3(-m, 0.0f, m), -m / MOSAICO, m / MOSAICO, arriba);
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	mar = new MeshModel();
	mar->CreateMeshModel(v.data(), indices, 32, 6);
}

void Oceano::InicializarOceano()
{
	if (inicializado)
		return;
	PlanetaOceano_M = Model();
	PlanetaOceano_M.LoadModel("Models/planetaOceano/planetaOceano.obj");
	hayPlaneta = PlanetaOceano_M.estaCargado();
	if (hayPlaneta)
		printf("[OK] Planeta oceano: Models/planetaOceano/planetaOceano.obj\n");

	aguaTexture = Texture("Textures/aguaOceanoTexture.png");
	aguaTexture.LoadTextureA();

	ConstruirMar();
	ConstruirOla();
	inicializado = true;
	printf("[OK] Oceano por dentro: ola de %.0f de alto a %.0f unidades de donde se aparece\n", ARRIBA_PARED + RADIO_CRESTA, -INICIO_S);
}

glm::vec3 Oceano::CentroPlanetaOceano()
{
	return CENTRO_OCEANO;
}

float Oceano::RadioAtmosferaOceano()
{
	return ATMOSFERA_OCEANO;
}

void Oceano::DibujarPlanetaOceano(float tiempo, GLuint modelLocation, GLuint colorLocation,
	GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation)
{
	if (!hayPlaneta)
		return;
	glm::mat4 model(1.0);

	// planeta: gira despacio sobre su eje Y. El modelo se centra y se escala para medir RADIO_OCEANO
	model = glm::mat4(1.0);
	model = glm::translate(model, CENTRO_OCEANO);
	model = glm::rotate(model, 0.05f * tiempo, glm::vec3(0.0f, 1.0f, 0.0f));   // ángulo en radianes
	model = glm::scale(model, glm::vec3(RADIO_OCEANO));
	model = glm::scale(model, glm::vec3(1.0f / PlanetaOceano_M.getRadio()));
	model = glm::translate(model, -PlanetaOceano_M.getCentro());
	glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(emisionLocation, 0.0f, 0.0f, 0.0f);
	Material_planeta.UseMaterial(specularIntensityLocation, shininessLocation);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	PlanetaOceano_M.RenderModel();

	// NUEVO: halo de atmósfera. Tres esferas un poco más grandes que solo suman luz azul
	// (blending aditivo GL_ONE y sin escribir profundidad), más intensa en la orilla.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glUniform3f(colorLocation, 0.0f, 0.0f, 0.0f);
	float capas[3] = { 1.03f, 1.07f, 1.12f };
	float fuerza[3] = { 0.55f, 0.35f, 0.18f };
	for (int k = 0; k < 3; k++)
	{
		model = glm::mat4(1.0);
		model = glm::translate(model, CENTRO_OCEANO);
		model = glm::scale(model, glm::vec3(RADIO_OCEANO * capas[k]));
		model = glm::scale(model, glm::vec3(1.0f / PlanetaOceano_M.getRadio()));
		model = glm::translate(model, -PlanetaOceano_M.getCentro());
		glUniform3f(emisionLocation, 0.25f * fuerza[k], 0.55f * fuerza[k], 1.2f * fuerza[k]);
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		PlanetaOceano_M.RenderModel();
	}
	glUniform3f(emisionLocation, 0.0f, 0.0f, 0.0f);
	glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

AmbienteOceano Oceano::ObtenerAmbienteOceano(float factorDia)
{
	// cielo despejado azul claro; de noche se oscurece
	AmbienteOceano a;
	float f = 0.25f + 0.75f * glm::clamp(factorDia, 0.0f, 1.0f);
	glm::vec3 horizonte(0.64f, 0.76f, 0.85f);
	glm::vec3 cenit(0.55f, 0.71f, 0.84f);
	a.colorNiebla = horizonte * f;
	a.densidadNiebla = 0.0007f;			// el mar se pierde en el horizonte
	a.tinteCielo = glm::vec3(1.0f);
	a.brumaHorizonte = glm::vec4(horizonte * f, 1.0f);
	a.colorCenit = cenit * f;
	a.cieloSolido = 1.0f;
	return a;
}

void Oceano::InicioOceano(float& x, float& s, float& yawGrados)
{
	x = 0.0f;
	s = INICIO_S;
	yawGrados = -90.0f;   // de espaldas a la ola: se ve el océano abierto; la ola está atrás
}

void Oceano::PuntoOla(float x, float s, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba)
{
	glm::vec2 p, t, n;
	Perfil(s, p, t, n);
	arriba = En3D(n);
	adelante = En3D(t);
	posicion = glm::vec3(x, p.y, p.x) + arriba * altura;
}

glm::vec2 Oceano::LimitarOceano(float x, float s)
{
	return glm::vec2(glm::clamp(x, -LIMITE_X, LIMITE_X), fmaxf(s, LIMITE_ATRAS));
}

glm::vec3 Oceano::AjustarCamaraOceano(glm::vec3 camara)
{
	glm::vec3 c = camara;
	if (c.y <= RADIO_CURVA)
	{
		// frente a la curva: no pasar del agua
		float cosA = (RADIO_CURVA - fmaxf(c.y, 0.0f)) / RADIO_CURVA;
		float zSuperficie = RADIO_CURVA * sqrtf(fmaxf(0.0f, 1.0f - cosA * cosA));
		if (c.z > zSuperficie - 2.0f)
			c.z = zSuperficie - 2.0f;
	}
	else if (c.y <= ARRIBA_PARED)
	{
		// frente a la pared
		if (c.z > Z_PARED - 2.0f)
			c.z = Z_PARED - 2.0f;
	}
	// sobre el mar plano
	if (c.z < Z_PARED && c.y < 1.5f)
		c.y = 1.5f;
	return c;
}

bool Oceano::LlegoALaCresta(glm::vec3 p)
{
	return p.y >= ARRIBA_PARED + 0.8f * RADIO_CRESTA && p.z >= Z_PARED - 6.0f && p.z <= Z_PARED + 2.0f * RADIO_CRESTA + 6.0f;
}

float Oceano::LargoCarreraOceano()
{
	return S_CIMA - INICIO_S;
}

void Oceano::PuntoCarreraOceano(float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba)
{
	PuntoOla(carril, INICIO_S + distancia, altura, posicion, adelante, arriba);
}

void Oceano::DibujarOceano(glm::vec3 posCamara, glm::vec2 toffset, GLuint modelLocation, GLuint colorLocation,
	GLuint offsetLocation, GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation)
{
	if (!inicializado)
		return;
	glm::mat4 model(1.0);
	glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(emisionLocation, 0.0f, 0.0f, 0.0f);
	Material_agua.UseMaterial(specularIntensityLocation, shininessLocation);
	aguaTexture.UseTexture();
	//textura con movimiento (práctica 9): toffset desplaza las coordenadas UV del agua
	glUniform2fv(offsetLocation, 1, glm::value_ptr(toffset));

	// el mar sigue a la cámara en pasos del tamaño de la textura (así no "nada" y nunca se acaba)
	float sx = floorf(posCamara.x / MOSAICO) * MOSAICO;
	float sz = floorf(posCamara.z / MOSAICO) * MOSAICO;
	model = glm::mat4(1.0);
	model = glm::translate(model, glm::vec3(sx, -0.05f, sz));   // apenas abajo del inicio de la ola
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	mar->RenderMeshModel();

	// la ola gigante
	model = glm::mat4(1.0);
	glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
	ola->RenderMeshModel();

	// la textura de los demás objetos queda quieta
	toffset = glm::vec2(0.0f, 0.0f);
	glUniform2fv(offsetLocation, 1, glm::value_ptr(toffset));
}

Oceano::~Oceano()
{
}

