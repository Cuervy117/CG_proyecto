#include "Oceano.h"

#include <cmath>
#include <cstdio>
#include <vector>
#include <fstream>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Model.h"
#include "Texture.h"
#include "Material.h"
#include "Mesh_tn.h"

namespace
{
	const float PI_F = 3.14159265f;

	// ---------------- por fuera ----------------
	const char* RUTA_PLANETA = "Models/planetaOceano/planetaOceano.obj";
	const char* RUTA_TEXTURA_PLANETA = "Textures/planetaOceanoTexture.png";
	const glm::vec3 CENTRO(150.0f, -30.0f, -40.0f);
	const float RADIO = 18.0f;
	const float ATMOSFERA = 27.0f;

	// ---------------- por dentro ----------------
	const char* RUTA_TEXTURA_AGUA = "Textures/aguaOceanoTexture.png";
	const float INICIO_S = -520.0f;      // la nave aparece 520 unidades antes de donde empieza la ola
	const float RADIO_CURVA = 45.0f;     // el agua pasa de horizontal a vertical en esta curva
	const float ALTO_PARED = 110.0f;     // pared vertical de agua
	const float RADIO_CRESTA = 10.0f;    // la cresta se enrosca hacia atrás
	const float BAJADA_ATRAS = 140.0f;   // del otro lado baja hasta el nivel del mar
	const float ANCHO_OLA = 5000.0f;     // de horizonte a horizonte
	const float MITAD_MAR = 3000.0f;     // el mar plano (sigue a la cámara)
	const float MOSAICO = 60.0f;         // unidades por repetición de la textura del agua
	const float LIMITE_X = 1500.0f, LIMITE_ATRAS = -1800.0f;

	const float S_CURVA = RADIO_CURVA * PI_F * 0.5f;              // fin de la curva
	const float S_PARED = S_CURVA + ALTO_PARED;                    // fin de la pared
	const float S_CIMA = S_PARED + RADIO_CRESTA * PI_F * 0.5f;     // punto más alto
	const float S_CRESTA = S_PARED + RADIO_CRESTA * PI_F;          // fin de la cresta
	const float Z_PARED = RADIO_CURVA;
	const float ARRIBA_PARED = RADIO_CURVA + ALTO_PARED;

	Model planeta;
	bool hayPlaneta = false;
	Texture texPlaneta, texAgua;
	bool hayTexturaAgua = false;
	Material matPlaneta(4.0f, 256.0f);   // muy brillante: el reflejo del sol como en la foto
	Material matAgua(1.2f, 64.0f);
	MeshModel* mar = nullptr;
	MeshModel* ola = nullptr;
	bool inicializado = false;

	bool existe(const char* ruta)
	{
		std::ifstream f(ruta);
		return f.good();
	}

	// Perfil de la ola en el plano (z, y): punto, tangente (hacia donde crece s) y normal (hacia el aire)
	void perfil(float s, glm::vec2& p, glm::vec2& tangente, glm::vec2& normal)
	{
		if (s <= 0.0f)
		{
			p = glm::vec2(s, 0.0f); tangente = glm::vec2(1.0f, 0.0f); normal = glm::vec2(0.0f, 1.0f);
		}
		else if (s <= S_CURVA)
		{
			float a = s / RADIO_CURVA;   // cuarto de círculo: de horizontal a vertical
			p = glm::vec2(RADIO_CURVA * sinf(a), RADIO_CURVA - RADIO_CURVA * cosf(a));
			tangente = glm::vec2(cosf(a), sinf(a)); normal = glm::vec2(-sinf(a), cosf(a));
		}
		else if (s <= S_PARED)
		{
			p = glm::vec2(Z_PARED, RADIO_CURVA + (s - S_CURVA));
			tangente = glm::vec2(0.0f, 1.0f); normal = glm::vec2(-1.0f, 0.0f);
		}
		else
		{
			float b = fminf((s - S_PARED) / RADIO_CRESTA, PI_F);   // la cresta se enrosca hacia atrás
			p = glm::vec2(Z_PARED + RADIO_CRESTA - RADIO_CRESTA * cosf(b), ARRIBA_PARED + RADIO_CRESTA * sinf(b));
			tangente = glm::vec2(sinf(b), cosf(b)); normal = glm::vec2(-cosf(b), sinf(b));
		}
	}

	glm::vec3 en3D(const glm::vec2& v) { return glm::vec3(0.0f, v.y, v.x); }   // (z, y) -> (0, y, z)

	void agregar(std::vector<GLfloat>& v, const glm::vec3& p, float s, float t, const glm::vec3& n)
	{
		GLfloat d[] = { p.x, p.y, p.z, s, t, -n.x, -n.y, -n.z };   // normales invertidas, como en el curso
		v.insert(v.end(), d, d + 8);
	}

	// La ola: el perfil (curva, pared, cresta y bajada del otro lado) extruido a lo ancho
	void construirOla()
	{
		std::vector<glm::vec2> puntos, normales;
		for (int k = 0; k <= 70; k++)
		{
			float s = S_CRESTA * k / 70.0f;
			glm::vec2 p, t, n;
			perfil(s, p, t, n);
			puntos.push_back(p);
			normales.push_back(n);
		}
		glm::vec2 inicioBajada = puntos.back();
		for (int k = 1; k <= 24; k++)
		{
			float u = k / 24.0f;   // bajada suave (medio coseno) del otro lado
			float y = ARRIBA_PARED * (0.5f + 0.5f * cosf(PI_F * u));
			float dy = -ARRIBA_PARED * 0.5f * PI_F * sinf(PI_F * u) / BAJADA_ATRAS;
			puntos.push_back(glm::vec2(inicioBajada.x + BAJADA_ATRAS * u, y));
			normales.push_back(glm::normalize(glm::vec2(-dy, 1.0f)));
		}
		std::vector<float> largo(puntos.size(), 0.0f);
		for (size_t i = 1; i < puntos.size(); i++)
			largo[i] = largo[i - 1] + glm::length(puntos[i] - puntos[i - 1]);

		const int columnas = 100;
		std::vector<GLfloat> v;
		std::vector<unsigned int> idx;
		for (int c = 0; c <= columnas; c++)
		{
			float x = -0.5f * ANCHO_OLA + ANCHO_OLA * c / columnas;
			for (size_t i = 0; i < puntos.size(); i++)
				agregar(v, glm::vec3(x, puntos[i].y, puntos[i].x), x / MOSAICO, largo[i] / MOSAICO, en3D(normales[i]));
		}
		unsigned int filas = (unsigned int)puntos.size();
		for (int c = 0; c < columnas; c++)
			for (unsigned int i = 0; i + 1 < filas; i++)
			{
				unsigned int a = c * filas + i, b = a + 1, d = a + filas, e = d + 1;
				unsigned int t[] = { a, d, b, b, d, e };
				idx.insert(idx.end(), t, t + 6);
			}
		ola = new MeshModel();
		ola->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());
	}

	// El mar plano: un cuadro enorme con la textura repetida (se dibuja centrado bajo la cámara)
	void construirMar()
	{
		std::vector<GLfloat> v;
		float m = MITAD_MAR;
		glm::vec3 arriba(0.0f, 1.0f, 0.0f);
		agregar(v, glm::vec3(-m, 0.0f, -m), -m / MOSAICO, -m / MOSAICO, arriba);
		agregar(v, glm::vec3(m, 0.0f, -m), m / MOSAICO, -m / MOSAICO, arriba);
		agregar(v, glm::vec3(m, 0.0f, m), m / MOSAICO, m / MOSAICO, arriba);
		agregar(v, glm::vec3(-m, 0.0f, m), -m / MOSAICO, m / MOSAICO, arriba);
		unsigned int idx[] = { 0, 1, 2, 0, 2, 3 };
		mar = new MeshModel();
		mar->CreateMeshModel(v.data(), idx, 32, 6);
	}
}

// =============================================================================================
void InicializarOceano()
{
	if (inicializado)
		return;
	texPlaneta = Texture(RUTA_TEXTURA_PLANETA);
	texPlaneta.LoadTextureA();
	if (existe(RUTA_PLANETA))
	{
		planeta.LoadModel(RUTA_PLANETA);
		hayPlaneta = planeta.estaCargado();
		if (hayPlaneta && !planeta.usaTexturaPropia())
			planeta.SetTexturaRespaldo(&texPlaneta);
	}
	printf(hayPlaneta ? "[OK] Planeta oceano: %s\n" : "[ERROR] No se encontro %s\n", RUTA_PLANETA);
	texAgua = Texture(RUTA_TEXTURA_AGUA);
	hayTexturaAgua = texAgua.LoadTextureA();
	if (!hayTexturaAgua)
	{
		texAgua = Texture("Textures/plain.png");
		texAgua.LoadTextureA();
	}
	construirMar();
	construirOla();
	inicializado = true;
	printf("[OK] Oceano por dentro: ola de %.0f de alto a %.0f unidades de donde se aparece\n", ARRIBA_PARED + RADIO_CRESTA, -INICIO_S);
}

glm::vec3 CentroPlanetaOceano() { return CENTRO; }
float RadioAtmosferaOceano() { return ATMOSFERA; }

void DibujarPlanetaOceano(float tiempo, const UniformsEspacio& u)
{
	if (!hayPlaneta)
		return;
	glm::mat4 base = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / planeta.getRadio()))
		* glm::translate(glm::mat4(1.0f), -planeta.getCentro());
	glm::mat4 m = glm::translate(glm::mat4(1.0f), CENTRO)
		* glm::rotate(glm::mat4(1.0f), 0.05f * tiempo, glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(RADIO)) * base;
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	matPlaneta.UseMaterial(u.especular, u.brillo);
	glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
	planeta.RenderModel();

	// halo de atmósfera: tres capas que suman luz azul (más intensa en la orilla)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glUniform3f(u.color, 0.0f, 0.0f, 0.0f);
	const float capas[3] = { 1.03f, 1.07f, 1.12f };
	const float fuerza[3] = { 0.55f, 0.35f, 0.18f };
	for (int k = 0; k < 3; k++)
	{
		glm::mat4 h = glm::translate(glm::mat4(1.0f), CENTRO) * glm::scale(glm::mat4(1.0f), glm::vec3(RADIO * capas[k])) * base;
		glUniform3f(u.emision, 0.25f * fuerza[k], 0.55f * fuerza[k], 1.2f * fuerza[k]);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(h));
		planeta.RenderModel();
	}
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

AmbienteOceano ObtenerAmbienteOceano(float factorDia)
{
	// cielo despejado azul claro, como en la foto; de noche se oscurece
	AmbienteOceano a;
	float f = 0.25f + 0.75f * glm::clamp(factorDia, 0.0f, 1.0f);
	glm::vec3 horizonte(0.64f, 0.76f, 0.85f), cenit(0.55f, 0.71f, 0.84f);
	a.colorNiebla = horizonte * f;
	a.densidadNiebla = 0.0007f;            // el mar se pierde en el horizonte
	a.tinteCielo = glm::vec3(1.0f);
	a.brumaHorizonte = glm::vec4(horizonte * f, 1.0f);
	a.colorCenit = cenit * f;
	a.cieloSolido = 1.0f;
	return a;
}

void InicioOceano(float& x, float& s, float& yawGrados)
{
	x = 0.0f;
	s = INICIO_S;
	yawGrados = -90.0f;   // de espaldas a la ola: se ve el océano abierto; la ola está atrás
}

void PuntoOla(float x, float s, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba)
{
	glm::vec2 p, t, n;
	perfil(s, p, t, n);
	arriba = en3D(n);
	adelante = en3D(t);
	posicion = glm::vec3(x, p.y, p.x) + arriba * altura;
}

glm::vec2 LimitarOceano(float x, float s)
{
	return glm::vec2(glm::clamp(x, -LIMITE_X, LIMITE_X), fmaxf(s, LIMITE_ATRAS));
}

glm::vec3 AjustarCamaraOceano(const glm::vec3& camara)
{
	glm::vec3 c = camara;
	if (c.y <= RADIO_CURVA)
	{
		// frente a la curva: no pasar del agua
		float cosA = (RADIO_CURVA - fmaxf(c.y, 0.0f)) / RADIO_CURVA;
		float zSuperficie = RADIO_CURVA * sqrtf(fmaxf(0.0f, 1.0f - cosA * cosA));
		if (c.z > zSuperficie - 2.0f) c.z = zSuperficie - 2.0f;
	}
	else if (c.y <= ARRIBA_PARED)
	{
		if (c.z > Z_PARED - 2.0f) c.z = Z_PARED - 2.0f;   // frente a la pared
	}
	if (c.z < Z_PARED && c.y < 1.5f)
		c.y = 1.5f;   // sobre el mar plano
	return c;
}

bool LlegoALaCresta(const glm::vec3& p)
{
	return p.y >= ARRIBA_PARED + 0.8f * RADIO_CRESTA && p.z >= Z_PARED - 6.0f && p.z <= Z_PARED + 2.0f * RADIO_CRESTA + 6.0f;
}

float LargoCarreraOceano()
{
	return S_CIMA - INICIO_S;
}

void PuntoCarreraOceano(float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba)
{
	PuntoOla(carril, INICIO_S + distancia, altura, posicion, adelante, arriba);
}

void DibujarOceano(const glm::vec3& posCamara, const UniformsEspacio& u)
{
	if (!inicializado)
		return;
	if (hayTexturaAgua) glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	else glUniform3f(u.color, 0.12f, 0.46f, 0.68f);
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	matAgua.UseMaterial(u.especular, u.brillo);
	texAgua.UseTexture();
	// el mar sigue a la cámara en pasos del tamaño de la textura (así no "nada" y nunca se acaba)
	float sx = floorf(posCamara.x / MOSAICO) * MOSAICO, sz = floorf(posCamara.z / MOSAICO) * MOSAICO;
	glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(sx, -0.05f, sz));   // apenas abajo del inicio de la ola
	glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
	mar->RenderMeshModel();
	glm::mat4 identidad(1.0f);
	glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(identidad));
	ola->RenderMeshModel();
}
