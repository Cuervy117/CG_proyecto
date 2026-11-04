#include "Superficie.h"

#include <cmath>
#include <cstdio>
#include <algorithm>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// =============================================================================================
// Ajustes. Todo el mundo es FINITO: una zona alrededor del camino.
// Si el programa va lento, sube CELDA_BOSQUE (menos árboles) o baja DISTANCIA_MAXIMA[OBJ_ARBOL].
// =============================================================================================
const float RADIO_CLARO = 24.0f;			// claro sin árboles donde aterriza la cápsula
const float ANCHO_BOSQUE = 40.0f;			// franja de selva a cada lado del camino
const float CELDA_BOSQUE = 12.0f;			// separación media entre árboles de la franja
const float LIMITE_JUGADOR_SELVA = 20.0f;	// qué tanto puede alejarse el jugador del camino
const float LIMITE_JUGADOR_ROCOSO = 75.0f;
const float MARGEN_PISO_SELVA = 300.0f;		// piso más allá del camino (la niebla tapa la orilla)
const float MARGEN_PISO_ROCOSO = 650.0f;	// en el desierto el piso llega hasta las montañas
const float ALTURA_CAMINO = 0.02f;			// el camino va apenas sobre el piso
const double PRESUPUESTO_TRIANGULOS = 4500000.0;	// tope de triángulos por cuadro (seguridad)
const float ANGULO_CONO = 62.0f;			// medio ángulo horizontal que se dibuja (algo más que la vista)
const float DISTANCIA_NPC = 260.0f;
// Plantas luminosas (arbustos que brillan de noche). En 0 están desactivadas, pero el número
// aleatorio se sigue pidiendo para que el mapa no cambie.
const float PROBABILIDAD_LUMINOSA_ORILLA = 0.0f;
const float PROBABILIDAD_LUMINOSA_BOSQUE = 0.0f;
const float RADIO_JUGADOR = 0.9f;			// para no meterse en troncos ni rocas
// distancia máxima a la que se dibuja cada tipo (la niebla tapa lo que queda más lejos)
const float DISTANCIA_MAXIMA[NUM_TIPOS_OBJETO] = { 150.0f, 70.0f, 100.0f, 320.0f, 160.0f, 220.0f, 1.0e9f };
const glm::vec3 COLOR_ROCA(1.10f, 0.62f, 0.46f);
const float PI_SUPERFICIE = 3.14159265f;


Superficie::Superficie()
{
	inicializado = false;
	hayArbol = false;
	hayRoca = false;
	for (int i = 0; i < NUM_MODELOS_NPC; i++)
		hayNPC[i] = false;
	hayPortal = false;
	paulValido = false;
	revelacionPaul = 0.0f;
	hayTexArbol = false;
	for (int i = 0; i < 3; i++)
	{
		mallasCristal[i] = nullptr;
		triangulosCristal[i] = 0;
	}
	Material_piso = Material(0.15f, 8.0f);
	Material_arbol = Material(0.25f, 16.0f);
	Material_roca = Material(0.35f, 16.0f);
	Material_cristal = Material(3.0f, 128.0f);
	Material_npc = Material(0.5f, 32.0f);
	uniformModel = 0;
	uniformColor = 0;
	uniformTextureOffset = 0;
	uniformSpecularIntensity = 0;
	uniformShininess = 0;
	uniformEmision = 0;
	camaraPiso = glm::vec2(0.0f);
	direccionPiso = glm::vec2(0.0f, -1.0f);
	usarCono = false;
	tanCono = 0.0f;
	cosCono = 1.0f;
	tipoActual = -1;
	emisionActiva = false;
	brilloNocturno = 0.0f;
	triangulos = 0.0;
	tiempoAnterior = 0.0f;
}

// =============================================================================================
// Utilidades
// =============================================================================================

// Traslación que pone la base del modelo en el origen: pies en y = 0 y centrado en x, z
glm::vec3 Superficie::PiesDelModelo(const Model& modelo)
{
	glm::vec3 c = modelo.getCentro(), t = modelo.getTamano();
	return glm::vec3(-c.x, -(c.y - 0.5f * t.y), -c.z);
}

// Curva suave de 0 a 1 (smoothstep): 3x^2 - 2x^3
float Superficie::Suave(float x)
{
	x = glm::clamp(x, 0.0f, 1.0f);
	return x * x * (3.0f - 2.0f * x);
}

// true si el círculo (p, radio) no choca con ningún objeto ya colocado. Cada objeto se guarda como
// (x, z, radio). (Antes se usaba una rejilla con un mapa hash; da el mismo resultado.)
bool Superficie::Libre(const std::vector<glm::vec3>& ocupados, glm::vec2 p, float radio)
{
	for (size_t i = 0; i < ocupados.size(); i++)
	{
		if (glm::length(glm::vec2(ocupados[i].x, ocupados[i].y) - p) < ocupados[i].z + radio)
			return false;
	}
	return true;
}

// =============================================================================================
// Camino: curva Catmull-Rom cerrada que pasa por los puntos de control
// =============================================================================================
glm::vec2 Superficie::CatmullRom(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
	float t2 = t * t, t3 = t2 * t;
	return 0.5f * ((2.0f * p1) + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2
		+ (3.0f * p1 - p0 - 3.0f * p2 + p3) * t3);
}

// Curva cerrada remuestreada a distancia constante 'paso'
std::vector<glm::vec2> Superficie::MuestrearCamino(const std::vector<glm::vec2>& control, float paso)
{
	std::vector<glm::vec2> denso;
	int n = (int)control.size();
	for (int i = 0; i < n; i++)
	{
		glm::vec2 p0 = control[(i - 1 + n) % n];
		glm::vec2 p1 = control[i];
		glm::vec2 p2 = control[(i + 1) % n];
		glm::vec2 p3 = control[(i + 2) % n];
		for (int k = 0; k < 40; k++)
			denso.push_back(CatmullRom(p0, p1, p2, p3, k / 40.0f));
	}

	std::vector<glm::vec2> salida;
	salida.push_back(denso[0]);
	float restante = paso;   // lo que falta recorrer para poner la siguiente muestra
	for (size_t i = 0; i < denso.size(); i++)
	{
		glm::vec2 a = denso[i], b = denso[(i + 1) % denso.size()];
		float largo = glm::length(b - a);
		float recorrido = 0.0f;
		while (largo - recorrido >= restante)
		{
			recorrido += restante;
			salida.push_back(a + (b - a) * (recorrido / largo));
			restante = paso;
		}
		restante -= (largo - recorrido);
	}
	if (salida.size() > 2 && glm::length(salida.back() - salida.front()) < paso * 0.5f)
		salida.pop_back();
	return salida;
}

void Superficie::PrepararCamino(Mundo& m, const std::vector<glm::vec2>& control)
{
	m.camino = MuestrearCamino(control, 2.0f);
	m.acumulado.resize(m.camino.size() + 1);
	m.acumulado[0] = 0.0f;
	for (size_t i = 0; i < m.camino.size(); i++)
		m.acumulado[i + 1] = m.acumulado[i] + glm::length(m.camino[(i + 1) % m.camino.size()] - m.camino[i]);
	m.largo = m.acumulado.back();
}

// Punto del camino más cercano a p
glm::vec2 Superficie::PuntoMasCercano(const Mundo& m, glm::vec2 p)
{
	float mejor = 1.0e30f;
	glm::vec2 resultado = m.camino[0];
	size_t n = m.camino.size();
	for (size_t i = 0; i < n; i++)
	{
		glm::vec2 a = m.camino[i], ab = m.camino[(i + 1) % n] - a;
		float t = glm::clamp(glm::dot(p - a, ab) / fmaxf(glm::dot(ab, ab), 1.0e-6f), 0.0f, 1.0f);
		glm::vec2 q = a + ab * t;
		float d = glm::length(p - q);
		if (d < mejor) { mejor = d; resultado = q; }
	}
	return resultado;
}

float Superficie::DistanciaAlCamino(const Mundo& m, glm::vec2 p)
{
	return glm::length(p - PuntoMasCercano(m, p));
}

// Posición y dirección del camino a 's' unidades del inicio de la vuelta
void Superficie::PuntoEnCamino(const Mundo& m, float s, glm::vec2& posicion, glm::vec2& tangente)
{
	s = fmodf(s, m.largo);
	if (s < 0.0f) s += m.largo;
	// último punto que está antes de s
	size_t i = 0;
	while (i + 1 < m.acumulado.size() && m.acumulado[i + 1] <= s)
		i++;
	if (i >= m.camino.size()) i = m.camino.size() - 1;
	glm::vec2 a = m.camino[i], b = m.camino[(i + 1) % m.camino.size()];
	float tramo = fmaxf(m.acumulado[i + 1] - m.acumulado[i], 1.0e-6f);
	posicion = a + (b - a) * ((s - m.acumulado[i]) / tramo);
	tangente = glm::normalize(b - a);
}

// Medio ancho del camino en la distancia s: varía un poco para que el borde no se vea de regla.
// Usa frecuencias enteras de la vuelta completa para que el final empate con el inicio.
float Superficie::AnchoEn(const Mundo& m, float s, float total)
{
	float fase = 6.2831853f * s / total;
	return m.anchoCamino * (1.0f + 0.12f * sinf(7.0f * fase) + 0.07f * sinf(19.0f * fase + 1.3f));
}

// Franja de triángulos a lo largo del camino. Vértices: x y z u v nx ny nz, con la normal hacia -Y
// porque este motor usa las normales invertidas (igual que Model.cpp y el piso de la práctica).
MeshModel* Superficie::CrearMallaCamino(const Mundo& m, float mosaico)
{
	size_t n = m.camino.size();
	float total = m.largo;
	float repeticiones = fmaxf(1.0f, roundf(total / mosaico));
	float mosaicoAjustado = total / repeticiones;   // número entero de repeticiones: la textura cierra sin salto

	std::vector<GLfloat> v;
	std::vector<unsigned int> indices;
	float s = 0.0f;
	for (size_t i = 0; i <= n; i++)
	{
		glm::vec2 p = m.camino[i % n];
		glm::vec2 tangente = glm::normalize(m.camino[(i + 1) % n] - m.camino[(i + n - 1) % n]);
		glm::vec2 izquierda(-tangente.y, tangente.x);
		float w = AnchoEn(m, s, total);
		glm::vec2 a = p + izquierda * w, b = p - izquierda * w;
		float coordV = s / mosaicoAjustado;
		float coordU = (2.0f * w) / mosaicoAjustado;
		GLfloat va[] = { a.x, ALTURA_CAMINO, a.y, 0.0f, coordV, 0.0f, -1.0f, 0.0f };
		GLfloat vb[] = { b.x, ALTURA_CAMINO, b.y, coordU, coordV, 0.0f, -1.0f, 0.0f };
		v.insert(v.end(), va, va + 8);
		v.insert(v.end(), vb, vb + 8);
		if (i < n)
		{
			s += glm::length(m.camino[(i + 1) % n] - p);
			unsigned int k = (unsigned int)(i * 2);
			unsigned int tri[] = { k, k + 1, k + 2, k + 1, k + 3, k + 2 };
			indices.insert(indices.end(), tri, tri + 6);
		}
	}
	MeshModel* malla = new MeshModel();
	malla->CreateMeshModel(v.data(), indices.data(), (unsigned int)v.size(), (unsigned int)indices.size());
	return malla;
}

// Piso: un rectángulo FIJO que cubre la zona del planeta, como el piso de la práctica pero más grande.
// Las UV salen de la posición en el mundo, así la textura se repite cada 'mosaico' unidades.
MeshModel* Superficie::CrearMallaPiso(glm::vec2 minimo, glm::vec2 maximo, float mosaico)
{
	unsigned int floorIndices[] = { 0, 1, 2, 0, 2, 3 };
	GLfloat floorVertices[] = {
		minimo.x, 0.0f, minimo.y,	minimo.x / mosaico, minimo.y / mosaico,	0.0f, -1.0f, 0.0f,
		maximo.x, 0.0f, minimo.y,	maximo.x / mosaico, minimo.y / mosaico,	0.0f, -1.0f, 0.0f,
		maximo.x, 0.0f, maximo.y,	maximo.x / mosaico, maximo.y / mosaico,	0.0f, -1.0f, 0.0f,
		minimo.x, 0.0f, maximo.y,	minimo.x / mosaico, maximo.y / mosaico,	0.0f, -1.0f, 0.0f,
	};
	MeshModel* malla = new MeshModel();
	malla->CreateMeshModel(floorVertices, floorIndices, 32, 6);
	return malla;
}

// Recorre el camino y guarda un punto cada 'paso' unidades (empezando en 'desfase') con su
// dirección y su izquierda. Con estos puntos se colocan los objetos a los lados del camino.
void Superficie::PuntosDelCamino(const Mundo& m, float paso, float desfase, std::vector<PuntoCamino>& puntos)
{
	puntos.clear();
	size_t n = m.camino.size();
	float siguiente = desfase, s = 0.0f;
	for (size_t i = 0; i < n; i++)
	{
		glm::vec2 a = m.camino[i], b = m.camino[(i + 1) % n];
		float largo = glm::length(b - a);
		while (siguiente <= s + largo)
		{
			PuntoCamino pc;
			pc.punto = a + (b - a) * ((siguiente - s) / fmaxf(largo, 1.0e-6f));
			pc.tangente = glm::normalize(b - a);
			pc.izquierda = glm::vec2(-pc.tangente.y, pc.tangente.x);
			puntos.push_back(pc);
			siguiente += paso;
		}
		s += largo;
	}
}

void Superficie::LimitesCamino(const Mundo& m, float margen, glm::vec2& minimo, glm::vec2& maximo)
{
	minimo = glm::vec2(1.0e9f);
	maximo = glm::vec2(-1.0e9f);
	for (size_t i = 0; i < m.camino.size(); i++)
	{
		minimo = glm::min(minimo, m.camino[i]);
		maximo = glm::max(maximo, m.camino[i]);
	}
	minimo -= glm::vec2(margen);
	maximo += glm::vec2(margen);
}

// Claro de aterrizaje (claros[0]): se le puede recortar el borde con 'margenAterrizaje' para que
// la avenida empiece en su orilla. Los demás claros (NPCs y portal) siempre quedan libres completos.
bool Superficie::Bloqueado(const Mundo& m, glm::vec2 q, float margenAterrizaje)
{
	if (!m.claros.empty() && glm::length(q - m.claros[0].centro) < m.claros[0].radio + margenAterrizaje)
		return true;
	for (size_t k = 1; k < m.claros.size(); k++)
		if (glm::length(q - m.claros[k].centro) < m.claros[k].radio)
			return true;
	return false;
}

// =============================================================================================
// Racimo de cristales hecho con código: prismas hexagonales con punta, abiertos como flor
// =============================================================================================
void Superficie::AgregarTrianguloCristal(std::vector<GLfloat>& v, std::vector<unsigned int>& indices, glm::vec3 base, glm::vec3 eje,
	glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float vBase)
{
	// cada cara con su propia normal (aspecto facetado)
	glm::vec3 nrm = glm::normalize(glm::cross(p1 - p0, p2 - p0));
	glm::vec3 centro = (p0 + p1 + p2) / 3.0f;
	glm::vec3 enEje = base + eje * glm::dot(centro - base, eje);
	if (glm::dot(nrm, centro - enEje) < 0.0f)
		nrm = -nrm;   // que apunte hacia afuera del cristal
	glm::vec3 p[3] = { p0, p1, p2 };
	unsigned int primero = (unsigned int)(v.size() / 8);
	for (int i = 0; i < 3; i++)
	{
		GLfloat datos[] = { p[i].x, p[i].y, p[i].z, (float)i * 0.5f, vBase,
			-nrm.x, -nrm.y, -nrm.z };   // normal invertida (convención del motor)
		v.insert(v.end(), datos, datos + 8);
		indices.push_back(primero + i);
	}
}

MeshModel* Superficie::CrearRacimoCristales(unsigned int semilla, int& numTriangulos)
{
	Aleatorio rnd(semilla);
	std::vector<GLfloat> v;
	std::vector<unsigned int> indices;
	int cantidad = 5 + (int)(rnd.uniforme() * 5.0f);   // 5 a 9 cristales

	for (int c = 0; c < cantidad; c++)
	{
		bool central = (c == 0);
		float radio = central ? rnd.rango(0.20f, 0.26f) : rnd.rango(0.09f, 0.19f);
		float alto = central ? rnd.rango(1.6f, 2.2f) : rnd.rango(0.6f, 1.5f);
		float punta = radio * rnd.rango(1.8f, 2.8f);
		float azimut = rnd.rango(0.0f, 6.2831853f);
		float inclinacion = central ? rnd.rango(0.0f, 0.12f) : rnd.rango(0.30f, 0.85f);
		float giro = rnd.rango(0.0f, 1.0472f);

		glm::vec3 base(cosf(azimut) * rnd.rango(0.05f, 0.30f), -0.15f, sinf(azimut) * rnd.rango(0.05f, 0.30f));
		glm::vec3 eje = glm::normalize(glm::vec3(cosf(azimut) * sinf(inclinacion), cosf(inclinacion), sinf(azimut) * sinf(inclinacion)));
		glm::vec3 e1 = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), eje));
		glm::vec3 e2 = glm::cross(eje, e1);

		glm::vec3 abajo[6], arriba[6];
		for (int k = 0; k < 6; k++)
		{
			float a = giro + k * 1.0471976f;
			glm::vec3 radial = (e1 * cosf(a) + e2 * sinf(a)) * radio;
			abajo[k] = base + radial;
			arriba[k] = base + radial + eje * alto;
		}
		glm::vec3 cima = base + eje * (alto + punta);

		for (int k = 0; k < 6; k++)
		{
			int k1 = (k + 1) % 6;
			AgregarTrianguloCristal(v, indices, base, eje, abajo[k], abajo[k1], arriba[k1], 0.0f);
			AgregarTrianguloCristal(v, indices, base, eje, abajo[k], arriba[k1], arriba[k], 0.5f);
			AgregarTrianguloCristal(v, indices, base, eje, arriba[k], arriba[k1], cima, 1.0f);
		}
	}
	numTriangulos = (int)(indices.size() / 3);
	MeshModel* malla = new MeshModel();
	malla->CreateMeshModel(v.data(), indices.data(), (unsigned int)v.size(), (unsigned int)indices.size());
	return malla;
}

// =============================================================================================
// Colocación de objetos. A cada objeto se le calcula UNA vez su matriz de modelo, con la misma
// forma de la práctica: model = glm::translate(model, ...); model = glm::rotate(...); model = glm::scale(...)
// =============================================================================================
void Superficie::AgregarObjeto(Mundo& m, TipoObjeto tipo, glm::vec2 p, float radio, glm::mat4 matriz,
	glm::vec3 color, int variante)
{
	Objeto o;
	o.tipo = tipo;
	o.posicion = p;
	o.radio = radio;
	o.matriz = matriz;
	o.color = color;
	o.variante = variante;
	o.brillaDeNoche = false;
	m.objetos.push_back(o);
}

// Árbol del modelo arbol.obj. 'esbeltez' = cuánto más alto que ancho respecto a su forma original.
// Regresa el radio aproximado del tronco (para que los troncos no se encimen).
float Superficie::AgregarArbol(Mundo& m, Aleatorio& rnd, TipoObjeto tipo, glm::vec2 p, float altura, float esbeltez)
{
	float alturaModelo = Arbol_M.estaCargado() ? Arbol_M.getTamano().y : 2.5f;
	float sy = altura / alturaModelo;
	float sxz = sy / esbeltez;
	// ancho en el mundo con las proporciones del árbol original (2.2 de ancho por 2.5 de alto):
	// así la distribución del bosque no cambia aunque el .obj se re-exporte con otra medida
	float anchoMundo = 2.2f * (altura / 2.5f) / esbeltez;
	float inclinacion = glm::radians(rnd.rango(0.0f, 4.0f));
	float dirInclinacion = rnd.rango(0.0f, 6.2831853f);
	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(p.x, -0.02f * altura, p.y));
	model = glm::rotate(model, inclinacion, glm::vec3(cosf(dirInclinacion), 0.0f, sinf(dirInclinacion)));
	model = glm::rotate(model, rnd.rango(0.0f, 6.2831853f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(sxz, sy, sxz));
	model = glm::translate(model, PiesDelModelo(Arbol_M));
	float tono = rnd.rango(0.82f, 1.05f);
	glm::vec3 color(tono * rnd.rango(0.90f, 1.0f), tono, tono * rnd.rango(0.85f, 0.97f));
	AgregarObjeto(m, tipo, p, 0.55f * anchoMundo, model, color, 0);
	float radioTronco = 0.13f * anchoMundo;
	if (tipo == OBJ_ARBOL)
		m.obstaculos.push_back({ p, radioTronco });
	return radioTronco;
}

// Roca hecha con el asteroide, normalizado a una caja de -1 a 1 en cada eje.
// 'semiejes' = media medida final en x, y, z; 'hundimiento' = fracción de la altura bajo el piso.
void Superficie::AgregarRoca(Mundo& m, Aleatorio& rnd, TipoObjeto tipo, glm::vec2 p, glm::vec3 semiejes,
	float hundimiento, glm::vec3 color, float inclinacionMaxima)
{
	glm::vec3 tamano = Roca_M.estaCargado() ? Roca_M.getTamano() * 0.5f : glm::vec3(1.0f);
	float inclinacion = glm::radians(rnd.rango(0.0f, inclinacionMaxima));
	float dirInclinacion = rnd.rango(0.0f, 6.2831853f);
	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(p.x, semiejes.y * (1.0f - 2.0f * hundimiento), p.y));
	model = glm::rotate(model, inclinacion, glm::vec3(cosf(dirInclinacion), 0.0f, sinf(dirInclinacion)));
	model = glm::rotate(model, rnd.rango(0.0f, 6.2831853f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, semiejes);
	model = glm::scale(model, 1.0f / glm::max(tamano, glm::vec3(0.001f)));	// la roca medida de -1 a 1
	model = glm::translate(model, -Roca_M.getCentro());
	AgregarObjeto(m, tipo, p, fmaxf(semiejes.x, semiejes.z), model, color, 0);
	float radioBase = 0.8f * fminf(semiejes.x, semiejes.z);
	if (tipo == OBJ_AGUJA || ((tipo == OBJ_ROCA || tipo == OBJ_ROCA_MUSGO) && radioBase > 1.0f))
		m.obstaculos.push_back({ p, radioBase });
}

void Superficie::AgregarCristal(Mundo& m, Aleatorio& rnd, glm::vec2 p, float escala)
{
	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(p.x, 0.0f, p.y));
	model = glm::rotate(model, rnd.rango(0.0f, 6.2831853f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(escala));
	float tono = rnd.rango(0.85f, 1.15f);
	float rosa = rnd.rango(0.0f, 0.35f);
	glm::vec3 color((0.58f + rosa) * tono, 0.42f * tono, (1.25f - 0.2f * rosa) * tono);
	AgregarObjeto(m, OBJ_CRISTAL, p, 0.9f * escala, model, color, (int)(rnd.uniforme() * 3.0f) % 3);
	m.obstaculos.push_back({ p, 0.45f * escala });
}

// Pone un NPC a un lado del camino, a 's' unidades del inicio de la vuelta. 'lado' = +1 o -1.
// Se separa del camino lo suficiente para que ni su cuerpo ni su movimiento lo invadan, y se le
// abre un claro (sin árboles ni rocas) del tamaño de su movimiento.
void Superficie::ColocarNPC(Mundo& m, ModeloNPC modelo, AnimacionNPC animacion, float s, float lado,
	float altura, float radio, float a, float b, float h, float omega, float alturaBase, float fase)
{
	glm::vec2 p, t;
	PuntoEnCamino(m, s, p, t);
	NPC n;
	n.modelo = modelo;
	n.animacion = animacion;
	n.altura = altura;
	n.adelante = t;
	n.afuera = glm::vec2(-t.y, t.x) * lado;
	n.a = a;
	n.b = b;
	n.h = h;
	n.omega = omega;
	n.alturaBase = alturaBase;
	n.fase = fase;
	n.radio = radio;
	// cuánto se mueve hacia los lados según su ecuación
	float lateral = 0.0f;
	if (animacion == ANIM_OCHO) lateral = 0.5f * b;            // sen th cos th vale a lo más 1/2
	else if (animacion == ANIM_REBOTE) lateral = a;
	else if (animacion == ANIM_ELIPSE || animacion == ANIM_LISSAJOUS) lateral = b;
	float separacion = m.anchoCamino + radio + lateral + 1.5f;
	n.centro = p + n.afuera * separacion;
	m.npcs.push_back(n);
	m.claros.push_back({ n.centro, radio + fmaxf(a, b) + 6.0f });   // holgado: que se vea desde el camino
	m.obstaculos.push_back({ n.centro, 0.7f * radio });
}

// Portal sobre el camino, de frente a la dirección en que se avanza
void Superficie::ColocarPortal(Mundo& m, float s, float altura)
{
	glm::vec2 p, t;
	PuntoEnCamino(m, s, p, t);
	m.portal.activo = true;
	m.portal.centro = p;
	m.portal.adelante = t;
	m.portal.altura = altura;
	m.portal.ancho = altura * (1.40f / 1.59f);   // proporciones del modelo del portal (1.40 x 1.59)
	glm::vec2 lado(-t.y, t.x);
	float columna = 0.42f * m.portal.ancho;       // las dos columnas del marco no se atraviesan
	m.obstaculos.push_back({ p + lado * columna, 0.08f * m.portal.ancho + 0.3f });
	m.obstaculos.push_back({ p - lado * columna, 0.08f * m.portal.ancho + 0.3f });
	m.claros.push_back({ p, 0.5f * m.portal.ancho + 3.0f });
}

// color de una roca del desierto con un poco de variación
glm::vec3 Superficie::ColorRocaAzar(Aleatorio& rnd)
{
	float t = rnd.rango(0.80f, 1.10f);
	return glm::vec3(COLOR_ROCA.x * t, COLOR_ROCA.y * t * rnd.rango(0.92f, 1.05f), COLOR_ROCA.z * t);
}

// grupo de 1 a 3 agujas de roca alrededor de un punto
void Superficie::GrupoAgujas(Mundo& m, Aleatorio& rnd, std::vector<glm::vec3>& ocupado, glm::vec2 centro, float alturaMaxima)
{
	int cantidad = 1 + (int)(rnd.uniforme() * 3.0f);
	for (int i = 0; i < cantidad; i++)
	{
		glm::vec2 q = centro + glm::vec2(rnd.rango(-5.0f, 5.0f), rnd.rango(-5.0f, 5.0f)) * (i == 0 ? 0.0f : 1.0f);
		float alto = alturaMaxima * (i == 0 ? 1.0f : rnd.rango(0.45f, 0.8f));
		float ancho = alto * rnd.rango(0.07f, 0.12f) + 0.9f;
		if (!Libre(ocupado, q, ancho * 0.7f) || Bloqueado(m, q, 0.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + 3.0f) continue;
		AgregarRoca(m, rnd, OBJ_AGUJA, q, glm::vec3(ancho, alto * 0.5f, ancho * rnd.rango(0.8f, 1.2f)),
			0.10f, ColorRocaAzar(rnd), 6.0f);
		ocupado.push_back(glm::vec3(q.x, q.y, ancho));
	}
}

// ---------------------------------------------------------------------------------------------
// Planeta selvático: camino con avenida de árboles, una franja de selva alrededor y NPCs
// ---------------------------------------------------------------------------------------------
void Superficie::GenerarSelva(Mundo& m)
{
	Aleatorio rnd(20271u);
	// vuelta cerrada: sale recto hacia -Z desde el claro y regresa por la derecha
	std::vector<glm::vec2> control = {
		{ 0.0f, 22.0f }, { 0.0f, 0.0f }, { 0.0f, -26.0f }, { 5.0f, -58.0f }, { -8.0f, -92.0f }, { 4.0f, -128.0f },
		{ 32.0f, -156.0f }, { 72.0f, -166.0f }, { 108.0f, -146.0f }, { 124.0f, -108.0f }, { 114.0f, -68.0f },
		{ 124.0f, -28.0f }, { 112.0f, 10.0f }, { 82.0f, 34.0f }, { 44.0f, 44.0f }, { 14.0f, 40.0f } };
	m.anchoCamino = 3.4f;
	PrepararCamino(m, control);
	m.limiteJugador = LIMITE_JUGADOR_SELVA;
	m.posCapsula = glm::vec3(-11.0f, 0.0f, -4.0f);
	m.posInicio = glm::vec3(0.0f, 0.0f, 14.0f);   // sobre el camino, mirando hacia donde avanza
	m.yawInicio = -90.0f;

	m.claros.push_back({ glm::vec2(0.0f), RADIO_CLARO });   // aterrizaje (siempre es claros[0])
	m.obstaculos.push_back({ glm::vec2(m.posCapsula.x, m.posCapsula.z), 2.6f });

	// --- NPCs: cerca uno del otro (cada 34 unidades), alternando lado, sin pisar el camino ---
	//          modelo              animación     s     lado  altura radio  a     b     h     omega alturaBase fase
	ColocarNPC(m, NPC_NARUTO,          ANIM_SALTOS,  36.0f, 1.0f, 4.2f, 1.3f, 0.0f, 0.0f, 1.6f, 2.1f, 0.0f, 0.0f);
	ColocarNPC(m, NPC_ZORRO_BLANCO,    ANIM_REBOTE,  70.0f,-1.0f, 2.2f, 1.2f, 1.2f, 0.0f, 0.5f, 1.4f, 0.0f, 0.0f);
	ColocarNPC(m, NPC_CABALLO_DELFIN,  ANIM_OCHO,   104.0f, 1.0f, 7.0f, 4.8f, 3.0f, 3.0f, 0.35f,0.55f,0.0f, 0.0f);
	ColocarNPC(m, NPC_CALAMARDO_GUAPO, ANIM_BAILE,  138.0f,-1.0f, 4.6f, 1.3f, 1.0f, 0.0f, 0.25f,2.0f, 0.0f, 0.0f);
	ColocarNPC(m, NPC_PULPO,           ANIM_RESPIRA,172.0f, 1.0f, 8.5f, 5.9f, 0.0f, 0.0f, 0.0f, 0.35f,0.0f, 0.0f);
	ColocarNPC(m, NPC_TIGRE_FUEGO,     ANIM_ELIPSE, 206.0f,-1.0f, 6.0f, 5.4f, 4.0f, 1.5f, 0.15f,0.45f,0.0f, 0.0f);
	ColocarPortal(m, 248.0f, 11.0f);   // un tramo después del último NPC

	std::vector<glm::vec3> troncos;   // (x, z, radio) de cada tronco ya colocado
	troncos.push_back(glm::vec3(m.posCapsula.x, m.posCapsula.z, 5.0f));
	const glm::vec3 colorMusgo(0.70f, 0.82f, 0.66f);
	std::vector<PuntoCamino> puntos;

	// 1) Las paredes del camino: árboles altos a ambos lados cuyas copas se juntan arriba
	PuntosDelCamino(m, 9.0f, 3.0f, puntos);
	for (size_t k = 0; k < puntos.size(); k++)
	{
		glm::vec2 c = puntos[k].punto, tangente = puntos[k].tangente, izq = puntos[k].izquierda;
		for (int lado = -1; lado <= 1; lado += 2)
		{
			glm::vec2 q = c + izq * ((float)lado * (m.anchoCamino + rnd.rango(3.4f, 4.6f))) + tangente * rnd.rango(-1.0f, 1.0f);
			if (Bloqueado(m, q, -6.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + 2.8f || !Libre(troncos, q, 2.5f)) continue;
			float tronco = AgregarArbol(m, rnd, OBJ_ARBOL, q, rnd.rango(30.0f, 46.0f), rnd.rango(1.35f, 1.65f));
			troncos.push_back(glm::vec3(q.x, q.y, tronco));
		}
	}

	// 2) Arbustos y rocas con musgo en la orilla del camino
	PuntosDelCamino(m, 3.0f, 1.0f, puntos);
	for (size_t k = 0; k < puntos.size(); k++)
	{
		glm::vec2 c = puntos[k].punto, tangente = puntos[k].tangente, izq = puntos[k].izquierda;
		float lado = (rnd.uniforme() < 0.5f) ? -1.0f : 1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(1.0f, 2.6f))) + tangente * rnd.rango(-1.0f, 1.0f);
		if (Bloqueado(m, q, -8.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + 0.7f || !Libre(troncos, q, 0.5f)) continue;
		float azar = rnd.uniforme();
		if (azar < 0.68f)
		{
			AgregarArbol(m, rnd, OBJ_ARBUSTO, q, rnd.rango(3.0f, 6.0f), rnd.rango(0.85f, 1.15f));
			m.objetos.back().brillaDeNoche = rnd.uniforme() < PROBABILIDAD_LUMINOSA_ORILLA;   // plantas luminosas
		}
		else if (azar < 0.82f)
		{
			float r = rnd.rango(1.0f, 2.4f);
			AgregarRoca(m, rnd, OBJ_ROCA_MUSGO, q, glm::vec3(r, r * rnd.rango(0.45f, 0.75f), r * rnd.rango(0.8f, 1.2f)),
				0.30f, colorMusgo * rnd.rango(0.85f, 1.1f), 10.0f);
		}
	}

	// 3) Orilla exterior de la franja: una fila cerrada de árboles grandes que cierra la vista
	PuntosDelCamino(m, 7.5f, 0.0f, puntos);
	for (size_t k = 0; k < puntos.size(); k++)
	{
		glm::vec2 c = puntos[k].punto, izq = puntos[k].izquierda;
		for (int lado = -1; lado <= 1; lado += 2)
		{
			glm::vec2 q = c + izq * ((float)lado * (m.anchoCamino + ANCHO_BOSQUE - rnd.rango(2.0f, 6.0f)));
			if (Bloqueado(m, q, 0.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + ANCHO_BOSQUE - 8.0f || !Libre(troncos, q, 2.5f)) continue;
			float tronco = AgregarArbol(m, rnd, OBJ_ARBOL, q, rnd.rango(30.0f, 44.0f), rnd.rango(1.25f, 1.5f));
			troncos.push_back(glm::vec3(q.x, q.y, tronco));
		}
	}

	// 4) La selva: solo en una franja de ANCHO_BOSQUE a cada lado del camino (zona finita)
	glm::vec2 minimo, maximo;
	LimitesCamino(m, ANCHO_BOSQUE + m.anchoCamino, minimo, maximo);
	const float limiteFranja = m.anchoCamino + ANCHO_BOSQUE;
	for (float x = minimo.x; x < maximo.x; x += CELDA_BOSQUE)
	{
		for (float z = minimo.y; z < maximo.y; z += CELDA_BOSQUE)
		{
			glm::vec2 q(x + rnd.rango(0.0f, CELDA_BOSQUE), z + rnd.rango(0.0f, CELDA_BOSQUE));
			float azar = rnd.uniforme();
			float dist = DistanciaAlCamino(m, q);
			if (azar < 0.88f && dist > m.anchoCamino + 10.0f && dist < limiteFranja && !Bloqueado(m, q, 0.0f) && Libre(troncos, q, 3.0f))
			{
				float tronco = AgregarArbol(m, rnd, OBJ_ARBOL, q, rnd.rango(22.0f, 42.0f), rnd.rango(1.25f, 1.65f));
				troncos.push_back(glm::vec3(q.x, q.y, tronco));
			}
			// árboles jóvenes, delgados y más bajos, entre los grandes
			glm::vec2 r(x + rnd.rango(0.0f, CELDA_BOSQUE), z + rnd.rango(0.0f, CELDA_BOSQUE));
			float distR = DistanciaAlCamino(m, r);
			if (rnd.uniforme() < 0.30f && distR > m.anchoCamino + 5.0f && distR < limiteFranja && !Bloqueado(m, r, 0.0f) && Libre(troncos, r, 1.5f))
			{
				float tronco = AgregarArbol(m, rnd, OBJ_ARBOL, r, rnd.rango(10.0f, 18.0f), rnd.rango(2.2f, 3.0f));
				troncos.push_back(glm::vec3(r.x, r.y, tronco));
			}
			// sotobosque: arbustos entre los troncos
			glm::vec2 s(x + rnd.rango(0.0f, CELDA_BOSQUE), z + rnd.rango(0.0f, CELDA_BOSQUE));
			float distS = DistanciaAlCamino(m, s);
			if (rnd.uniforme() < 0.25f && distS > m.anchoCamino + 1.0f && distS < limiteFranja && !Bloqueado(m, s, -4.0f) && Libre(troncos, s, 0.5f))
			{
				AgregarArbol(m, rnd, OBJ_ARBUSTO, s, rnd.rango(3.0f, 7.5f), rnd.rango(0.85f, 1.2f));
				m.objetos.back().brillaDeNoche = rnd.uniforme() < PROBABILIDAD_LUMINOSA_BOSQUE;
			}
		}
	}

	// 5) Rocas con musgo sueltas dentro de la franja
	for (float x = minimo.x; x < maximo.x; x += 26.0f)
	{
		for (float z = minimo.y; z < maximo.y; z += 26.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 26.0f), z + rnd.rango(0.0f, 26.0f));
			float dist = DistanciaAlCamino(m, q);
			if (rnd.uniforme() > 0.40f || dist < m.anchoCamino + 1.5f || dist > limiteFranja || Bloqueado(m, q, 0.0f) || !Libre(troncos, q, 2.0f)) continue;
			float r = rnd.rango(1.5f, 4.0f);
			AgregarRoca(m, rnd, OBJ_ROCA_MUSGO, q, glm::vec3(r, r * rnd.rango(0.45f, 0.8f), r * rnd.rango(0.8f, 1.2f)),
				0.30f, colorMusgo * rnd.rango(0.85f, 1.1f), 12.0f);
		}
	}

	// 6) Orilla del claro de aterrizaje y del primer NPC: arbustos que marcan el borde
	for (size_t k = 0; k < m.claros.size() && k < 2; k++)
	{
		for (int i = 0; i < 24; i++)
		{
			const Claro& claro = m.claros[k];
			float a = rnd.rango(0.0f, 6.2831853f);
			float r = rnd.rango(claro.radio - 3.0f, claro.radio + 3.0f);
			glm::vec2 q = claro.centro + glm::vec2(cosf(a), sinf(a)) * r;
			if (DistanciaAlCamino(m, q) < m.anchoCamino + 1.5f || !Libre(troncos, q, 0.5f)) continue;
			AgregarArbol(m, rnd, OBJ_ARBUSTO, q, rnd.rango(3.5f, 6.0f), rnd.rango(0.85f, 1.1f));
		}
	}

	m.mosaicoPiso = 5.0f;
	m.texturaPiso = &sueloSelvaTexture;
	m.texturaCamino = &sueloRocosoTexture;				// tierra apisonada en el camino
	m.colorPiso = glm::vec3(0.58f, 0.50f, 0.38f);		// hojarasca oscura y húmeda
	m.colorCamino = glm::vec3(0.86f, 0.77f, 0.63f);	// tierra pisada, más clara
}

// ---------------------------------------------------------------------------------------------
// Planeta rocoso: desierto rojo con agujas de roca, piedras y cristales violetas en el camino
// ---------------------------------------------------------------------------------------------
void Superficie::GenerarDesierto(Mundo& m)
{
	Aleatorio rnd(19461u);
	// vuelta cerrada: sale recto hacia -Z desde el claro y regresa por la izquierda
	std::vector<glm::vec2> control = {
		{ 0.0f, 22.0f }, { 0.0f, 0.0f }, { 0.0f, -26.0f }, { -10.0f, -60.0f }, { -2.0f, -98.0f }, { -28.0f, -134.0f },
		{ -68.0f, -154.0f }, { -110.0f, -136.0f }, { -130.0f, -98.0f }, { -118.0f, -58.0f }, { -130.0f, -18.0f },
		{ -112.0f, 18.0f }, { -76.0f, 38.0f }, { -40.0f, 44.0f }, { -12.0f, 40.0f } };
	m.anchoCamino = 4.2f;
	PrepararCamino(m, control);
	m.limiteJugador = LIMITE_JUGADOR_ROCOSO;
	m.posCapsula = glm::vec3(11.0f, 0.0f, -4.0f);
	m.posInicio = glm::vec3(0.0f, 0.0f, 14.0f);
	m.yawInicio = -90.0f;
	m.claros.push_back({ glm::vec2(0.0f), RADIO_CLARO });
	m.obstaculos.push_back({ glm::vec2(m.posCapsula.x, m.posCapsula.z), 2.6f });

	// --- NPCs: cerca uno del otro, alternando lado, sin pisar el camino ---
	//          modelo              animación       s     lado  altura radio  a     b     h     omega alturaBase fase
	ColocarNPC(m, NPC_MONSTRUO_NEGRO, ANIM_LEVITA,    36.0f, 1.0f, 3.6f, 1.2f, 0.0f, 0.0f, 0.6f, 1.1f, 1.0f, 0.0f);
	ColocarNPC(m, NPC_ESCORPION,      ANIM_OCHO,      70.0f,-1.0f, 1.5f, 3.9f, 2.0f, 2.0f, 0.05f,0.7f, 0.0f, 0.0f);
	ColocarNPC(m, NPC_CERBERO,        ANIM_ELIPSE,   104.0f, 1.0f, 6.0f, 3.8f, 5.0f, 1.5f, 0.12f,0.4f, 0.0f, 0.0f);
	ColocarNPC(m, NPC_SERPIENTE_ALAS, ANIM_LISSAJOUS,138.0f,-1.0f, 7.0f, 5.4f, 5.0f, 2.5f, 1.5f, 0.35f,7.0f, 0.0f);
	ColocarPortal(m, 180.0f, 11.0f);   // un tramo después del último NPC
	// Paul se esconde detrás del portal: se deja libre la zona donde se asoma
	glm::vec2 ladoPortal(-m.portal.adelante.y, m.portal.adelante.x);
	m.claros.push_back({ m.portal.centro + m.portal.adelante * 3.0f + ladoPortal * 4.0f, 8.0f });
	m.paulActivo = true;

	std::vector<glm::vec3> ocupado;   // (x, z, radio) de lo que ya se colocó
	ocupado.push_back(glm::vec3(m.posCapsula.x, m.posCapsula.z, 6.0f));
	const float franjaAgujas = m.anchoCamino + 95.0f;   // zona finita de agujas alrededor del camino
	const float franjaPiedras = m.anchoCamino + 60.0f;
	std::vector<PuntoCamino> puntos;

	// 1) Cristales a los lados del camino, alternando lado: marcan la ruta y brillan de noche
	int alterna = 0;
	PuntosDelCamino(m, 12.0f, 6.0f, puntos);
	for (size_t k = 0; k < puntos.size(); k++)
	{
		glm::vec2 c = puntos[k].punto, tangente = puntos[k].tangente, izq = puntos[k].izquierda;
		float lado = (alterna++ % 2 == 0) ? 1.0f : -1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(1.2f, 2.8f))) + tangente * rnd.rango(-2.0f, 2.0f);
		if (Bloqueado(m, q, -10.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + 0.9f) continue;
		AgregarCristal(m, rnd, q, rnd.rango(2.0f, 3.6f));
		ocupado.push_back(glm::vec3(q.x, q.y, 2.5f));
	}

	// 2) Agujas de roca cerca del camino, un poco más atrás que los cristales
	PuntosDelCamino(m, 28.0f, 10.0f, puntos);
	for (size_t k = 0; k < puntos.size(); k++)
	{
		glm::vec2 c = puntos[k].punto, izq = puntos[k].izquierda;
		float lado = (rnd.uniforme() < 0.5f) ? -1.0f : 1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(9.0f, 18.0f)));
		if (Bloqueado(m, q, 12.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + 7.0f) continue;
		GrupoAgujas(m, rnd, ocupado, q, rnd.rango(16.0f, 40.0f));
	}

	// 3) Piedras en la orilla del camino
	PuntosDelCamino(m, 5.0f, 2.0f, puntos);
	for (size_t k = 0; k < puntos.size(); k++)
	{
		glm::vec2 c = puntos[k].punto, izq = puntos[k].izquierda;
		if (rnd.uniforme() > 0.45f) continue;
		float lado = (rnd.uniforme() < 0.5f) ? -1.0f : 1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(0.8f, 3.5f)));
		if (Bloqueado(m, q, -10.0f) || DistanciaAlCamino(m, q) < m.anchoCamino + 0.6f || !Libre(ocupado, q, 0.8f)) continue;
		float r = rnd.rango(0.6f, 1.8f);
		AgregarRoca(m, rnd, OBJ_ROCA, q, glm::vec3(r, r * rnd.rango(0.45f, 0.75f), r * rnd.rango(0.8f, 1.2f)),
			0.30f, ColorRocaAzar(rnd), 15.0f);
	}

	// 4) Relleno, solo en una franja alrededor del camino: agujas, piedras y cristales grandes
	glm::vec2 minimo, maximo;
	LimitesCamino(m, franjaAgujas, minimo, maximo);
	for (float x = minimo.x; x < maximo.x; x += 24.0f)
	{
		for (float z = minimo.y; z < maximo.y; z += 24.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 24.0f), z + rnd.rango(0.0f, 24.0f));
			float dist = DistanciaAlCamino(m, q);
			if (rnd.uniforme() < 0.50f && dist > m.anchoCamino + 9.0f && dist < franjaAgujas && !Bloqueado(m, q, 12.0f))
				GrupoAgujas(m, rnd, ocupado, q, rnd.rango(12.0f, 44.0f));
		}
	}
	for (float x = minimo.x; x < maximo.x; x += 12.0f)
	{
		for (float z = minimo.y; z < maximo.y; z += 12.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 12.0f), z + rnd.rango(0.0f, 12.0f));
			float dist = DistanciaAlCamino(m, q);
			if (rnd.uniforme() > 0.40f || dist < m.anchoCamino + 1.0f || dist > franjaPiedras || Bloqueado(m, q, -6.0f)) continue;
			if (!Libre(ocupado, q, 1.0f)) continue;
			float r = rnd.rango(0.7f, 3.2f);
			AgregarRoca(m, rnd, OBJ_ROCA, q, glm::vec3(r, r * rnd.rango(0.4f, 0.75f), r * rnd.rango(0.8f, 1.2f)),
				0.30f, ColorRocaAzar(rnd), 15.0f);
		}
	}
	for (float x = minimo.x; x < maximo.x; x += 46.0f)
	{
		for (float z = minimo.y; z < maximo.y; z += 46.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 46.0f), z + rnd.rango(0.0f, 46.0f));
			float dist = DistanciaAlCamino(m, q);
			if (rnd.uniforme() > 0.35f || dist < m.anchoCamino + 6.0f || dist > franjaAgujas || Bloqueado(m, q, 0.0f)) continue;
			if (!Libre(ocupado, q, 3.0f)) continue;
			AgregarCristal(m, rnd, q, rnd.rango(4.0f, 8.0f));
			ocupado.push_back(glm::vec3(q.x, q.y, 4.0f));
		}
	}

	// 5) Montañas lejanas: siluetas que se pierden en el polvo y cierran la zona
	glm::vec2 centro = (minimo + maximo) * 0.5f;
	for (int i = 0; i < 28; i++)
	{
		float a = (i + rnd.rango(-0.3f, 0.3f)) * (6.2831853f / 28.0f);
		float r = rnd.rango(360.0f, 470.0f);
		glm::vec2 q = centro + glm::vec2(cosf(a), sinf(a)) * r;
		glm::vec3 semiejes(rnd.rango(55.0f, 100.0f), rnd.rango(28.0f, 70.0f), rnd.rango(55.0f, 100.0f));
		AgregarRoca(m, rnd, OBJ_MONTANA, q, semiejes, 0.35f, COLOR_ROCA * rnd.rango(0.65f, 0.85f), 4.0f);
	}

	m.mosaicoPiso = 9.0f;
	m.texturaPiso = &sueloRocosoTexture;
	m.texturaCamino = &sueloRocosoTexture;
	m.colorPiso = glm::vec3(1.05f, 0.56f, 0.40f);		// tierra rojiza
	m.colorCamino = glm::vec3(1.22f, 0.86f, 0.66f);	// arena más clara pisada
}

// =============================================================================================
// NUEVO (animación compleja): posición y orientación de cada NPC en el tiempo, con funciones
// =============================================================================================
glm::mat4 Superficie::MatrizNPC(const NPC& n, float tiempo, glm::vec2& posicion)
{
	const Model& modelo = modelosNPC[n.modelo];
	float escala = n.altura / fmaxf(modelo.getTamano().y, 0.001f);
	float th = n.omega * tiempo + n.fase;
	glm::vec2 desplazamiento(0.0f);   // movimiento sobre el piso respecto a su centro
	glm::vec2 velocidad(0.0f);        // derivada de la posición: hacia dónde mira
	float y = n.alturaBase, alabeo = 0.0f, cabeceo = 0.0f, respiracion = 1.0f;
	float yaw = atan2f(-n.afuera.x, -n.afuera.y);   // los que no caminan miran hacia el camino

	if (n.animacion == ANIM_SALTOS)
	{
		// cada periodo T = 2 pi / omega da un salto que dura D = 0.35 T: y = h sen(pi u / D)
		float periodo = 2.0f * PI_SUPERFICIE / n.omega;
		float u = fmodf(tiempo + n.fase, periodo), duracion = 0.35f * periodo;
		if (u < duracion)
			y += n.h * sinf(PI_SUPERFICIE * u / duracion);
		yaw += 0.35f * sinf(0.6f * tiempo + n.fase);           // voltea a los lados
		respiracion = 1.0f + 0.015f * sinf(2.0f * tiempo);
	}
	else if (n.animacion == ANIM_REBOTE)
	{
		// círculo de radio a: p = a (cos th, sen th); rebote y = h |sen 3th|
		desplazamiento = n.adelante * (n.a * cosf(th)) + n.afuera * (n.a * sinf(th));
		velocidad = n.adelante * (-n.a * sinf(th)) + n.afuera * (n.a * cosf(th));
		y += n.h * fabsf(sinf(3.0f * th));
	}
	else if (n.animacion == ANIM_OCHO)
	{
		// lemniscata de Gerono: p = (a sen th, b sen th cos th); trote y = h |sen 3th|
		desplazamiento = n.adelante * (n.a * sinf(th)) + n.afuera * (n.b * sinf(th) * cosf(th));
		velocidad = n.adelante * (n.a * cosf(th)) + n.afuera * (n.b * cosf(2.0f * th));
		y += n.h * fabsf(sinf(3.0f * th));
		cabeceo = 0.04f * sinf(6.0f * th);
	}
	else if (n.animacion == ANIM_RESPIRA)
	{
		// respiración y balanceo: escalaY = 1 + 0.04 sen(1.6 t); giro = 0.3 sen(omega t)
		respiracion = 1.0f + 0.04f * sinf(1.6f * tiempo + n.fase);
		alabeo = 0.04f * sinf(0.8f * tiempo + n.fase);
		yaw += 0.3f * sinf(th);
	}
	else if (n.animacion == ANIM_ELIPSE)
	{
		// elipse: p = (a cos th, b sen th); paso y = h |sen 4th|
		desplazamiento = n.adelante * (n.a * cosf(th)) + n.afuera * (n.b * sinf(th));
		velocidad = n.adelante * (-n.a * sinf(th)) + n.afuera * (n.b * cosf(th));
		y += n.h * fabsf(sinf(4.0f * th));
	}
	else if (n.animacion == ANIM_LEVITA)
	{
		// flota: y = y0 + h sen(omega t); gira y se mece despacio
		y += n.h * sinf(th);
		yaw += 0.6f * sinf(0.45f * tiempo + n.fase);
		alabeo = 0.08f * sinf(1.3f * tiempo + n.fase);
	}
	else if (n.animacion == ANIM_BAILE)
	{
		// baile: se mece a lo largo del camino y gira la cadera de un lado a otro
		desplazamiento = n.adelante * (n.a * sinf(th));
		y += n.h * fabsf(sinf(2.0f * th));
		yaw += 0.5f * sinf(2.0f * th);
		alabeo = 0.08f * sinf(th);
	}
	else if (n.animacion == ANIM_LISSAJOUS)
	{
		// curva de Lissajous: p = (a sen th, b sen 2th); y = y0 + h sen(0.9 t)
		desplazamiento = n.adelante * (n.a * sinf(th)) + n.afuera * (n.b * sinf(2.0f * th));
		velocidad = n.adelante * (n.a * cosf(th)) + n.afuera * (2.0f * n.b * cosf(2.0f * th));
		glm::vec2 aceleracion = n.adelante * (-n.a * sinf(th)) + n.afuera * (-4.0f * n.b * sinf(2.0f * th));
		y += n.h * sinf(0.9f * tiempo);
		// se inclina hacia el lado de la curva (aceleración lateral respecto a su derecha)
		float yawVuelo = atan2f(velocidad.x, velocidad.y);
		glm::vec2 derecha(cosf(yawVuelo), -sinf(yawVuelo));
		alabeo = -glm::clamp(0.06f * glm::dot(aceleracion, derecha), -0.45f, 0.45f);
	}
	if (n.animacion != ANIM_BAILE && glm::length(velocidad) > 1.0e-4f)
		yaw = atan2f(velocidad.x, velocidad.y);   // los modelos miran hacia +Z: se orientan con su velocidad

	posicion = n.centro + desplazamiento;
	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(posicion.x, y, posicion.y));
	model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));		// ángulos en radianes
	model = glm::rotate(model, alabeo, glm::vec3(0.0f, 0.0f, 1.0f));
	model = glm::rotate(model, cabeceo, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(escala, escala * respiracion, escala));
	model = glm::translate(model, PiesDelModelo(modelo));
	return model;
}

// =============================================================================================
void Superficie::InicializarSuperficies()
{
	if (inicializado)
		return;

	// texturas
	arbolTexture = Texture("Textures/arbolTexture.png");
	hayTexArbol = arbolTexture.LoadTextureA();
	sueloSelvaTexture = Texture("Textures/sueloSelva.png");
	sueloSelvaTexture.LoadTextureA();
	sueloRocosoTexture = Texture("Textures/sueloRocoso.png");
	sueloRocosoTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	// árbol y roca (el asteroide pequeño se usa como roca, aguja y montaña)
	Arbol_M = Model();
	Arbol_M.LoadModel("Models/arbol/arbol.obj");
	hayArbol = Arbol_M.estaCargado();
	Roca_M = Model();
	Roca_M.LoadModel("Models/asteroide/Rocky_Asteroid_6.obj");
	hayRoca = Roca_M.estaCargado();

	// NPCs del planeta selvático (cada .obj pesa decenas de MB: tardan en cargar)
	printf("Cargando los NPCs ...\n");
	modelosNPC[NPC_NARUTO] = Model();
	modelosNPC[NPC_NARUTO].LoadModel("Models/npcSelva/naruto.obj");
	modelosNPC[NPC_ZORRO_BLANCO] = Model();
	modelosNPC[NPC_ZORRO_BLANCO].LoadModel("Models/npcSelva/zorroBlanco.obj");
	modelosNPC[NPC_CABALLO_DELFIN] = Model();
	modelosNPC[NPC_CABALLO_DELFIN].LoadModel("Models/npcSelva/caballoDelfin.obj");
	modelosNPC[NPC_PULPO] = Model();
	modelosNPC[NPC_PULPO].LoadModel("Models/npcSelva/pulpo.obj");
	modelosNPC[NPC_TIGRE_FUEGO] = Model();
	modelosNPC[NPC_TIGRE_FUEGO].LoadModel("Models/npcSelva/tigreFuego.obj");
	modelosNPC[NPC_CALAMARDO_GUAPO] = Model();
	modelosNPC[NPC_CALAMARDO_GUAPO].LoadModel("Models/npcSelva/calamardoGuapo.obj");
	// NPCs del planeta rocoso
	modelosNPC[NPC_MONSTRUO_NEGRO] = Model();
	modelosNPC[NPC_MONSTRUO_NEGRO].LoadModel("Models/npcRoca/monstruoNegro.obj");
	modelosNPC[NPC_ESCORPION] = Model();
	modelosNPC[NPC_ESCORPION].LoadModel("Models/npcRoca/escorpion.obj");
	modelosNPC[NPC_CERBERO] = Model();
	modelosNPC[NPC_CERBERO].LoadModel("Models/npcRoca/Cerbero.obj");
	modelosNPC[NPC_SERPIENTE_ALAS] = Model();
	modelosNPC[NPC_SERPIENTE_ALAS].LoadModel("Models/npcRoca/serpienteAlas.obj");
	modelosNPC[NPC_PAUL] = Model();
	modelosNPC[NPC_PAUL].LoadModel("Models/npcRoca/Paul.obj");
	for (int i = 0; i < NUM_MODELOS_NPC; i++)
		hayNPC[i] = modelosNPC[i].estaCargado();

	// portal de salida (el archivo se llama asteroide.obj pero tiene el portal)
	Portal_M = Model();
	Portal_M.LoadModel("Models/portal/asteroide.obj");
	hayPortal = Portal_M.estaCargado();

	// Si Paul.obj trae la misma malla que el portal (en Blender se exportó el objeto equivocado)
	// se dibujaría un segundo portal: en ese caso Paul no se usa hasta que se re-exporte.
	paulValido = hayNPC[NPC_PAUL];
	if (paulValido && hayPortal && modelosNPC[NPC_PAUL].getNumTriangulos() == Portal_M.getNumTriangulos()
		&& glm::length(modelosNPC[NPC_PAUL].getTamano() - Portal_M.getTamano()) < 0.01f)
	{
		printf("[AVISO] Models/npcRoca/Paul.obj tiene la misma malla que el portal. No se dibuja.\n");
		paulValido = false;
	}

	// los tres racimos de cristales hechos con código
	for (int i = 0; i < 3; i++)
		mallasCristal[i] = CrearRacimoCristales(777u + 131u * (unsigned int)i, triangulosCristal[i]);

	GenerarSelva(mundos[SUPERFICIE_SELVATICA]);
	GenerarDesierto(mundos[SUPERFICIE_ROCOSA]);

	for (int i = 0; i < 2; i++)
	{
		Mundo& m = mundos[i];
		glm::vec2 minimo, maximo;
		LimitesCamino(m, (i == SUPERFICIE_SELVATICA) ? MARGEN_PISO_SELVA : MARGEN_PISO_ROCOSO, minimo, maximo);
		m.mallaPiso = CrearMallaPiso(minimo, maximo, m.mosaicoPiso);
		m.mallaCamino = CrearMallaCamino(m, (i == SUPERFICIE_SELVATICA) ? 4.0f : 6.0f);
		printf("[OK] Superficie %s: camino de %.0f unidades, %d objetos, %d NPCs\n",
			(i == SUPERFICIE_SELVATICA) ? "selvatica" : "rocosa", m.largo, (int)m.objetos.size(), (int)m.npcs.size());
	}
	inicializado = true;
}

// NUEVO: recorte por cono. Solo se dibuja lo que está cerca y frente a la cámara (un cono horizontal
// un poco más ancho que la vista); con cientos de árboles es lo que mantiene el programa fluido.
bool Superficie::Visible(glm::vec2 posicion, float radio, float distanciaMaxima, float& dist)
{
	glm::vec2 d = posicion - camaraPiso;
	dist = glm::length(d);
	if (dist - radio > distanciaMaxima)
		return false;
	if (usarCono && dist > radio)
	{
		float proyeccion = glm::dot(d, direccionPiso);
		float lateral = sqrtf(fmaxf(dist * dist - proyeccion * proyeccion, 0.0f));
		if (lateral > proyeccion * tanCono + radio / cosCono)
			return false;
	}
	return true;
}

void Superficie::DibujarObjeto(const Objeto& o)
{
	// se cambia el material solo cuando cambia el tipo de objeto
	if (o.tipo != tipoActual)
	{
		if (o.tipo == OBJ_CRISTAL) Material_cristal.UseMaterial(uniformSpecularIntensity, uniformShininess);
		else if (o.tipo == OBJ_ARBOL || o.tipo == OBJ_ARBUSTO) Material_arbol.UseMaterial(uniformSpecularIntensity, uniformShininess);
		else Material_roca.UseMaterial(uniformSpecularIntensity, uniformShininess);
		tipoActual = o.tipo;
	}
	if (o.tipo == OBJ_CRISTAL)
	{
		// los cristales brillan un poco de día y mucho de noche
		glUniform3f(uniformEmision, o.color.x * 0.55f * brilloNocturno, o.color.y * 0.45f * brilloNocturno, o.color.z * 0.65f * brilloNocturno);
		emisionActiva = true;
	}
	else if (o.brillaDeNoche)
	{
		// planta luminosa: brillo suave verde azulado
		glUniform3f(uniformEmision, 0.10f * brilloNocturno, 0.55f * brilloNocturno, 0.45f * brilloNocturno);
		emisionActiva = true;
	}
	else if (emisionActiva)
	{
		glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
		emisionActiva = false;
	}
	glUniform3f(uniformColor, o.color.x, o.color.y, o.color.z);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(o.matriz));

	if (o.tipo == OBJ_ARBOL || o.tipo == OBJ_ARBUSTO)
	{
		if (!hayArbol)
			return;
		Arbol_M.RenderModel();
		triangulos += Arbol_M.getNumTriangulos();
	}
	else if (o.tipo == OBJ_ROCA_MUSGO)
	{
		Roca_M.RenderModelConTextura(hayTexArbol ? arbolTexture : plainTexture);   // la textura de hojas parece musgo
		triangulos += Roca_M.getNumTriangulos();
	}
	else if (o.tipo == OBJ_AGUJA || o.tipo == OBJ_ROCA || o.tipo == OBJ_MONTANA)
	{
		Roca_M.RenderModelConTextura(sueloRocosoTexture);
		triangulos += Roca_M.getNumTriangulos();
	}
	else if (o.tipo == OBJ_CRISTAL)
	{
		plainTexture.UseTexture();
		mallasCristal[o.variante]->RenderMeshModel();
		triangulos += triangulosCristal[o.variante];
	}
}

void Superficie::DibujarSuperficie(TipoSuperficie tipo, glm::vec3 posCamara, glm::vec3 dirCamara, float factorDia, float tiempo,
	GLuint modelLocation, GLuint colorLocation, GLuint offsetLocation,
	GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation)
{
	if (!inicializado)
		return;
	uniformModel = modelLocation;
	uniformColor = colorLocation;
	uniformTextureOffset = offsetLocation;
	uniformSpecularIntensity = specularIntensityLocation;
	uniformShininess = shininessLocation;
	uniformEmision = emisionLocation;

	Mundo& m = mundos[tipo];
	float dt = glm::clamp(tiempo - tiempoAnterior, 0.0f, 0.1f);
	tiempoAnterior = tiempo;
	glm::mat4 model(1.0);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	// --- 1) Piso fijo (cubre toda la zona del planeta) ---
	glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
	glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
	Material_piso.UseMaterial(uniformSpecularIntensity, uniformShininess);
	m.texturaPiso->UseTexture();
	glUniform3f(uniformColor, m.colorPiso.x, m.colorPiso.y, m.colorPiso.z);
	model = glm::mat4(1.0);
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	m.mallaPiso->RenderMeshModel();

	// --- 2) Camino. NUEVO: glPolygonOffset lo "adelanta" en profundidad para que no parpadee
	//        con el piso (los dos están casi a la misma altura) ---
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -4.0f);
	m.texturaCamino->UseTexture();
	glUniform3f(uniformColor, m.colorCamino.x, m.colorCamino.y, m.colorCamino.z);
	m.mallaCamino->RenderMeshModel();
	glDisable(GL_POLYGON_OFFSET_FILL);

	// datos del recorte por cono (ver Visible)
	camaraPiso = glm::vec2(posCamara.x, posCamara.z);
	direccionPiso = glm::vec2(dirCamara.x, dirCamara.z);
	float largoDireccion = glm::length(direccionPiso);
	usarCono = largoDireccion > 0.25f;   // mirando casi vertical no se recorta por ángulo
	if (usarCono)
		direccionPiso /= largoDireccion;
	tanCono = tanf(glm::radians(ANGULO_CONO));
	cosCono = cosf(glm::radians(ANGULO_CONO));

	triangulos = 0.0;
	float dist;

	// --- 3) NPCs (animación compleja, ver MatrizNPC) ---
	Material_npc.UseMaterial(uniformSpecularIntensity, uniformShininess);
	glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < m.npcs.size(); i++)
	{
		const NPC& n = m.npcs[i];
		if (!hayNPC[n.modelo])
			continue;
		glm::vec2 posicion;
		model = MatrizNPC(n, tiempo, posicion);
		if (!Visible(posicion, n.radio + fmaxf(n.a, n.b), DISTANCIA_NPC, dist))
			continue;
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		modelosNPC[n.modelo].RenderModel();
	}

	// --- Portal: brilla con un pulso (emisión = base * (0.6 + 0.4 sen(2.2 t))) ---
	if (hayPortal && m.portal.activo && Visible(m.portal.centro, 0.5f * m.portal.ancho, DISTANCIA_NPC, dist))
	{
		float escala = m.portal.altura / fmaxf(Portal_M.getTamano().y, 0.001f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(m.portal.centro.x, 0.0f, m.portal.centro.y));
		// el frente del modelo (+Z, donde está la galaxia) mira hacia quien llega por el camino
		model = glm::rotate(model, atan2f(-m.portal.adelante.x, -m.portal.adelante.y), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(escala));
		model = glm::translate(model, PiesDelModelo(Portal_M));
		float pulso = 0.6f + 0.4f * sinf(2.2f * tiempo);
		glUniform3f(uniformEmision, 0.45f * pulso, 0.50f * pulso, 0.75f * pulso);   // la galaxia del portal brilla
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Portal_M.RenderModel();
		glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
	}

	// --- Paul: escondido detrás del portal; se asoma cuando miras hacia el portal ---
	if (paulValido && m.paulActivo && m.portal.activo)
	{
		glm::vec2 haciaPortal = m.portal.centro - camaraPiso;
		float distanciaPortal = glm::length(haciaPortal);
		bool mirando = usarCono && distanciaPortal > 1.0f && distanciaPortal < 110.0f
			&& glm::dot(haciaPortal / distanciaPortal, direccionPiso) > cosf(glm::radians(20.0f));
		// mientras miras el portal se asoma y se esconde (lo decide sen(1.4 t)); si no lo miras, se esconde
		float objetivo = (mirando && sinf(1.4f * tiempo) > -0.2f) ? 1.0f : 0.0f;
		revelacionPaul += (objetivo - revelacionPaul) * fminf(1.0f, 4.0f * dt);
		float r = Suave(revelacionPaul);

		glm::vec2 lado(-m.portal.adelante.y, m.portal.adelante.x);
		glm::vec2 pos = m.portal.centro + m.portal.adelante * 2.5f + lado * (r * (0.5f * m.portal.ancho + 1.5f));
		float yaw = atan2f(-m.portal.adelante.x, -m.portal.adelante.y);   // mira hacia quien llega
		glm::vec2 ejeX(cosf(yaw), -sinf(yaw));                               // su derecha en el mundo
		float inclinacion = -0.22f * r * (glm::dot(lado, ejeX) > 0.0f ? 1.0f : -1.0f);   // se ladea al asomarse
		const Model& paul = modelosNPC[NPC_PAUL];
		float escala = 4.4f / fmaxf(paul.getTamano().y, 0.001f);
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(pos.x, 0.12f * r * fabsf(sinf(3.0f * tiempo)), pos.y));
		model = glm::rotate(model, yaw, glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::rotate(model, inclinacion, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(escala));
		model = glm::translate(model, PiesDelModelo(paul));
		Material_npc.UseMaterial(uniformSpecularIntensity, uniformShininess);
		glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		modelosNPC[NPC_PAUL].RenderModel();
	}

	// --- 4) Objetos: del más cercano al más lejano, sin pasar del presupuesto de triángulos ---
	visibles.clear();
	for (size_t i = 0; i < m.objetos.size(); i++)
	{
		const Objeto& o = m.objetos[i];
		if (o.tipo != OBJ_MONTANA && Visible(o.posicion, o.radio, DISTANCIA_MAXIMA[o.tipo], dist))
			visibles.push_back(std::make_pair(dist, (int)i));
	}
	std::sort(visibles.begin(), visibles.end());

	// los cristales y las plantas luminosas brillan un poco de día y mucho de noche
	brilloNocturno = 0.06f + 0.75f * (1.0f - factorDia);
	tipoActual = -1;
	emisionActiva = false;
	for (size_t i = 0; i < visibles.size(); i++)
	{
		if (triangulos > PRESUPUESTO_TRIANGULOS)
			break;   // lo que falta está más lejos que todo lo ya dibujado
		DibujarObjeto(m.objetos[visibles[i].second]);
	}

	// montañas: siempre (son pocas y están tan lejos que la niebla decide cuánto se ven)
	for (size_t i = 0; i < m.objetos.size(); i++)
		if (m.objetos[i].tipo == OBJ_MONTANA)
			DibujarObjeto(m.objetos[i]);

	glUniform3f(uniformEmision, 0.0f, 0.0f, 0.0f);
}

glm::vec3 Superficie::LimitarPosicion(TipoSuperficie tipo, glm::vec3 posicion)
{
	const Mundo& m = mundos[tipo];
	if (!inicializado || m.camino.empty())
		return posicion;
	glm::vec2 p(posicion.x, posicion.z);

	// 1) obstáculos: si el jugador quedó dentro de un tronco, roca o cristal, se le saca por la orilla
	//    (dos pasadas por si quedó entre dos obstáculos)
	for (int pasada = 0; pasada < 2; pasada++)
	{
		for (size_t i = 0; i < m.obstaculos.size(); i++)
		{
			const Obstaculo& o = m.obstaculos[i];
			glm::vec2 d = p - o.centro;
			float minimo = o.radio + RADIO_JUGADOR;
			if (fabsf(d.x) > minimo || fabsf(d.y) > minimo)
				continue;   // descarte rápido
			float distancia = glm::length(d);
			if (distancia < minimo)
				p = o.centro + (distancia > 1.0e-4f ? d / distancia : glm::vec2(1.0f, 0.0f)) * minimo;
		}
	}

	// 2) zona: no puede alejarse del camino más de limiteJugador
	glm::vec2 cercano = PuntoMasCercano(m, p);
	glm::vec2 d = p - cercano;
	float distancia = glm::length(d);
	if (distancia > m.limiteJugador)
		p = cercano + d * (m.limiteJugador / distancia);   // se queda en la orilla de la zona
	return glm::vec3(p.x, posicion.y, p.y);
}

// Carrera dentro del planeta: del punto de inicio hasta un poco después del portal
float Superficie::InicioCarrera(const Mundo& m)
{
	return 8.0f;   // posInicio está a 8 unidades del principio del camino
}

float Superficie::LargoCarreraSuperficie(TipoSuperficie tipo)
{
	const Mundo& m = mundos[tipo];
	float sPortal = m.portal.activo ? 0.0f : m.largo * 0.5f;
	if (m.portal.activo)
	{
		// distancia del camino donde quedó el portal
		float mejor = 1.0e30f;
		for (size_t i = 0; i < m.camino.size(); i++)
		{
			float d = glm::length(m.camino[i] - m.portal.centro);
			if (d < mejor) { mejor = d; sPortal = m.acumulado[i]; }
		}
	}
	return sPortal + 5.0f - InicioCarrera(m);
}

void Superficie::PuntoCarreraSuperficie(TipoSuperficie tipo, float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante)
{
	const Mundo& m = mundos[tipo];
	glm::vec2 p, t;
	PuntoEnCamino(m, InicioCarrera(m) + distancia, p, t);
	glm::vec2 lado(-t.y, t.x);
	glm::vec2 q = p + lado * carril;
	posicion = glm::vec3(q.x, altura, q.y);
	adelante = glm::vec3(t.x, 0.0f, t.y);
}

glm::vec3 Superficie::AjustarACamino(TipoSuperficie tipo, glm::vec3 posicion, float altura, glm::vec3& adelante)
{
	const Mundo& m = mundos[tipo];
	glm::vec2 q(posicion.x, posicion.z);
	float mejor = 1.0e30f, sMejor = 0.0f;
	for (size_t i = 0; i < m.camino.size(); i++)
	{
		glm::vec2 a = m.camino[i], b = m.camino[(i + 1) % m.camino.size()];
		glm::vec2 ab = b - a;
		float f = glm::clamp(glm::dot(q - a, ab) / fmaxf(glm::dot(ab, ab), 1.0e-6f), 0.0f, 1.0f);
		float d = glm::length(q - (a + ab * f));
		if (d < mejor) { mejor = d; sMejor = m.acumulado[i] + f * (m.acumulado[i + 1] - m.acumulado[i]); }
	}
	glm::vec2 p, t;
	PuntoEnCamino(m, sMejor, p, t);
	glm::vec2 lado(-t.y, t.x);
	float lateral = glm::clamp(glm::dot(q - p, lado), -(m.anchoCamino - 1.5f), m.anchoCamino - 1.5f);
	glm::vec2 r = p + lado * lateral;
	adelante = glm::vec3(t.x, 0.0f, t.y);
	return glm::vec3(r.x, altura, r.y);
}

Model* Superficie::ModeloPaul()
{
	return (inicializado && paulValido) ? &modelosNPC[NPC_PAUL] : nullptr;
}

bool Superficie::CruzandoPortal(TipoSuperficie tipo, glm::vec3 posicion)
{
	const Portal& portal = mundos[tipo].portal;
	if (!inicializado || !portal.activo)
		return false;
	if (posicion.y > portal.altura)
		return false;   // con la cámara libre, pasar volando por encima no cuenta
	// posición del jugador medida en los ejes del portal: a lo largo del camino y hacia los lados
	glm::vec2 d = glm::vec2(posicion.x, posicion.z) - portal.centro;
	glm::vec2 lado(-portal.adelante.y, portal.adelante.x);
	float alFrente = glm::dot(d, portal.adelante);
	float alLado = glm::dot(d, lado);
	return fabsf(alFrente) < 1.4f && fabsf(alLado) < 0.30f * portal.ancho;   // dentro del hueco del marco
}

AmbienteSuperficie Superficie::ObtenerAmbiente(TipoSuperficie tipo, float factorDia)
{
	AmbienteSuperficie a;
	float luz = 0.10f + 0.90f * factorDia;
	if (tipo == SUPERFICIE_SELVATICA)
	{
		// bruma húmeda verdosa
		a.colorNiebla = glm::vec3(0.52f, 0.64f, 0.46f) * luz;
		a.densidadNiebla = 1.0f / 160.0f;
		a.tinteCielo = glm::vec3(0.55f, 0.85f, 0.62f) * (0.45f + 0.55f * factorDia);
		a.brumaHorizonte = glm::vec4(a.colorNiebla, 0.90f);
	}
	else
	{
		// polvo rojizo del desierto; de noche se ve el cielo estrellado
		a.colorNiebla = glm::vec3(0.72f, 0.38f, 0.24f) * luz;
		a.densidadNiebla = 1.0f / 300.0f;
		a.tinteCielo = glm::vec3(1.0f, 0.80f, 0.82f) * (0.55f + 0.45f * factorDia);
		a.brumaHorizonte = glm::vec4(a.colorNiebla, 0.85f);
	}
	return a;
}

glm::vec3 Superficie::PosicionCapsula(TipoSuperficie tipo)
{
	return mundos[tipo].posCapsula;
}

glm::vec3 Superficie::PosicionInicio(TipoSuperficie tipo)
{
	return mundos[tipo].posInicio;
}

float Superficie::YawInicio(TipoSuperficie tipo)
{
	return mundos[tipo].yawInicio;
}

Superficie::~Superficie()
{
}
