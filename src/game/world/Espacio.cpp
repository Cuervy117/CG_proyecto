#include "Espacio.h"

#include <cmath>
#include <cstdio>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// =============================================================================================
// Pista. Planetas: rocoso en (0, 0, 0) con radio 14 (atmósfera 22) y selvático en (120, 10, 90)
// con radio 16 (atmósfera 24). Cada tramo empieza y termina DENTRO de un planeta: así el camino
// "entra" a su atmósfera y la carrera sigue en su superficie.
// =============================================================================================
const glm::vec3 CONTROL_TRAMO_A[] = {			// sale del océano -> meta -> entra al rocoso
	{ 138.0f, -30.0f, -46.0f },		//    dentro del planeta océano (sale por su lado cercano)
	{ 120.0f, -28.0f, -56.0f },
	{ 92.0f, -18.0f, -60.0f },		//    SUBIDA hacia la meta
	{ 62.0f, -8.0f, -56.0f },
	{ 34.0f, 2.0f, -50.0f },
	{ 12.0f, 6.0f, -46.0f },
	{ -6.0f, 6.0f, -44.0f },		// 6  META
	{ -30.0f, 2.0f, -40.0f },
	{ -42.0f, -8.0f, -22.0f },		//    BAJADA para pasar por debajo del rocoso
	{ -32.0f, -22.0f, -8.0f },
	{ -14.0f, -22.0f, -2.0f },
	{ -4.0f, -10.0f, -1.0f },		//    dentro del planeta rocoso (entra por abajo)
};
const int INDICE_META = 6;
const glm::vec3 CONTROL_TRAMO_B[] = {			// sale del rocoso -> cinturón -> entra al selvático
	{ 6.0f, 9.0f, 5.0f },			//    dentro del planeta rocoso (sale por su lado superior)
	{ 18.0f, 16.0f, 12.0f },
	{ 34.0f, 18.0f, 28.0f },
	{ 52.0f, 16.0f, 46.0f },		//    cruza el cinturón de asteroides
	{ 72.0f, 26.0f, 66.0f },		//    SUBIDA
	{ 92.0f, 40.0f, 82.0f },
	{ 110.0f, 48.0f, 90.0f },		//    sobre el planeta selvático
	{ 118.0f, 36.0f, 90.0f },
	{ 120.0f, 22.0f, 90.0f },		//    dentro del planeta selvático (entra por arriba)
};
const glm::vec3 CONTROL_TRAMO_C[] = {			// sale del selvático -> entra al océano
	{ 114.0f, 0.0f, 80.0f },		//    dentro del planeta selvático (sale por abajo)
	{ 110.0f, -10.0f, 66.0f },
	{ 126.0f, -22.0f, 44.0f },		//    BAJADA
	{ 148.0f, -26.0f, 18.0f },
	{ 158.0f, -20.0f, -2.0f },		//    rodea por arriba al planeta océano...
	{ 154.0f, -16.0f, -18.0f },
	{ 150.0f, -22.0f, -32.0f },		//    ...y entra (dentro del planeta océano)
};
const glm::vec3 CENTRO_ROCOSO(0.0f, 0.0f, 0.0f);
const glm::vec3 CENTRO_SELVATICO(120.0f, 10.0f, 90.0f);
const float ATMOSFERA_ROCOSO = 22.0f;
const float ATMOSFERA_SELVATICO = 24.0f;

const float ANCHO_PISTA = 10.0f;		// ancho del camino
const float GROSOR_PISTA = 1.2f;		// la pista flota en el espacio: se le da grosor
const float MOSAICO_PISTA = 8.0f;		// unidades de pista por cada repetición de la textura
const float PASO_LUCES = 4.0f;
const glm::vec3 COLOR_LUCES(0.35f, 0.85f, 1.0f);

// Base de inicio: plataforma circular bajo la pista, antes de la meta
const float RADIO_BASE = 15.0f;
const float GROSOR_BASE = 1.8f;
const float BASE_BAJO_PISTA = 16.0f;		// qué tan abajo del camino queda el piso de la base
const float BASE_ANTES_DE_META = 34.0f;		// a cuántas unidades antes de la meta

// Cinturón de asteroides (los dos modelos vienen en Models/asteroide)
const int CANTIDAD_ASTEROIDES = 150;
const int MAX_ASTEROIDES_GRANDES = 6;					// asteroide.obj pesa 500 mil triángulos: solo unos cuantos
const int TRIANGULOS_PARA_USARLO_EN_TODOS = 30000;		// si se reduce en Blender, se usa en todos
const float DISTANCIA_CINTURON = 420.0f;
const float PI_ESPACIO = 3.14159265f;


Espacio::Espacio()
{
	sMeta = 0.0f;
	direccionCinturon = glm::vec3(1.0f, 0.0f, 0.0f);
	hayModeloPista = false;
	for (int t = 0; t < NUM_TRAMOS; t++)
		mallaPistaCodigo[t] = nullptr;
	hayGrande = false;
	hayChico = false;
	grandeEnTodos = false;
	cuadroMarca = nullptr;
	mallaBase = nullptr;
	mallaPilares = nullptr;
	centroBase = glm::vec3(0.0f);
	adelanteBase = glm::vec3(0.0f, 0.0f, 1.0f);
	derechaBase = glm::vec3(1.0f, 0.0f, 0.0f);
	Material_pista = Material(0.3f, 8.0f);
	Material_marca = Material(0.2f, 4.0f);
	Material_asteroide = Material(0.4f, 16.0f);
	inicializado = false;
	oceano = nullptr;
	tramoActual = -1;
	indiceActual = 0;
	uniformModel = 0;
	uniformColor = 0;
	uniformSpecularIntensity = 0;
	uniformShininess = 0;
	uniformEmision = 0;
}

// NUEVO: curva de Catmull-Rom entre p1 y p2 (p0 y p3 dan la dirección con la que entra y sale).
//   C(t) = 0.5 [ 2 p1 + (p2 - p0) t + (2 p0 - 5 p1 + 4 p2 - p3) t^2 + (3 p1 - p0 - 3 p2 + p3) t^3 ]
glm::vec3 Espacio::CatmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
	float t2 = t * t, t3 = t2 * t;
	return 0.5f * ((2.0f * p1) + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2
		+ (3.0f * p1 - p0 - 3.0f * p2 + p3) * t3);
}

// Curva abierta por los puntos de control, remuestreada cada 1 unidad.
// 'indiceMarca' (opcional) regresa en sMarca a qué distancia pasa ese punto de control (la meta).
void Espacio::ConstruirTramo(Tramo& tramo, const glm::vec3* control, int n, int indiceMarca, float* sMarca)
{
	// 1) 40 puntos de la curva entre cada par de puntos de control
	std::vector<glm::vec3> denso;
	std::vector<int> indiceDe;
	for (int i = 0; i + 1 < n; i++)
	{
		glm::vec3 p0 = (i == 0) ? 2.0f * control[0] - control[1] : control[i - 1];
		glm::vec3 p3 = (i + 2 >= n) ? 2.0f * control[n - 1] - control[n - 2] : control[i + 2];
		for (int k = 0; k < 40; k++)
		{
			denso.push_back(CatmullRom(p0, control[i], control[i + 1], p3, k / 40.0f));
			indiceDe.push_back(k == 0 ? i : -1);
		}
	}
	denso.push_back(control[n - 1]);
	indiceDe.push_back(n - 1);

	// 2) se vuelve a muestrear para tener un punto exactamente cada 1 unidad de distancia
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

// Punto del tramo a 's' unidades de su inicio (y su dirección en 'tangente')
glm::vec3 Espacio::PuntoTramo(const Tramo& tramo, float s, glm::vec3* tangente)
{
	s = glm::clamp(s, 0.0f, tramo.largo);
	// último punto que está antes de s
	size_t i = 0;
	while (i + 1 < tramo.acumulado.size() && tramo.acumulado[i + 1] <= s)
		i++;
	if (i + 1 >= tramo.puntos.size()) i = tramo.puntos.size() - 2;
	float t = (s - tramo.acumulado[i]) / fmaxf(tramo.acumulado[i + 1] - tramo.acumulado[i], 1.0e-6f);
	if (tangente) *tangente = glm::normalize(tramo.puntos[i + 1] - tramo.puntos[i]);
	return tramo.puntos[i] + (tramo.puntos[i + 1] - tramo.puntos[i]) * glm::clamp(t, 0.0f, 1.0f);
}

// Marco de la pista en un punto: adelante, arriba (hacia el cielo) y derecha
void Espacio::MarcoTramo(const Tramo& tramo, float s, glm::vec3& p, glm::vec3& adelante, glm::vec3& arriba, glm::vec3& derecha)
{
	p = PuntoTramo(tramo, s, &adelante);
	glm::vec3 mundoArriba(0.0f, 1.0f, 0.0f);
	arriba = mundoArriba - adelante * glm::dot(mundoArriba, adelante);
	arriba = (glm::length(arriba) < 1.0e-3f) ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::normalize(arriba);
	derecha = glm::normalize(glm::cross(adelante, arriba));
}

// Malla del camino de un tramo (solo si no está pista.obj): cara superior con la textura de asfalto,
// dos costados y fondo. Las normales van invertidas, como las de los modelos cargados.
void Espacio::MallaTramo(const Tramo& tramo, std::vector<GLfloat>& v, std::vector<unsigned int>& indices)
{
	for (size_t i = 0; i < tramo.puntos.size(); i++)
	{
		glm::vec3 p, adelante, arriba, derecha;
		MarcoTramo(tramo, tramo.acumulado[i], p, adelante, arriba, derecha);
		float coordV = tramo.acumulado[i] / MOSAICO_PISTA;
		glm::vec3 izq = p - derecha * (0.5f * ANCHO_PISTA), der = p + derecha * (0.5f * ANCHO_PISTA);
		glm::vec3 abajo = -arriba * GROSOR_PISTA;
		// (posición, u, normal) de las 8 esquinas de la sección: arriba, costado derecho, fondo, costado izquierdo
		glm::vec3 posiciones[8] = { izq, der, der, der + abajo, der + abajo, izq + abajo, izq + abajo, izq };
		float coordU[8] = { 0.0f, 1.0f, 0.98f, 1.0f, 0.5f, 0.5f, 0.0f, 0.02f };
		glm::vec3 normales[8] = { arriba, arriba, derecha, derecha, -arriba, -arriba, -derecha, -derecha };
		for (int k = 0; k < 8; k++)
		{
			GLfloat d[] = { posiciones[k].x, posiciones[k].y, posiciones[k].z, coordU[k], coordV,
				-normales[k].x, -normales[k].y, -normales[k].z };
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

// x y z u v nx ny nz, con la normal invertida como en las mallas creadas con código del curso
void Espacio::AgregarVertice(std::vector<GLfloat>& v, glm::vec3 p, float s, float t, glm::vec3 n)
{
	GLfloat d[] = { p.x, p.y, p.z, s, t, -n.x, -n.y, -n.z };
	v.insert(v.end(), d, d + 8);
}

// punto de la orilla de la base a un ángulo, un radio y una altura
glm::vec3 Espacio::PuntoBase(float angulo, float radio, float y)
{
	return centroBase + derechaBase * (radio * cosf(angulo)) + adelanteBase * (radio * sinf(angulo)) + glm::vec3(0.0f, 1.0f, 0.0f) * y;
}

// Plataforma (cilindro de 40 lados) y dos pilares que la cuelgan del camino
void Espacio::ConstruirBase()
{
	glm::vec3 p, adelante, arriba, derecha;
	float sBase = fmaxf(sMeta - BASE_ANTES_DE_META, 0.0f);
	MarcoTramo(tramos[0], sBase, p, adelante, arriba, derecha);
	adelanteBase = glm::normalize(glm::vec3(adelante.x, 0.0f, adelante.z));
	derechaBase = glm::normalize(glm::cross(adelanteBase, glm::vec3(0.0f, 1.0f, 0.0f)));
	centroBase = p - glm::vec3(0.0f, BASE_BAJO_PISTA, 0.0f);

	const int lados = 40;
	const glm::vec3 Y(0.0f, 1.0f, 0.0f);
	std::vector<GLfloat> v;
	std::vector<unsigned int> idx;
	// tapa superior: abanico con la textura de pista de aterrizaje
	AgregarVertice(v, centroBase, 0.5f, 0.5f, Y);
	for (int k = 0; k <= lados; k++)
	{
		float a = 2.0f * PI_ESPACIO * k / lados;
		AgregarVertice(v, PuntoBase(a, RADIO_BASE, 0.0f), 0.5f + 0.5f * cosf(a), 0.5f + 0.5f * sinf(a), Y);
	}
	for (int k = 0; k < lados; k++)
	{
		unsigned int t[] = { 0u, (unsigned)(1 + k), (unsigned)(2 + k) };
		idx.insert(idx.end(), t, t + 3);
	}
	// costado: toma el color de la franja de seguridad del borde de la textura
	unsigned int inicio = (unsigned int)(v.size() / 8);
	for (int k = 0; k <= lados; k++)
	{
		float a = 2.0f * PI_ESPACIO * k / lados;
		glm::vec3 radial = derechaBase * cosf(a) + adelanteBase * sinf(a);
		float s = 0.5f + 0.47f * cosf(a), t = 0.5f + 0.47f * sinf(a);
		AgregarVertice(v, PuntoBase(a, RADIO_BASE, 0.0f), s, t, radial);
		AgregarVertice(v, PuntoBase(a, RADIO_BASE * 0.9f, -GROSOR_BASE), s, t, radial);
	}
	for (int k = 0; k < lados; k++)
	{
		unsigned int a0 = inicio + 2 * k, a1 = a0 + 1, b0 = a0 + 2, b1 = a0 + 3;
		unsigned int t[] = { a0, a1, b0, b0, a1, b1 };
		idx.insert(idx.end(), t, t + 6);
	}
	// fondo
	unsigned int centroFondo = (unsigned int)(v.size() / 8);
	AgregarVertice(v, centroBase - Y * GROSOR_BASE, 0.5f, 0.5f, -Y);
	for (int k = 0; k <= lados; k++)
	{
		float a = 2.0f * PI_ESPACIO * k / lados;
		AgregarVertice(v, PuntoBase(a, RADIO_BASE * 0.9f, -GROSOR_BASE), 0.5f + 0.2f * cosf(a), 0.5f + 0.2f * sinf(a), -Y);
	}
	for (int k = 0; k < lados; k++)
	{
		unsigned int t[] = { centroFondo, centroFondo + 2 + k, centroFondo + 1 + k };
		idx.insert(idx.end(), t, t + 3);
	}
	mallaBase = new MeshModel();
	mallaBase->CreateMeshModel(v.data(), idx.data(), (unsigned int)v.size(), (unsigned int)idx.size());

	// dos pilares de la plataforma hasta la parte de abajo del camino
	std::vector<GLfloat> vp;
	std::vector<unsigned int> ip;
	for (int lado = -1; lado <= 1; lado += 2)
	{
		glm::vec3 q, a2, ar2, de2;
		MarcoTramo(tramos[0], sBase + 9.0f * (float)lado, q, a2, ar2, de2);
		glm::vec3 arribaPilar = q - ar2 * (GROSOR_PISTA + 0.05f);
		glm::vec3 abajoPilar(arribaPilar.x, centroBase.y, arribaPilar.z);
		unsigned int primero = (unsigned int)(vp.size() / 8);
		for (int k = 0; k <= 10; k++)
		{
			float a = 2.0f * PI_ESPACIO * k / 10.0f;
			glm::vec3 radial(cosf(a), 0.0f, sinf(a));
			AgregarVertice(vp, abajoPilar + radial * 0.9f, 0.5f, 0.12f, radial);
			AgregarVertice(vp, arribaPilar + radial * 0.6f, 0.5f, 0.14f, radial);
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

float Espacio::DistanciaAPista(glm::vec3 q)
{
	float mejor = 1.0e30f;
	for (int t = 0; t < NUM_TRAMOS; t++)
		for (size_t i = 0; i < tramos[t].puntos.size(); i += 2)
			mejor = fminf(mejor, glm::length(tramos[t].puntos[i] - q));
	return mejor;
}

// Reparte los asteroides en una franja entre los dos planetas, dejando libre el paso de la pista
void Espacio::GenerarCinturon()
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
		if (DistanciaAPista(p) < tamano + 8.0f)
			continue;   // deja libre el paso de la pista
		Asteroide a;
		a.base = p;
		a.escala = tamano;
		a.estiramiento = glm::vec3(1.0f, rnd.rango(0.7f, 1.1f), rnd.rango(0.8f, 1.2f));
		a.eje = glm::normalize(glm::vec3(rnd.rango(-1.0f, 1.0f), rnd.rango(-1.0f, 1.0f), rnd.rango(-1.0f, 1.0f)) + glm::vec3(0.0f, 0.01f, 0.0f));
		a.giro = rnd.rango(0.15f, 0.7f) * (rnd.uniforme() < 0.5f ? -1.0f : 1.0f);
		a.fase = rnd.rango(0.0f, 2.0f * PI_ESPACIO);
		a.grande = (tamano > 2.6f && grandes < MAX_ASTEROIDES_GRANDES);
		if (a.grande) grandes++;
		asteroides.push_back(a);
	}
}

// Un cuadro brillante (luz, cuadro de la meta o pieza de un arco) en el marco dado
void Espacio::DibujarMarca(glm::vec3 centro, glm::vec3 ejeX, glm::vec3 ejeY, glm::vec3 normal,
	float ancho, float largo, glm::vec3 color)
{
	// matriz armada con los ejes (como Orientar de la práctica 9-1) y luego escalada
	glm::mat4 model(1.0f);
	model[0] = glm::vec4(ejeX, 0.0f);
	model[1] = glm::vec4(ejeY, 0.0f);
	model[2] = glm::vec4(normal, 0.0f);
	model[3] = glm::vec4(centro, 1.0f);
	model = glm::scale(model, glm::vec3(ancho, largo, 1.0f));
	glUniform3f(uniformColor, color.x * 0.3f, color.y * 0.3f, color.z * 0.3f);
	glUniform3f(uniformEmision, color.x, color.y, color.z);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	cuadroMarca->RenderMeshModel();
}

// Distancia (dentro del tramo) donde el camino cruza la atmósfera de un planeta
float Espacio::DistanciaAPlaneta(const Tramo& tramo, glm::vec3 centro, float distancia, bool desdeElFinal)
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

void Espacio::PuntoDeAparicion(const Tramo& tramo, float s, float alturaOjos, glm::vec3& posicion, float& yaw, float& pitch)
{
	glm::vec3 p, adelante, arriba, derecha;
	MarcoTramo(tramo, s, p, adelante, arriba, derecha);
	posicion = p + arriba * alturaOjos;
	yaw = glm::degrees(atan2f(adelante.z, adelante.x));
	pitch = glm::clamp(glm::degrees(asinf(glm::clamp(adelante.y, -1.0f, 1.0f))), -30.0f, 30.0f);
}

// =============================================================================================
void Espacio::InicializarEspacio(Oceano* oceanoPlaneta)
{
	if (inicializado)
		return;
	oceano = oceanoPlaneta;
	ConstruirTramo(tramos[0], CONTROL_TRAMO_A, (int)(sizeof(CONTROL_TRAMO_A) / sizeof(CONTROL_TRAMO_A[0])), INDICE_META, &sMeta);
	ConstruirTramo(tramos[1], CONTROL_TRAMO_B, (int)(sizeof(CONTROL_TRAMO_B) / sizeof(CONTROL_TRAMO_B[0])), -1, nullptr);
	ConstruirTramo(tramos[2], CONTROL_TRAMO_C, (int)(sizeof(CONTROL_TRAMO_C) / sizeof(CONTROL_TRAMO_C[0])), -1, nullptr);
	printf("[OK] Pista: tramos de %.0f (meta en %.0f), %.0f y %.0f unidades\n", tramos[0].largo, sMeta, tramos[1].largo, tramos[2].largo);

	// texturas
	pistaTexture = Texture("Textures/pistaTexture.png");
	if (!pistaTexture.LoadTextureA()) {
		pistaTexture = Texture("Textures/plain.png");
		pistaTexture.LoadTextureA();
	}
	asteroideTexture = Texture("Textures/asteroideTexture.png");
	asteroideTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	baseTexture = Texture("Textures/baseTexture.png");
	if (!baseTexture.LoadTextureA()) {
		baseTexture = Texture("Textures/plain.png");
		baseTexture.LoadTextureA();
	}

	// la pista es un modelo .obj (hecho con las mismas curvas); si no está, se arma aquí con código
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista/pista.obj");
	hayModeloPista = Pista_M.estaCargado();
	if (hayModeloPista)
		printf("[OK] Pista: Models/pista/pista.obj (%d triangulos)\n", Pista_M.getNumTriangulos());
	else
	{
		printf("[AVISO] No se encontro Models/pista/pista.obj: la pista se arma con codigo\n");
		for (int t = 0; t < NUM_TRAMOS; t++)
		{
			std::vector<GLfloat> v;
			std::vector<unsigned int> indices;
			MallaTramo(tramos[t], v, indices);
			mallaPistaCodigo[t] = new MeshModel();
			mallaPistaCodigo[t]->CreateMeshModel(v.data(), indices.data(), (unsigned int)v.size(), (unsigned int)indices.size());
		}
	}

	// asteroides
	printf("Cargando Models/asteroide/asteroide.obj ...\n");
	AsteroideGrande_M = Model();
	AsteroideGrande_M.LoadModel("Models/asteroide/asteroide.obj");
	hayGrande = AsteroideGrande_M.estaCargado();
	AsteroideChico_M = Model();
	AsteroideChico_M.LoadModel("Models/asteroide/Rocky_Asteroid_6.obj");
	hayChico = AsteroideChico_M.estaCargado();
	grandeEnTodos = hayGrande && AsteroideGrande_M.getNumTriangulos() <= TRIANGULOS_PARA_USARLO_EN_TODOS;

	ConstruirBase();
	printf("[OK] Base de inicio: plataforma de radio %.0f bajo la pista, %.0f unidades antes de la meta\n", RADIO_BASE, BASE_ANTES_DE_META);

	GenerarCinturon();
	printf("[OK] Cinturon de asteroides: %d asteroides\n", (int)asteroides.size());

	// cuadro de las luces, la meta y los arcos
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, -1.0f,
	};
	cuadroMarca = new MeshModel();
	cuadroMarca->CreateMeshModel(vertices, indices, 32, 6);
	inicializado = true;
}

void Espacio::DibujarPista(float tiempo, GLuint modelLocation, GLuint colorLocation,
	GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation)
{
	if (!inicializado)
		return;
	uniformModel = modelLocation;
	uniformColor = colorLocation;
	uniformSpecularIntensity = specularIntensityLocation;
	uniformShininess = shininessLocation;
	uniformEmision = emisionLocation;

	// 1) el camino (modelo pista.obj con textura de asfalto)
	glm::mat4 model(1.0);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
	glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
	Material_pista.UseMaterial(uniformSpecularIntensity, uniformShininess);
	if (hayModeloPista)
		Pista_M.RenderModel();
	else
	{
		pistaTexture.UseTexture();
		for (int t = 0; t < NUM_TRAMOS; t++)
			mallaPistaCodigo[t]->RenderMeshModel();
	}

	// NUEVO (animación continua): luces en las orillas con ondas que corren por la pista
	//    brillo = 0.35 + 0.65 * max(0, sen(2 pi (s/48 - 0.7 t)))^4
	// donde s es la distancia sobre la pista y t el tiempo. Se dibujan con blending aditivo.
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	plainTexture.UseTexture();
	Material_marca.UseMaterial(uniformSpecularIntensity, uniformShininess);
	glm::vec3 p, adelante, arriba, derecha;
	for (int t = 0; t < NUM_TRAMOS; t++)
	{
		const Tramo& tramo = tramos[t];
		float desfase = 300.0f * (float)t;
		for (float s = 2.0f; s < tramo.largo - 2.0f; s += PASO_LUCES)
		{
			MarcoTramo(tramo, s, p, adelante, arriba, derecha);
			float onda = fmaxf(0.0f, sinf(2.0f * PI_ESPACIO * ((s + desfase) / 48.0f - 0.7f * tiempo)));
			glm::vec3 c = COLOR_LUCES * (0.35f + 0.65f * onda * onda * onda * onda);
			for (int lado = -1; lado <= 1; lado += 2)
				DibujarMarca(p + derecha * ((0.5f * ANCHO_PISTA - 0.35f) * (float)lado) + arriba * 0.08f,
					derecha, adelante, arriba, 0.3f, 1.4f, c);
		}
	}

	// 3) línea de meta: cuadros blancos y azules a lo ancho del camino
	MarcoTramo(tramos[0], sMeta, p, adelante, arriba, derecha);
	for (int fila = 0; fila < 2; fila++)
	{
		for (int k = 0; k < 10; k++)
		{
			bool claro = ((k + fila) % 2) == 0;
			glm::vec3 c = claro ? glm::vec3(1.0f) : glm::vec3(0.1f, 0.25f, 0.6f);
			DibujarMarca(p + derecha * (-0.5f * ANCHO_PISTA + (k + 0.5f) * ANCHO_PISTA / 10.0f) + adelante * ((float)fila - 0.5f) + arriba * 0.06f,
				derecha, adelante, arriba, ANCHO_PISTA / 10.0f, 1.0f, c);
		}
	}

	// 4) arcos de luz donde el camino entra a la atmósfera de cada planeta (pulsan con un seno)
	int tramoArco[3] = { 0, 1, 2 };
	glm::vec3 centroArco[3] = { CENTRO_ROCOSO, CENTRO_SELVATICO, oceano->CentroPlanetaOceano() };
	float atmosferaArco[3] = { ATMOSFERA_ROCOSO, ATMOSFERA_SELVATICO, oceano->RadioAtmosferaOceano() };
	glm::vec3 colorArco[3] = { glm::vec3(0.75f, 0.45f, 1.0f), glm::vec3(0.45f, 1.0f, 0.55f), glm::vec3(0.35f, 0.7f, 1.0f) };
	float pulso = 0.6f + 0.4f * sinf(3.0f * tiempo);
	for (int a = 0; a < 3; a++)
	{
		float s = DistanciaAPlaneta(tramos[tramoArco[a]], centroArco[a], atmosferaArco[a] + 2.0f, true);
		MarcoTramo(tramos[tramoArco[a]], s, p, adelante, arriba, derecha);
		const int piezas = 18;
		float radio = 0.5f * ANCHO_PISTA + 1.0f;
		for (int k = 0; k < piezas; k++)
		{
			float ang = PI_ESPACIO * (k + 0.5f) / piezas;   // de la orilla derecha a la izquierda, por arriba
			glm::vec3 radial = derecha * cosf(ang) + arriba * sinf(ang);
			glm::vec3 tangenteArco = -derecha * sinf(ang) + arriba * cosf(ang);
			DibujarMarca(p + radial * radio, tangenteArco, adelante, glm::normalize(glm::cross(tangenteArco, adelante)),
				PI_ESPACIO * radio / piezas * 0.85f, 0.5f, colorArco[a] * pulso);
		}
	}

	glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void Espacio::DibujarCinturon(glm::vec3 posCamara, glm::vec3 dirCamara, float tiempo, GLuint modelLocation, GLuint colorLocation,
	GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation)
{
	if (!inicializado || (!hayGrande && !hayChico))
		return;
	glm::mat4 model(1.0);
	Material_asteroide.UseMaterial(specularIntensityLocation, shininessLocation);
	glUniform3f(colorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(emisionLocation, 0.0f, 0.0f, 0.0f);
	glm::vec3 direccion = glm::normalize(dirCamara);
	for (size_t i = 0; i < asteroides.size(); i++)
	{
		const Asteroide& a = asteroides[i];
		// NUEVO (animación continua): cada asteroide gira sobre su eje (ángulo = giro * t + fase)
		// y se mece a lo largo del cinturón con un seno
		glm::vec3 p = a.base + direccionCinturon * (1.2f * sinf(0.15f * tiempo + a.fase));
		// NUEVO: solo se dibujan los cercanos que están frente a la cámara (para que no vaya lento)
		glm::vec3 d = p - posCamara;
		float dist = glm::length(d);
		if (dist > DISTANCIA_CINTURON)
			continue;
		if (dist > a.escala * 2.0f && glm::dot(d / dist, direccion) < 0.35f - a.escala / dist)
			continue;
		bool usarGrande = hayGrande && (grandeEnTodos || a.grande || !hayChico);
		Model& modelo = usarGrande ? AsteroideGrande_M : AsteroideChico_M;
		model = glm::mat4(1.0);
		model = glm::translate(model, p);
		model = glm::rotate(model, a.giro * tiempo + a.fase, a.eje);   // ángulo en radianes
		model = glm::scale(model, a.estiramiento * a.escala);
		model = glm::scale(model, glm::vec3(1.0f / modelo.getRadio()));   // el modelo medido a radio 1
		model = glm::translate(model, -modelo.getCentro());
		glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
		if (usarGrande)
			modelo.RenderModel();
		else
			modelo.RenderModelConTextura(asteroideTexture);
	}
}

float Espacio::LargoTramo(int tramo)
{
	return (tramo >= 0 && tramo < NUM_TRAMOS) ? tramos[tramo].largo : 0.0f;
}

float Espacio::DistanciaMeta()
{
	return sMeta;
}

void Espacio::PuntoEnPista(int tramo, float s, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba)
{
	glm::vec3 p, derecha;
	MarcoTramo(tramos[glm::clamp(tramo, 0, NUM_TRAMOS - 1)], s, p, adelante, arriba, derecha);
	posicion = p + derecha * carril + arriba * altura;
}

glm::vec3 Espacio::AjustarAPista(glm::vec3 posicion, float alturaOjos, glm::vec3& adelanteSalida, glm::vec3& arribaSalida)
{
	if (!inicializado)
		return posicion;
	// búsqueda cerca del último punto (para no brincar de un tramo a otro); completa si se perdió
	int mejorTramo = -1, mejorIndice = 0;
	float mejor = 1.0e30f;
	if (tramoActual >= 0)
	{
		const Tramo& t = tramos[tramoActual];
		int desde = indiceActual - 80;
		if (desde < 0) desde = 0;
		int hasta = indiceActual + 80;
		if (hasta > (int)t.puntos.size() - 1) hasta = (int)t.puntos.size() - 1;
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
		if (glm::length(posicion - q) <= glm::length(posicion - PuntoTramo(t, s, nullptr)) + 1.0e-4f)
			s = t.acumulado[i0] + f * (t.acumulado[i1] - t.acumulado[i0]);
	}
	glm::vec3 p, adelante, arriba, derecha;
	MarcoTramo(t, s, p, adelante, arriba, derecha);
	float lateral = glm::clamp(glm::dot(posicion - p, derecha), -(0.5f * ANCHO_PISTA - 1.0f), 0.5f * ANCHO_PISTA - 1.0f);
	adelanteSalida = adelante;
	arribaSalida = arriba;
	return p + derecha * lateral + arriba * alturaOjos;
}

void Espacio::SalidaEnMeta(glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba, float altura, float carril)
{
	glm::vec3 p, derecha;
	MarcoTramo(tramos[0], fmaxf(sMeta - 3.0f, 0.0f), p, adelante, arriba, derecha);
	posicion = p + derecha * carril + arriba * altura;
}

void Espacio::DatosBase(glm::vec3& centro, glm::vec3& adelante, glm::vec3& derecha, float& radio)
{
	centro = centroBase;
	adelante = adelanteBase;
	derecha = derechaBase;
	radio = RADIO_BASE;
}

void Espacio::DibujarBase(float tiempo, GLuint modelLocation, GLuint colorLocation,
	GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation)
{
	if (!inicializado || mallaBase == nullptr)
		return;
	uniformModel = modelLocation;
	uniformColor = colorLocation;
	uniformSpecularIntensity = specularIntensityLocation;
	uniformShininess = shininessLocation;
	uniformEmision = emisionLocation;

	glm::mat4 model(1.0);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
	glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
	Material_pista.UseMaterial(uniformSpecularIntensity, uniformShininess);
	baseTexture.UseTexture();
	mallaBase->RenderMeshModel();
	mallaPilares->RenderMeshModel();

	// NUEVO (animación continua): luces en la orilla de la plataforma, una onda que gira:
	//    brillo = 0.3 + 0.7 * max(0, sen(4 a - 3 t))^3   (a = ángulo alrededor de la base)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	plainTexture.UseTexture();
	Material_marca.UseMaterial(uniformSpecularIntensity, uniformShininess);
	const glm::vec3 Y(0.0f, 1.0f, 0.0f);
	for (int k = 0; k < 32; k++)
	{
		float a = 2.0f * PI_ESPACIO * k / 32.0f;
		float onda = fmaxf(0.0f, sinf(4.0f * a - 3.0f * tiempo));
		glm::vec3 radial = derechaBase * cosf(a) + adelanteBase * sinf(a);
		glm::vec3 tangente = -derechaBase * sinf(a) + adelanteBase * cosf(a);
		DibujarMarca(centroBase + radial * (RADIO_BASE - 0.5f) + Y * 0.05f, tangente, radial, Y, 1.4f, 0.35f,
			glm::vec3(1.0f, 0.75f, 0.25f) * (0.3f + 0.7f * onda * onda * onda));
	}
	glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
}

void Espacio::SalidaDePlaneta(int planeta, glm::vec3& posicion, float& yaw, float& pitch, float alturaOjos)
{
	// sobre la pista, ya fuera de la atmósfera (si no, volvería a entrar al planeta)
	if (planeta == PLANETA_SELVATICO)
		PuntoDeAparicion(tramos[2], DistanciaAPlaneta(tramos[2], CENTRO_SELVATICO, ATMOSFERA_SELVATICO + 9.0f, false), alturaOjos, posicion, yaw, pitch);
	else if (planeta == PLANETA_OCEANO)
		PuntoDeAparicion(tramos[0], DistanciaAPlaneta(tramos[0], oceano->CentroPlanetaOceano(), oceano->RadioAtmosferaOceano() + 8.0f, false), alturaOjos, posicion, yaw, pitch);
	else
		PuntoDeAparicion(tramos[1], DistanciaAPlaneta(tramos[1], CENTRO_ROCOSO, ATMOSFERA_ROCOSO + 8.0f, false), alturaOjos, posicion, yaw, pitch);
}

Espacio::~Espacio()
{
}

