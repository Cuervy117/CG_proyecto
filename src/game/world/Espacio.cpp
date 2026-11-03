#include "Espacio.h"

#include <cmath>
#include <cstdio>
#include <vector>
#include <fstream>
#include <algorithm>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Model.h"
#include "Texture.h"
#include "Mesh_tn.h"
#include "Material.h"
#include "Oceano.h"

namespace   // todo lo de este archivo es interno
{
	// =========================================================================================
	// Pista. Planetas: rocoso en (0, 0, 0) con radio 14 (atmósfera 22) y selvático en
	// (120, 10, 90) con radio 16 (atmósfera 24). Cada tramo empieza y termina DENTRO de un
	// planeta: así el camino "entra" a su atmósfera y la carrera sigue en su superficie.
	// =========================================================================================
	const glm::vec3 CONTROL_TRAMO_A[] = {          // sale del océano -> meta -> entra al rocoso
		{ 138.0f, -30.0f, -46.0f },  //    dentro del planeta océano (sale por su lado cercano)
		{ 120.0f, -28.0f, -56.0f },
		{ 92.0f, -18.0f, -60.0f },   //    SUBIDA hacia la meta
		{ 62.0f, -8.0f, -56.0f },
		{ 34.0f, 2.0f, -50.0f },
		{ 12.0f, 6.0f, -46.0f },
		{ -6.0f, 6.0f, -44.0f },     // 6  META
		{ -30.0f, 2.0f, -40.0f },
		{ -42.0f, -8.0f, -22.0f },   //    BAJADA para pasar por debajo del rocoso
		{ -32.0f, -22.0f, -8.0f },
		{ -14.0f, -22.0f, -2.0f },
		{ -4.0f, -10.0f, -1.0f },    //    dentro del planeta rocoso (entra por abajo)
	};
	const int INDICE_META = 6;
	const glm::vec3 CONTROL_TRAMO_B[] = {          // sale del rocoso -> cinturón -> entra al selvático
		{ 6.0f, 9.0f, 5.0f },        //    dentro del planeta rocoso (sale por su lado superior)
		{ 18.0f, 16.0f, 12.0f },
		{ 34.0f, 18.0f, 28.0f },
		{ 52.0f, 16.0f, 46.0f },     //    cruza el cinturón de asteroides
		{ 72.0f, 26.0f, 66.0f },     //    SUBIDA
		{ 92.0f, 40.0f, 82.0f },
		{ 110.0f, 48.0f, 90.0f },    //    sobre el planeta selvático
		{ 118.0f, 36.0f, 90.0f },
		{ 120.0f, 22.0f, 90.0f },    //    dentro del planeta selvático (entra por arriba)
	};
	const glm::vec3 CONTROL_TRAMO_C[] = {          // sale del selvático -> entra al océano
		{ 114.0f, 0.0f, 80.0f },     //    dentro del planeta selvático (sale por abajo)
		{ 110.0f, -10.0f, 66.0f },
		{ 126.0f, -22.0f, 44.0f },   //    BAJADA
		{ 148.0f, -26.0f, 18.0f },
		{ 158.0f, -20.0f, -2.0f },   //    rodea por arriba al planeta océano...
		{ 154.0f, -16.0f, -18.0f },
		{ 150.0f, -22.0f, -32.0f },  //    ...y entra (dentro del planeta océano)
	};
	const int NUM_TRAMOS = 3;
	const glm::vec3 CENTRO_ROCOSO(0.0f, 0.0f, 0.0f), CENTRO_SELVATICO(120.0f, 10.0f, 90.0f);
	const float ATMOSFERA_ROCOSO = 22.0f, ATMOSFERA_SELVATICO = 24.0f;

	const char* RUTA_PISTA = "Models/pista/pista.obj";
	const char* RUTA_TEXTURA_PISTA = "Textures/pistaTexture.png";
	const float ANCHO_PISTA = 10.0f;          // ancho del camino
	const float GROSOR_PISTA = 1.2f;          // la pista flota en el espacio: se le da grosor
	const float MOSAICO_PISTA = 8.0f;         // unidades de pista por cada repetición de la textura
	const float PASO_LUCES = 4.0f;
	const glm::vec3 COLOR_LUCES(0.35f, 0.85f, 1.0f);

	const float VELOCIDAD_NAVE = 18.0f;
	const float TIEMPO_DENTRO_PLANETA = 7.0f; // segundos que la nave corre dentro de cada planeta
	const float CARRIL_RIVAL = -2.5f;         // la nave rival va por el carril izquierdo

	// Base de inicio: plataforma circular bajo la pista, antes de la meta
	const char* RUTA_TEXTURA_BASE = "Textures/baseTexture.png";
	const float RADIO_BASE = 15.0f;
	const float GROSOR_BASE = 1.8f;
	const float BASE_BAJO_PISTA = 16.0f;      // qué tan abajo del camino queda el piso de la base
	const float BASE_ANTES_DE_META = 34.0f;   // a cuántas unidades antes de la meta

	// =========================================================================================
	// Cinturón de asteroides (los dos modelos vienen en asteroide.zip)
	// =========================================================================================
	const char* RUTA_ASTEROIDE_GRANDE = "Models/asteroide/asteroide.obj";
	const char* RUTA_ASTEROIDE_CHICO = "Models/asteroide/Rocky_Asteroid_6.obj";
	const char* RUTA_TEXTURA_ASTEROIDE = "Textures/asteroideTexture.png";
	const int CANTIDAD_ASTEROIDES = 150;
	const int MAX_ASTEROIDES_GRANDES = 6;     // asteroide.obj pesa 500 mil triángulos: solo unos cuantos
	const int TRIANGULOS_PARA_USARLO_EN_TODOS = 30000;   // si lo reduces en Blender, se usa en todos
	const float DISTANCIA_CINTURON = 420.0f;
	const float PI_F = 3.14159265f;

	struct Tramo
	{
		std::vector<glm::vec3> puntos;   // un punto cada 1 unidad
		std::vector<float> acumulado;    // distancia recorrida hasta cada punto
		float largo = 0.0f;
	};

	struct Asteroide
	{
		glm::vec3 base, estiramiento, eje;
		float escala, giro, fase;
		bool grande;
	};

	struct AleatorioEspacio
	{
		unsigned int estado;
		explicit AleatorioEspacio(unsigned int semilla) : estado(semilla) {}
		float uniforme()
		{
			estado ^= estado << 13; estado ^= estado >> 17; estado ^= estado << 5;
			return (float)(estado >> 8) * (1.0f / 16777216.0f);
		}
		float rango(float a, float b) { return a + (b - a) * uniforme(); }
	};

	Tramo tramos[NUM_TRAMOS];                 // 0: océano -> meta -> rocoso, 1: rocoso -> selvático, 2: selvático -> océano
	float sMeta = 0.0f;                       // dónde está la meta dentro del tramo 0
	std::vector<Asteroide> asteroides;
	glm::vec3 direccionCinturon(1.0f, 0.0f, 0.0f);

	Model modeloPista;
	bool hayModeloPista = false;
	MeshModel* mallaPistaCodigo[NUM_TRAMOS] = { nullptr, nullptr, nullptr };   // respaldo si falta pista.obj
	Texture texPista, texAsteroide, texPlain;
	Model asteroideGrande, asteroideChico;
	bool hayGrande = false, hayChico = false, grandeEnTodos = false;
	MeshModel* quadMarca = nullptr;
	MeshModel* mallaBase = nullptr;
	MeshModel* mallaPilares = nullptr;
	Texture texBase;
	glm::vec3 centroBase(0.0f), adelanteBase(0.0f, 0.0f, 1.0f), derechaBase(1.0f, 0.0f, 0.0f);
	Material matPista(0.3f, 8.0f);
	Material matMarca(0.2f, 4.0f);
	Material matAsteroide(0.4f, 16.0f);
	bool inicializado = false;

	bool existe(const char* ruta)
	{
		std::ifstream f(ruta);
		return f.good();
	}

	glm::vec3 catmullRom(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec3& p3, float t)
	{
		float t2 = t * t, t3 = t2 * t;
		return 0.5f * ((2.0f * p1) + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2
			+ (3.0f * p1 - p0 - 3.0f * p2 + p3) * t3);
	}

	// Curva abierta por los puntos de control, remuestreada cada 1 unidad.
	// 'indiceMarca' (opcional) devuelve a qué distancia pasa ese punto de control.
	void construirTramo(Tramo& tramo, const glm::vec3* control, int n, int indiceMarca, float* sMarca)
	{
		std::vector<glm::vec3> denso;
		std::vector<int> indiceDe;
		for (int i = 0; i + 1 < n; i++)
		{
			glm::vec3 p0 = (i == 0) ? 2.0f * control[0] - control[1] : control[i - 1];
			glm::vec3 p3 = (i + 2 >= n) ? 2.0f * control[n - 1] - control[n - 2] : control[i + 2];
			for (int k = 0; k < 40; k++)
			{
				denso.push_back(catmullRom(p0, control[i], control[i + 1], p3, k / 40.0f));
				indiceDe.push_back(k == 0 ? i : -1);
			}
		}
		denso.push_back(control[n - 1]);
		indiceDe.push_back(n - 1);

		tramo.puntos.assign(1, denso[0]);
		tramo.acumulado.assign(1, 0.0f);
		float restante = 1.0f, total = 0.0f;
		for (size_t i = 0; i + 1 < denso.size(); i++)
		{
			if (sMarca && indiceDe[i] == indiceMarca) *sMarca = total;
			glm::vec3 a = denso[i], b = denso[i + 1];
			float largo = glm::length(b - a), recorrido = 0.0f;
			while (largo - recorrido >= restante)
			{
				recorrido += restante;
				tramo.puntos.push_back(a + (b - a) * (recorrido / largo));
				tramo.acumulado.push_back(total + recorrido);
				restante = 1.0f;
			}
			restante -= (largo - recorrido);
			total += largo;
		}
		tramo.largo = total;
	}

	glm::vec3 puntoTramo(const Tramo& tramo, float s, glm::vec3* tangente)
	{
		s = glm::clamp(s, 0.0f, tramo.largo);
		size_t i = (size_t)(std::upper_bound(tramo.acumulado.begin(), tramo.acumulado.end(), s) - tramo.acumulado.begin());
		i = (i == 0) ? 0 : i - 1;
		if (i + 1 >= tramo.puntos.size()) i = tramo.puntos.size() - 2;
		float t = (s - tramo.acumulado[i]) / fmaxf(tramo.acumulado[i + 1] - tramo.acumulado[i], 1.0e-6f);
		if (tangente) *tangente = glm::normalize(tramo.puntos[i + 1] - tramo.puntos[i]);
		return tramo.puntos[i] + (tramo.puntos[i + 1] - tramo.puntos[i]) * glm::clamp(t, 0.0f, 1.0f);
	}

	// Marco de la pista en un punto: adelante, arriba (hacia el cielo) y derecha
	void marcoTramo(const Tramo& tramo, float s, glm::vec3& p, glm::vec3& adelante, glm::vec3& arriba, glm::vec3& derecha)
	{
		p = puntoTramo(tramo, s, &adelante);
		glm::vec3 mundoArriba(0.0f, 1.0f, 0.0f);
		arriba = mundoArriba - adelante * glm::dot(mundoArriba, adelante);
		arriba = (glm::length(arriba) < 1.0e-3f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::normalize(arriba);
		derecha = glm::normalize(glm::cross(adelante, arriba));
	}

	// Malla del camino de un tramo: cara superior con la textura de asfalto, dos costados y fondo.
	// La misma función genera pista.obj (con la herramienta) y el respaldo si el archivo no está.
	void mallaTramo(const Tramo& tramo, std::vector<GLfloat>& v, std::vector<unsigned int>& indices, bool normalesInvertidas)
	{
		float signo = normalesInvertidas ? -1.0f : 1.0f;   // este motor invierte las normales al cargar modelos
		for (size_t i = 0; i < tramo.puntos.size(); i++)
		{
			glm::vec3 p, adelante, arriba, derecha;
			marcoTramo(tramo, tramo.acumulado[i], p, adelante, arriba, derecha);
			float coordV = tramo.acumulado[i] / MOSAICO_PISTA;
			glm::vec3 izq = p - derecha * (0.5f * ANCHO_PISTA), der = p + derecha * (0.5f * ANCHO_PISTA);
			glm::vec3 abajo = -arriba * GROSOR_PISTA;
			// (posición, u, normal) de las 8 esquinas de la sección: arriba, costado derecho, fondo, costado izquierdo
			struct E { glm::vec3 q; float u; glm::vec3 n; };
			E esquinas[8] = {
				{ izq, 0.0f, arriba }, { der, 1.0f, arriba },
				{ der, 0.98f, derecha }, { der + abajo, 1.0f, derecha },
				{ der + abajo, 0.5f, -arriba }, { izq + abajo, 0.5f, -arriba },
				{ izq + abajo, 0.0f, -derecha }, { izq, 0.02f, -derecha } };
			for (int k = 0; k < 8; k++)
			{
				GLfloat d[] = { esquinas[k].q.x, esquinas[k].q.y, esquinas[k].q.z, esquinas[k].u, coordV,
					signo * esquinas[k].n.x, signo * esquinas[k].n.y, signo * esquinas[k].n.z };
				v.insert(v.end(), d, d + 8);
			}
			if (i + 1 < tramo.puntos.size())
			{
				unsigned int a = (unsigned int)(i * 8), b = a + 8;
				for (int cara = 0; cara < 4; cara++)
				{
					unsigned int k0 = a + cara * 2, k1 = k0 + 1, m0 = b + cara * 2, m1 = m0 + 1;
					unsigned int tri[] = { k0, m0, k1, k1, m0, m1 };
					indices.insert(indices.end(), tri, tri + 6);
				}
			}
		}
	}

	// Plataforma (cilindro de 40 lados) y dos pilares que la cuelgan del camino. Las normales van
	// invertidas, como en las mallas creadas con código del curso.
	void agregarVertice(std::vector<GLfloat>& v, const glm::vec3& p, float s, float t, const glm::vec3& n)
	{
		GLfloat d[] = { p.x, p.y, p.z, s, t, -n.x, -n.y, -n.z };
		v.insert(v.end(), d, d + 8);
	}

	void construirBase()
	{
		glm::vec3 p, adelante, arriba, derecha;
		float sBase = fmaxf(sMeta - BASE_ANTES_DE_META, 0.0f);
		marcoTramo(tramos[0], sBase, p, adelante, arriba, derecha);
		adelanteBase = glm::normalize(glm::vec3(adelante.x, 0.0f, adelante.z));
		derechaBase = glm::normalize(glm::cross(adelanteBase, glm::vec3(0.0f, 1.0f, 0.0f)));
		centroBase = p - glm::vec3(0.0f, BASE_BAJO_PISTA, 0.0f);

		const int lados = 40;
		const glm::vec3 Y(0.0f, 1.0f, 0.0f);
		std::vector<GLfloat> v;
		std::vector<unsigned int> idx;
		auto punto = [&](float ang, float r, float y) {
			return centroBase + derechaBase * (r * cosf(ang)) + adelanteBase * (r * sinf(ang)) + Y * y; };
		// tapa superior: abanico con textura de pista de aterrizaje (u, v planas)
		agregarVertice(v, centroBase, 0.5f, 0.5f, Y);
		for (int k = 0; k <= lados; k++)
		{
			float a = 2.0f * PI_F * k / lados;
			agregarVertice(v, punto(a, RADIO_BASE, 0.0f), 0.5f + 0.5f * cosf(a), 0.5f + 0.5f * sinf(a), Y);
		}
		for (int k = 0; k < lados; k++) { unsigned int t[] = { 0u, (unsigned)(1 + k), (unsigned)(2 + k) }; idx.insert(idx.end(), t, t + 3); }
		// costado: toma el color de la franja de seguridad del borde de la textura
		unsigned int inicio = (unsigned int)(v.size() / 8);
		for (int k = 0; k <= lados; k++)
		{
			float a = 2.0f * PI_F * k / lados;
			glm::vec3 radial = derechaBase * cosf(a) + adelanteBase * sinf(a);
			float s = 0.5f + 0.47f * cosf(a), t = 0.5f + 0.47f * sinf(a);
			agregarVertice(v, punto(a, RADIO_BASE, 0.0f), s, t, radial);
			agregarVertice(v, punto(a, RADIO_BASE * 0.9f, -GROSOR_BASE), s, t, radial);
		}
		for (int k = 0; k < lados; k++)
		{
			unsigned int a0 = inicio + 2 * k, a1 = a0 + 1, b0 = a0 + 2, b1 = a0 + 3;
			unsigned int t[] = { a0, a1, b0, b0, a1, b1 };
			idx.insert(idx.end(), t, t + 6);
		}
		// fondo
		unsigned int centroFondo = (unsigned int)(v.size() / 8);
		agregarVertice(v, centroBase - Y * GROSOR_BASE, 0.5f, 0.5f, -Y);
		for (int k = 0; k <= lados; k++)
		{
			float a = 2.0f * PI_F * k / lados;
			agregarVertice(v, punto(a, RADIO_BASE * 0.9f, -GROSOR_BASE), 0.5f + 0.2f * cosf(a), 0.5f + 0.2f * sinf(a), -Y);
		}
		for (int k = 0; k < lados; k++) { unsigned int t[] = { centroFondo, centroFondo + 2 + k, centroFondo + 1 + k }; idx.insert(idx.end(), t, t + 3); }
		mallaBase = new MeshModel();
		mallaBase->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());

		// dos pilares de la plataforma hasta la parte de abajo del camino
		std::vector<GLfloat> vp;
		std::vector<unsigned int> ip;
		for (int lado = -1; lado <= 1; lado += 2)
		{
			glm::vec3 q, a2, ar2, de2;
			marcoTramo(tramos[0], sBase + 9.0f * (float)lado, q, a2, ar2, de2);
			glm::vec3 arribaPilar = q - ar2 * (GROSOR_PISTA + 0.05f);
			glm::vec3 abajoPilar(arribaPilar.x, centroBase.y, arribaPilar.z);
			unsigned int primero = (unsigned int)(vp.size() / 8);
			for (int k = 0; k <= 10; k++)
			{
				float a = 2.0f * PI_F * k / 10.0f;
				glm::vec3 radial(cosf(a), 0.0f, sinf(a));
				agregarVertice(vp, abajoPilar + radial * 0.9f, 0.5f, 0.12f, radial);
				agregarVertice(vp, arribaPilar + radial * 0.6f, 0.5f, 0.14f, radial);
			}
			for (int k = 0; k < 10; k++)
			{
				unsigned int a0 = primero + 2 * k, a1 = a0 + 1, b0 = a0 + 2, b1 = a0 + 3;
				unsigned int t[] = { a0, a1, b0, b0, a1, b1 };
				ip.insert(ip.end(), t, t + 6);
			}
		}
		mallaPilares = new MeshModel();
		mallaPilares->CreateMeshModel(vp.data(), ip.data(), (unsigned int)vp.size(), (unsigned int)ip.size());
	}

	float distanciaAPista(const glm::vec3& q)
	{
		float mejor = 1.0e30f;
		for (int t = 0; t < NUM_TRAMOS; t++)
			for (size_t i = 0; i < tramos[t].puntos.size(); i += 2)
				mejor = fminf(mejor, glm::length(tramos[t].puntos[i] - q));
		return mejor;
	}

	void generarCinturon()
	{
		AleatorioEspacio rnd(4242u);
		glm::vec3 centro = (CENTRO_ROCOSO + CENTRO_SELVATICO) * 0.5f;
		glm::vec3 entrePlanetas = glm::normalize(CENTRO_SELVATICO - CENTRO_ROCOSO);
		glm::vec3 arriba(0.0f, 1.0f, 0.0f);
		direccionCinturon = glm::normalize(glm::cross(entrePlanetas, arriba));
		int grandes = 0;
		asteroides.clear();
		for (int intento = 0; intento < 2000 && (int)asteroides.size() < CANTIDAD_ASTEROIDES; intento++)
		{
			float u = rnd.rango(-1.0f, 1.0f);
			float v = (rnd.uniforme() + rnd.uniforme() + rnd.uniforme()) / 1.5f - 1.0f;
			float h = (rnd.uniforme() + rnd.uniforme()) - 1.0f;
			glm::vec3 p = centro + direccionCinturon * (58.0f * u) + entrePlanetas * (10.0f * v) + arriba * (8.0f * h);
			float tamano = 0.5f + 3.6f * powf(rnd.uniforme(), 3.0f);
			if (distanciaAPista(p) < tamano + 8.0f)
				continue;   // deja libre el paso de la pista
			Asteroide a;
			a.base = p;
			a.escala = tamano;
			a.estiramiento = glm::vec3(1.0f, rnd.rango(0.7f, 1.1f), rnd.rango(0.8f, 1.2f));
			a.eje = glm::normalize(glm::vec3(rnd.rango(-1.0f, 1.0f), rnd.rango(-1.0f, 1.0f), rnd.rango(-1.0f, 1.0f)) + glm::vec3(0.0f, 0.01f, 0.0f));
			a.giro = rnd.rango(0.15f, 0.7f) * (rnd.uniforme() < 0.5f ? -1.0f : 1.0f);
			a.fase = rnd.rango(0.0f, 2.0f * PI_F);
			a.grande = (tamano > 2.6f && grandes < MAX_ASTEROIDES_GRANDES);
			if (a.grande) grandes++;
			asteroides.push_back(a);
		}
	}

	glm::mat4 normalizar(const Model& modelo)
	{
		return glm::scale(glm::mat4(1.0f), glm::vec3(1.0f / modelo.getRadio()))
			* glm::translate(glm::mat4(1.0f), -modelo.getCentro());
	}

	// Un cuadro brillante (luz, cuadro de la meta o tramo de arco) en el marco dado
	void dibujarMarca(const glm::vec3& centro, const glm::vec3& ejeX, const glm::vec3& ejeY, const glm::vec3& normal,
		float ancho, float largo, const glm::vec3& color, const UniformsEspacio& u)
	{
		glm::mat4 m(1.0f);
		m[0] = glm::vec4(ejeX, 0.0f);
		m[1] = glm::vec4(ejeY, 0.0f);
		m[2] = glm::vec4(normal, 0.0f);
		m[3] = glm::vec4(centro, 1.0f);
		m = m * glm::scale(glm::mat4(1.0f), glm::vec3(ancho, largo, 1.0f));
		glUniform3f(u.color, color.x * 0.3f, color.y * 0.3f, color.z * 0.3f);
		glUniform3f(u.emision, color.x, color.y, color.z);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
		quadMarca->RenderMeshModel();
	}

	// Distancia (dentro del tramo) donde el camino cruza la atmósfera de un planeta
	float sDistanciaAPlaneta(const Tramo& tramo, const glm::vec3& centro, float distancia, bool desdeElFinal)
	{
		if (desdeElFinal)
		{
			for (int i = (int)tramo.puntos.size() - 1; i >= 0; i--)
				if (glm::length(tramo.puntos[i] - centro) >= distancia) return tramo.acumulado[i];
			return 0.0f;
		}
		for (size_t i = 0; i < tramo.puntos.size(); i++)
			if (glm::length(tramo.puntos[i] - centro) >= distancia) return tramo.acumulado[i];
		return tramo.largo;
	}

	void puntoDeAparicion(const Tramo& tramo, float s, float alturaOjos, glm::vec3& posicion, float& yaw, float& pitch)
	{
		glm::vec3 p, adelante, arriba, derecha;
		marcoTramo(tramo, s, p, adelante, arriba, derecha);
		posicion = p + arriba * alturaOjos;
		yaw = glm::degrees(atan2f(adelante.z, adelante.x));
		pitch = glm::clamp(glm::degrees(asinf(glm::clamp(adelante.y, -1.0f, 1.0f))), -30.0f, 30.0f);
	}
}

// =============================================================================================
// API
// =============================================================================================
void InicializarEspacio()
{
	if (inicializado)
		return;
	construirTramo(tramos[0], CONTROL_TRAMO_A, (int)(sizeof(CONTROL_TRAMO_A) / sizeof(CONTROL_TRAMO_A[0])), INDICE_META, &sMeta);
	construirTramo(tramos[1], CONTROL_TRAMO_B, (int)(sizeof(CONTROL_TRAMO_B) / sizeof(CONTROL_TRAMO_B[0])), -1, nullptr);
	construirTramo(tramos[2], CONTROL_TRAMO_C, (int)(sizeof(CONTROL_TRAMO_C) / sizeof(CONTROL_TRAMO_C[0])), -1, nullptr);
	printf("[OK] Pista: tramos de %.0f (meta en %.0f), %.0f y %.0f unidades\n", tramos[0].largo, sMeta, tramos[1].largo, tramos[2].largo);

	// la pista es un modelo .obj (generado con las mismas curvas); si no está, se arma aquí
	texPista = Texture(RUTA_TEXTURA_PISTA);
	if (!texPista.LoadTextureA())
	{
		texPista = Texture("Textures/plain.png");
		texPista.LoadTextureA();
	}
	if (existe(RUTA_PISTA))
	{
		modeloPista.LoadModel(RUTA_PISTA);
		hayModeloPista = modeloPista.estaCargado();
		if (hayModeloPista && !modeloPista.usaTexturaPropia())
			modeloPista.SetTexturaRespaldo(&texPista);
	}
	if (hayModeloPista)
		printf("[OK] Pista: %s (%d triangulos)\n", RUTA_PISTA, modeloPista.getNumTriangulos());
	else
	{
		printf("[AVISO] No se encontro %s: la pista se arma con codigo\n", RUTA_PISTA);
		for (int t = 0; t < NUM_TRAMOS; t++)
		{
			std::vector<GLfloat> v;
			std::vector<unsigned int> indices;
			mallaTramo(tramos[t], v, indices, true);
			mallaPistaCodigo[t] = new MeshModel();
			mallaPistaCodigo[t]->CreateMeshModel(v.data(), indices.data(), (unsigned int)v.size(), (unsigned int)indices.size());
		}
	}

	if (existe(RUTA_ASTEROIDE_GRANDE))
	{
		printf("Cargando %s ...\n", RUTA_ASTEROIDE_GRANDE);
		asteroideGrande.LoadModel(RUTA_ASTEROIDE_GRANDE);
		hayGrande = asteroideGrande.estaCargado();
	}
	if (existe(RUTA_ASTEROIDE_CHICO))
	{
		asteroideChico.LoadModel(RUTA_ASTEROIDE_CHICO);
		hayChico = asteroideChico.estaCargado();
	}
	texAsteroide = Texture(RUTA_TEXTURA_ASTEROIDE);
	if (!texAsteroide.LoadTextureA())
	{
		texAsteroide = Texture("Textures/plain.png");
		texAsteroide.LoadTextureA();
	}
	if (hayGrande && !asteroideGrande.usaTexturaPropia())
		asteroideGrande.SetTexturaRespaldo(&texAsteroide);
	grandeEnTodos = hayGrande && asteroideGrande.getNumTriangulos() <= TRIANGULOS_PARA_USARLO_EN_TODOS;
	texPlain = Texture("Textures/plain.png");
	texPlain.LoadTextureA();
	texBase = Texture(RUTA_TEXTURA_BASE);
	if (!texBase.LoadTextureA())
	{
		texBase = Texture("Textures/plain.png");
		texBase.LoadTextureA();
	}
	construirBase();
	printf("[OK] Base de inicio: plataforma de radio %.0f bajo la pista, %.0f unidades antes de la meta\n", RADIO_BASE, BASE_ANTES_DE_META);

	generarCinturon();
	printf("[OK] Cinturon de asteroides: %d asteroides (%s)\n", (int)asteroides.size(),
		grandeEnTodos ? "todos con asteroide.obj" : "grandes con asteroide.obj, chicos con Rocky_Asteroid_6.obj");

	GLfloat v[] = {
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, -1.0f,
	};
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	quadMarca = new MeshModel();
	quadMarca->CreateMeshModel(v, indices, 32, 6);
	inicializado = true;
}

void DibujarPista(float tiempo, const UniformsEspacio& u)
{
	if (!inicializado)
		return;
	// 1) el camino (modelo pista.obj con textura de asfalto)
	glm::mat4 identidad(1.0f);
	glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(identidad));
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	matPista.UseMaterial(u.especular, u.brillo);
	if (hayModeloPista)
		modeloPista.RenderModel();
	else
	{
		texPista.UseTexture();
		for (int t = 0; t < NUM_TRAMOS; t++) mallaPistaCodigo[t]->RenderMeshModel();
	}

	// 2) luces en las orillas, con ondas que corren por la pista:
	//    brillo = 0.35 + 0.65 * max(0, sen(2 pi (s/48 - 0.7 t)))^4
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	texPlain.UseTexture();
	matMarca.UseMaterial(u.especular, u.brillo);
	glm::vec3 p, adelante, arriba, derecha;
	for (int t = 0; t < NUM_TRAMOS; t++)
	{
		const Tramo& tramo = tramos[t];
		float desfase = 300.0f * (float)t;
		for (float s = 2.0f; s < tramo.largo - 2.0f; s += PASO_LUCES)
		{
			marcoTramo(tramo, s, p, adelante, arriba, derecha);
			float onda = fmaxf(0.0f, sinf(2.0f * PI_F * ((s + desfase) / 48.0f - 0.7f * tiempo)));
			glm::vec3 c = COLOR_LUCES * (0.35f + 0.65f * onda * onda * onda * onda);
			for (int lado = -1; lado <= 1; lado += 2)
				dibujarMarca(p + derecha * ((0.5f * ANCHO_PISTA - 0.35f) * (float)lado) + arriba * 0.08f,
					derecha, adelante, arriba, 0.3f, 1.4f, c, u);
		}
	}

	// 3) línea de meta: cuadros blancos y azules a lo ancho del camino
	marcoTramo(tramos[0], sMeta, p, adelante, arriba, derecha);
	for (int fila = 0; fila < 2; fila++)
		for (int k = 0; k < 10; k++)
		{
			bool claro = ((k + fila) % 2) == 0;
			glm::vec3 c = claro ? glm::vec3(1.0f) : glm::vec3(0.1f, 0.25f, 0.6f);
			dibujarMarca(p + derecha * (-0.5f * ANCHO_PISTA + (k + 0.5f) * ANCHO_PISTA / 10.0f) + adelante * ((float)fila - 0.5f) + arriba * 0.06f,
				derecha, adelante, arriba, ANCHO_PISTA / 10.0f, 1.0f, c, u);
		}

	// 4) arcos de luz donde el camino entra a la atmósfera de cada planeta (pulsan)
	struct Arco { int tramo; glm::vec3 centro; float radioAtmosfera; glm::vec3 color; };
	Arco arcos[3] = { { 0, CENTRO_ROCOSO, ATMOSFERA_ROCOSO, glm::vec3(0.75f, 0.45f, 1.0f) },
		{ 1, CENTRO_SELVATICO, ATMOSFERA_SELVATICO, glm::vec3(0.45f, 1.0f, 0.55f) },
		{ 2, CentroPlanetaOceano(), RadioAtmosferaOceano(), glm::vec3(0.35f, 0.7f, 1.0f) } };
	float pulso = 0.6f + 0.4f * sinf(3.0f * tiempo);
	for (const Arco& a : arcos)
	{
		float s = sDistanciaAPlaneta(tramos[a.tramo], a.centro, a.radioAtmosfera + 2.0f, true);
		marcoTramo(tramos[a.tramo], s, p, adelante, arriba, derecha);
		const int piezas = 18;
		float radio = 0.5f * ANCHO_PISTA + 1.0f;
		for (int k = 0; k < piezas; k++)
		{
			float ang = PI_F * (k + 0.5f) / piezas;   // de la orilla derecha a la izquierda, por arriba
			glm::vec3 radial = derecha * cosf(ang) + arriba * sinf(ang);
			glm::vec3 tangenteArco = -derecha * sinf(ang) + arriba * cosf(ang);
			dibujarMarca(p + radial * radio, tangenteArco, adelante, glm::normalize(glm::cross(tangenteArco, adelante)),
				PI_F * radio / piezas * 0.85f, 0.5f, a.color * pulso, u);
		}
	}

	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void DibujarCinturon(const glm::vec3& posCamara, const glm::vec3& dirCamara, float tiempo, const UniformsEspacio& u)
{
	if (!inicializado || (!hayGrande && !hayChico))
		return;
	matAsteroide.UseMaterial(u.especular, u.brillo);
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	glm::vec3 direccion = glm::normalize(dirCamara);
	for (const Asteroide& a : asteroides)
	{
		// cada asteroide gira sobre su eje (ángulo = giro * t + fase) y se mece a lo largo del cinturón
		glm::vec3 p = a.base + direccionCinturon * (1.2f * sinf(0.15f * tiempo + a.fase));
		glm::vec3 d = p - posCamara;
		float dist = glm::length(d);
		if (dist > DISTANCIA_CINTURON)
			continue;
		if (dist > a.escala * 2.0f && glm::dot(d / dist, direccion) < 0.35f - a.escala / dist)
			continue;
		bool usarGrande = hayGrande && (grandeEnTodos || a.grande || !hayChico);
		Model& modelo = usarGrande ? asteroideGrande : asteroideChico;
		glm::mat4 m = glm::translate(glm::mat4(1.0f), p)
			* glm::rotate(glm::mat4(1.0f), a.giro * tiempo + a.fase, a.eje)
			* glm::scale(glm::mat4(1.0f), a.estiramiento * a.escala)
			* normalizar(modelo);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(m));
		if (usarGrande)
			modelo.RenderModel();
		else
			modelo.RenderModelConTextura(texAsteroide);
	}
}

float LargoTramo(int tramo)
{
	return (tramo >= 0 && tramo < NUM_TRAMOS) ? tramos[tramo].largo : 0.0f;
}

float DistanciaMeta()
{
	return sMeta;
}

void PuntoEnPista(int tramo, float s, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba)
{
	glm::vec3 p, derecha;
	marcoTramo(tramos[glm::clamp(tramo, 0, NUM_TRAMOS - 1)], s, p, adelante, arriba, derecha);
	posicion = p + derecha * carril + arriba * altura;
}

glm::vec3 AjustarAPista(const glm::vec3& posicion, float alturaOjos, glm::vec3* adelanteSalida, glm::vec3* arribaSalida)
{
	if (!inicializado)
		return posicion;
	static int tramoActual = -1, indiceActual = 0;
	// búsqueda cerca del último punto (para no brincar de un tramo a otro); completa si se perdió
	int mejorTramo = -1, mejorIndice = 0;
	float mejor = 1.0e30f;
	if (tramoActual >= 0)
	{
		const Tramo& t = tramos[tramoActual];
		int desde = std::max(0, indiceActual - 80), hasta = std::min((int)t.puntos.size() - 1, indiceActual + 80);
		for (int i = desde; i <= hasta; i++)
		{
			float d = glm::length(t.puntos[i] - posicion);
			if (d < mejor) { mejor = d; mejorTramo = tramoActual; mejorIndice = i; }
		}
	}
	if (mejorTramo < 0 || mejor > 25.0f)
	{
		mejor = 1.0e30f;
		for (int k = 0; k < NUM_TRAMOS; k++)
			for (int i = 0; i < (int)tramos[k].puntos.size(); i++)
			{
				float d = glm::length(tramos[k].puntos[i] - posicion);
				if (d < mejor) { mejor = d; mejorTramo = k; mejorIndice = i; }
			}
	}
	tramoActual = mejorTramo;
	indiceActual = mejorIndice;

	// proyección sobre los segmentos vecinos para que el movimiento sea suave
	const Tramo& t = tramos[mejorTramo];
	float s = t.acumulado[mejorIndice];
	for (int k = -1; k <= 0; k++)
	{
		int i0 = mejorIndice + k, i1 = i0 + 1;
		if (i0 < 0 || i1 >= (int)t.puntos.size()) continue;
		glm::vec3 ab = t.puntos[i1] - t.puntos[i0];
		float f = glm::clamp(glm::dot(posicion - t.puntos[i0], ab) / fmaxf(glm::dot(ab, ab), 1.0e-6f), 0.0f, 1.0f);
		glm::vec3 q = t.puntos[i0] + ab * f;
		if (glm::length(posicion - q) <= glm::length(posicion - puntoTramo(t, s, nullptr)) + 1.0e-4f)
			s = t.acumulado[i0] + f * (t.acumulado[i1] - t.acumulado[i0]);
	}
	glm::vec3 p, adelante, arriba, derecha;
	marcoTramo(t, s, p, adelante, arriba, derecha);
	float lateral = glm::clamp(glm::dot(posicion - p, derecha), -(0.5f * ANCHO_PISTA - 1.0f), 0.5f * ANCHO_PISTA - 1.0f);
	if (adelanteSalida) *adelanteSalida = adelante;
	if (arribaSalida) *arribaSalida = arriba;
	return p + derecha * lateral + arriba * alturaOjos;
}

void SalidaEnMeta(glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba, float altura, float carril)
{
	glm::vec3 p, derecha;
	marcoTramo(tramos[0], fmaxf(sMeta - 3.0f, 0.0f), p, adelante, arriba, derecha);
	posicion = p + derecha * carril + arriba * altura;
}

void DatosBase(glm::vec3& centro, glm::vec3& adelante, glm::vec3& derecha, float& radio)
{
	centro = centroBase;
	adelante = adelanteBase;
	derecha = derechaBase;
	radio = RADIO_BASE;
}

void DibujarBase(float tiempo, const UniformsEspacio& u)
{
	if (!inicializado || mallaBase == nullptr)
		return;
	glm::mat4 identidad(1.0f);
	glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(identidad));
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	matPista.UseMaterial(u.especular, u.brillo);
	texBase.UseTexture();
	mallaBase->RenderMeshModel();
	mallaPilares->RenderMeshModel();

	// luces en la orilla de la plataforma: una onda que gira, brillo = 0.3 + 0.7 max(0, sen(4 a - 3 t))^3
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	texPlain.UseTexture();
	matMarca.UseMaterial(u.especular, u.brillo);
	const glm::vec3 Y(0.0f, 1.0f, 0.0f);
	for (int k = 0; k < 32; k++)
	{
		float a = 2.0f * PI_F * k / 32.0f;
		float onda = fmaxf(0.0f, sinf(4.0f * a - 3.0f * tiempo));
		glm::vec3 radial = derechaBase * cosf(a) + adelanteBase * sinf(a);
		glm::vec3 tangente = -derechaBase * sinf(a) + adelanteBase * cosf(a);
		dibujarMarca(centroBase + radial * (RADIO_BASE - 0.5f) + Y * 0.05f, tangente, radial, Y, 1.4f, 0.35f,
			glm::vec3(1.0f, 0.75f, 0.25f) * (0.3f + 0.7f * onda * onda * onda), u);
	}
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void InicioDePista(glm::vec3& posicion, float& yaw, float& pitch, float alturaOjos)
{
	puntoDeAparicion(tramos[0], fmaxf(sMeta - 6.0f, 0.0f), alturaOjos, posicion, yaw, pitch);   // justo antes de la meta
	pitch -= 8.0f;   // mirando un poco hacia el camino
}

void SalidaDePlaneta(int planeta, glm::vec3& posicion, float& yaw, float& pitch, float alturaOjos)
{
	// sobre la pista, ya fuera de la atmósfera (si no, volvería a entrar al planeta)
	if (planeta == PLANETA_SELVATICO)
		puntoDeAparicion(tramos[2], sDistanciaAPlaneta(tramos[2], CENTRO_SELVATICO, ATMOSFERA_SELVATICO + 9.0f, false), alturaOjos, posicion, yaw, pitch);
	else if (planeta == PLANETA_OCEANO)
		puntoDeAparicion(tramos[0], sDistanciaAPlaneta(tramos[0], CentroPlanetaOceano(), RadioAtmosferaOceano() + 8.0f, false), alturaOjos, posicion, yaw, pitch);
	else
		puntoDeAparicion(tramos[1], sDistanciaAPlaneta(tramos[1], CENTRO_ROCOSO, ATMOSFERA_ROCOSO + 8.0f, false), alturaOjos, posicion, yaw, pitch);
}
