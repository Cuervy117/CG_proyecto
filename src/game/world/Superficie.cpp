#include "Superficie.h"

#include <cmath>
#include <cstdio>
#include <vector>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Model.h"
#include "Texture.h"
#include "Mesh_tn.h"
#include "Material.h"

// =============================================================================================
// Recursos. Se prueba cada ruta en orden y se usa la primera que exista.
// Las texturas se buscan por nombre: si el .mtl exportado por Blender dice "texture_diffuse.png"
// y esa imagen no existe, el modelo usa la textura indicada aquí (Model::SetTexturaRespaldo).
// =============================================================================================
static const char* RUTAS_ARBOL[] = { "Models/arbol/arbol.obj", "Models/arbol.obj" };
static const char* RUTAS_TEXTURA_ARBOL[] = { "Textures/arbolTexture.png", "Models/arbol/arbolTexture.png" };
static const char* RUTAS_ROCA[] = { "Models/asteroide/Rocky_Asteroid_6.obj" };
static const char* RUTA_SUELO_SELVA = "Textures/sueloSelva.png";
static const char* RUTA_SUELO_ROCOSO = "Textures/sueloRocoso.png";
static const char* RUTA_PLAIN = "Textures/plain.png";

// Modelos de los NPCs (carpetas Models/npcSelva y Models/npcRoca) con su textura
enum ModeloNPC
{
	NPC_NARUTO = 0,        // selva
	NPC_ZORRO_BLANCO,
	NPC_CABALLO_DELFIN,
	NPC_PULPO,
	NPC_TIGRE_FUEGO,
	NPC_CALAMARDO_GUAPO,
	NPC_MONSTRUO_NEGRO,    // roca
	NPC_ESCORPION,
	NPC_CERBERO,
	NPC_SERPIENTE_ALAS,
	NPC_PAUL,              // se esconde detrás del portal del planeta rocoso
	NUM_MODELOS_NPC
};
struct DefinicionNPC
{
	const char* rutas[2];
	const char* textura;
	const char* nombre;
};
static const DefinicionNPC DEFINICION_NPC[NUM_MODELOS_NPC] = {
	{ { "Models/npcSelva/naruto.obj", nullptr }, "Textures/narutoTexture.png", "naruto" },
	{ { "Models/npcSelva/zorroBlanco.obj", nullptr }, "Textures/zorroBlancoTexture.png", "zorro blanco" },
	{ { "Models/npcSelva/caballoDelfin.obj", nullptr }, "Textures/caballoDelfinTexture.png", "caballo-delfin" },
	{ { "Models/npcSelva/pulpo.obj", nullptr }, "Textures/pulpoTexture.png", "pulpo" },
	{ { "Models/npcSelva/tigreFuego.obj", nullptr }, "Textures/tigreFuegoTexture.png", "tigre de fuego" },
	{ { "Models/npcSelva/calamardoGuapo.obj", nullptr }, "Textures/calamardoGuapoTexture.png", "calamardo guapo" },
	{ { "Models/npcRoca/monstruoNegro.obj", nullptr }, "Textures/monstruoNegroTexture.png", "monstruo negro" },
	{ { "Models/npcRoca/escorpion.obj", nullptr }, "Textures/escorpionTexture.png", "escorpion" },
	{ { "Models/npcRoca/Cerbero.obj", nullptr }, "Textures/cerberoTexture.png", "cerbero" },
	{ { "Models/npcRoca/serpienteAlas.obj", "Models/npcSelva/serpienteAlas.obj" }, "Textures/serpienteAlasTexture.png", "serpiente alada" },
	{ { "Models/npcRoca/Paul.obj", nullptr }, "Textures/PaulTexture.png", "Paul" },
};

// Portal de salida (al cruzarlo regresas al espacio). En la carpeta venía como asteroide.obj.
static const char* RUTAS_PORTAL[] = { "Models/portal/portal.obj", "Models/portal/asteroide.obj" };
static const char* RUTA_TEXTURA_PORTAL = "Textures/portalTexture.png";

// =============================================================================================
// Ajustes. Todo el mundo es FINITO: una zona alrededor del camino.
// Si el programa va lento, sube CELDA_BOSQUE (menos árboles) o baja DISTANCIA_MAXIMA[OBJ_ARBOL].
// =============================================================================================
static const float RADIO_CLARO = 24.0f;            // claro sin árboles donde aterriza la cápsula
static const float ANCHO_BOSQUE = 40.0f;           // franja de selva a cada lado del camino
static const float CELDA_BOSQUE = 12.0f;           // separación media entre árboles de la franja
static const float LIMITE_JUGADOR_SELVA = 20.0f;   // qué tanto puede alejarse el jugador del camino
static const float LIMITE_JUGADOR_ROCOSO = 75.0f;
static const float MARGEN_PISO_SELVA = 300.0f;     // piso más allá del camino (la niebla tapa la orilla)
static const float MARGEN_PISO_ROCOSO = 650.0f;    // en el desierto el piso llega hasta las montañas
static const float ALTURA_CAMINO = 0.02f;          // el camino va apenas sobre el piso
static const double PRESUPUESTO_TRIANGULOS = 4500000.0;   // tope de triángulos por cuadro (seguridad)
static const float ANGULO_CONO = 62.0f;            // medio ángulo horizontal que se dibuja (algo más que la vista)
static const float DISTANCIA_NPC = 260.0f;
// Plantas luminosas (arbustos que brillan de noche, como las "plantas luminosas" del boceto).
// Ponle 0 para quitarlas.
static const float PROBABILIDAD_LUMINOSA_ORILLA = 0.0f;   // desactivadas: eran los árboles "todos verdes"
static const float PROBABILIDAD_LUMINOSA_BOSQUE = 0.0f;
static const float RADIO_JUGADOR = 0.9f;           // para no meterse en troncos ni rocas

enum TipoObjeto
{
	OBJ_ARBOL = 0,
	OBJ_ARBUSTO,
	OBJ_ROCA_MUSGO,
	OBJ_AGUJA,
	OBJ_ROCA,
	OBJ_CRISTAL,
	OBJ_MONTANA,
	NUM_TIPOS_OBJETO
};

// distancia máxima a la que se dibuja cada tipo (la niebla tapa lo que queda más lejos)
static const float DISTANCIA_MAXIMA[NUM_TIPOS_OBJETO] = { 150.0f, 70.0f, 100.0f, 320.0f, 160.0f, 220.0f, 1.0e9f };

struct Objeto
{
	TipoObjeto tipo;
	glm::vec2 posicion;   // posición en el piso (x, z)
	float radio;          // radio aproximado, para recortar por distancia y ángulo
	glm::mat4 matriz;     // matriz de modelo completa (se calcula una sola vez)
	glm::vec3 color;
	int variante;         // cristales: qué racimo se usa
	bool brillaDeNoche;   // plantas luminosas de la selva
};

// Cada NPC se mueve poco alrededor de su lugar, con una ecuación (función del tiempo t).
// th = omega * t + fase. "adelante" es la dirección del camino y "afuera" la que se aleja de él.
enum AnimacionNPC
{
	ANIM_SALTOS = 0,  // salto de medio seno cada periodo:      y = h * sen(pi * u / D)  si u < D
	ANIM_REBOTE,      // círculo pequeño rebotando:              p = a (cos th, sen th),  y = h |sen 3th|
	ANIM_OCHO,        // trote en un ocho (lemniscata de Gerono): p = (a sen th, b sen th cos th)
	ANIM_RESPIRA,     // respira y se balancea en su lugar:      escalaY = 1 + 0.04 sen(1.6 t)
	ANIM_ELIPSE,      // ronda en una elipse pequeña:            p = (a cos th, b sen th)
	ANIM_LEVITA,      // flota y gira:                           y = y0 + h sen(omega t)
	ANIM_LISSAJOUS,   // vuela en un ocho de Lissajous:          p = (a sen th, b sen 2th)
	ANIM_BAILE        // se mece y gira de lado a lado:          x = a sen th, y = h |sen 2th|, giro = 0.5 sen 2th
};

struct NPC
{
	ModeloNPC modelo;
	AnimacionNPC animacion;
	float altura;         // altura del modelo en el mundo
	glm::vec2 centro;     // punto fijo alrededor del cual se mueve (a un lado del camino)
	glm::vec2 adelante;   // dirección del camino en ese punto
	glm::vec2 afuera;     // dirección que se aleja del camino
	float a, b, h;        // amplitudes: a lo largo del camino, hacia afuera y vertical
	float omega;          // velocidad angular (radianes por segundo)
	float alturaBase;     // altura sobre el piso (los que vuelan o flotan)
	float fase;
	float radio;          // radio aproximado de su huella (para claros, recorte y obstáculos)
};

// Portal de salida: está sobre el camino, poco después del último NPC
struct Portal
{
	bool activo = false;
	glm::vec2 centro = glm::vec2(0.0f);
	glm::vec2 adelante = glm::vec2(0.0f, 1.0f);   // el portal se cruza en esta dirección
	float altura = 11.0f;
	float ancho = 9.7f;                          // ancho total del marco
};

struct Claro
{
	glm::vec2 centro;
	float radio;
};

// Obstáculo para el jugador: un círculo en el piso (tronco, aguja, cristal, cápsula...)
struct Obstaculo
{
	glm::vec2 centro;
	float radio;
};

struct Mundo
{
	std::vector<glm::vec2> camino;      // centro del camino, un punto cada 2 unidades (curva cerrada)
	std::vector<float> acumulado;       // distancia recorrida hasta cada punto del camino
	float largo = 0.0f;
	float anchoCamino = 3.0f;           // medio ancho
	float limiteJugador = 40.0f;
	std::vector<Claro> claros;          // zonas sin árboles (aterrizaje, claro de la serpiente...)
	std::vector<Obstaculo> obstaculos;  // lo que el jugador no puede atravesar
	MeshModel* mallaCamino = nullptr;
	MeshModel* mallaPiso = nullptr;
	float mosaicoPiso = 8.0f;           // unidades de mundo que ocupa una repetición de la textura
	Texture* texturaPiso = nullptr;
	Texture* texturaCamino = nullptr;
	glm::vec3 colorPiso = glm::vec3(1.0f);
	glm::vec3 colorCamino = glm::vec3(1.0f);
	std::vector<Objeto> objetos;
	std::vector<NPC> npcs;
	Portal portal;
	bool paulActivo = false;      // Paul se asoma detrás del portal de este planeta
	glm::vec3 posCapsula = glm::vec3(0.0f);
	glm::vec3 posInicio = glm::vec3(0.0f, 0.0f, 20.0f);
	float yawInicio = -90.0f;
};

static Mundo mundos[2];
static bool inicializado = false;

static Model arbol, roca;
static bool hayArbol = false, hayRoca = false;
static Model modelosNPC[NUM_MODELOS_NPC];
static bool hayNPC[NUM_MODELOS_NPC] = {};
static Texture texturasNPC[NUM_MODELOS_NPC];
static Model modeloPortal;
static bool paulValido = false;       // false si Paul.obj no se cargó o trae la malla equivocada
static float revelacionPaul = 0.0f;   // 0 = escondido detrás del portal, 1 = asomado
static Texture texturaPortal;
static bool hayPortal = false;
static Texture texSueloSelva, texSueloRocoso, texArbol, texPlain;
static bool hayTexArbol = false;
static MeshModel* mallasCristal[3] = { nullptr, nullptr, nullptr };
static int triangulosCristal[3] = { 0, 0, 0 };
static int objetosUltimoCuadro = 0;
static double triangulosUltimoCuadro = 0.0;

static Material matPiso(0.15f, 8.0f);
static Material matArbol(0.25f, 16.0f);
static Material matRoca(0.35f, 16.0f);
static Material matCristal(3.0f, 128.0f);
static Material matNPC(0.5f, 32.0f);

static const float PI_F = 3.14159265f;

// =============================================================================================
// Utilidades
// =============================================================================================

// Generador pseudoaleatorio propio (xorshift): con la misma semilla da los mismos números en
// cualquier compilador, así el mapa siempre sale igual (sirve para el croquis y el manual).
struct Aleatorio
{
	unsigned int estado;
	explicit Aleatorio(unsigned int semilla) : estado(semilla ? semilla : 0x9E3779B9u) {}
	unsigned int siguiente()
	{
		estado ^= estado << 13;
		estado ^= estado >> 17;
		estado ^= estado << 5;
		return estado;
	}
	float uniforme() { return (float)(siguiente() >> 8) * (1.0f / 16777216.0f); }   // [0, 1)
	float rango(float a, float b) { return a + (b - a) * uniforme(); }
};

// Rejilla para saber rápido si un punto choca con objetos ya colocados (troncos, NPCs)
struct RejillaEspacial
{
	float celda;
	std::unordered_map<long long, std::vector<glm::vec3>> mapa;   // (x, z, radio)

	explicit RejillaEspacial(float tamanoCelda) : celda(tamanoCelda) {}
	long long clave(int i, int j) const { return ((long long)i << 32) ^ (long long)(unsigned int)j; }

	bool libre(const glm::vec2& p, float radio) const
	{
		int ci = (int)floorf(p.x / celda), cj = (int)floorf(p.y / celda);
		for (int i = ci - 2; i <= ci + 2; i++)
			for (int j = cj - 2; j <= cj + 2; j++)
			{
				auto it = mapa.find(clave(i, j));
				if (it == mapa.end()) continue;
				for (const glm::vec3& o : it->second)
					if (glm::length(glm::vec2(o.x, o.y) - p) < o.z + radio)
						return false;
			}
		return true;
	}
	void agregar(const glm::vec2& p, float radio)
	{
		mapa[clave((int)floorf(p.x / celda), (int)floorf(p.y / celda))].push_back(glm::vec3(p.x, p.y, radio));
	}
};

static bool existeArchivoSuperficie(const char* ruta)
{
	if (ruta == nullptr)
		return false;
	std::ifstream f(ruta);
	return f.good();
}

// Carga una textura; si falla usa plain.png. Regresa true si cargó la imagen pedida.
static bool cargarTextura(Texture& textura, const char* ruta)
{
	textura = Texture(ruta);
	if (textura.LoadTextureA())
		return true;
	printf("[AVISO] No se pudo cargar %s, se usa %s\n", ruta, RUTA_PLAIN);
	textura = Texture(RUTA_PLAIN);
	textura.LoadTextureA();
	return false;
}

// Carga el primer archivo que exista de 'rutas'. Si su .mtl no encontró la imagen (pasa cuando
// Blender exporta "texture_diffuse.png" y la imagen ya tiene otro nombre), se le asigna 'textura'.
static bool cargarModelo(Model& modelo, const char* const* rutas, int cantidad, const char* nombre,
	Texture* textura, const char* rutaTextura)
{
	for (int i = 0; i < cantidad; i++)
	{
		if (!existeArchivoSuperficie(rutas[i]))
			continue;
		printf("Cargando %s ...\n", rutas[i]);
		modelo = Model();
		modelo.LoadModel(rutas[i]);
		if (!modelo.estaCargado())
			continue;
		printf("[OK] %s: %s (%d triangulos)\n", nombre, rutas[i], modelo.getNumTriangulos());
		if (textura != nullptr && !modelo.usaTexturaPropia() && existeArchivoSuperficie(rutaTextura))
		{
			cargarTextura(*textura, rutaTextura);
			modelo.SetTexturaRespaldo(textura);
			printf("     su .mtl no encontro la imagen: se usa %s\n", rutaTextura);
		}
		return true;
	}
	printf("[ERROR] No se encontro el modelo de %s (se busco primero en %s)\n", nombre, rutas[0]);
	return false;
}

// Matriz que pone la base del modelo en el origen: pies en y = 0 y centrado en x, z
static glm::mat4 matrizBase(const Model& modelo)
{
	glm::vec3 c = modelo.getCentro(), t = modelo.getTamano();
	return glm::translate(glm::mat4(1.0f), glm::vec3(-c.x, -(c.y - 0.5f * t.y), -c.z));
}

static float suave(float x)
{
	x = glm::clamp(x, 0.0f, 1.0f);
	return x * x * (3.0f - 2.0f * x);
}

// =============================================================================================
// Camino: curva Catmull-Rom cerrada que pasa por los puntos de control
// =============================================================================================
static glm::vec2 catmullRom(const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2, const glm::vec2& p3, float t)
{
	float t2 = t * t, t3 = t2 * t;
	return 0.5f * ((2.0f * p1) + (p2 - p0) * t + (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2
		+ (3.0f * p1 - p0 - 3.0f * p2 + p3) * t3);
}

// Curva cerrada remuestreada a distancia constante 'paso'
static std::vector<glm::vec2> muestrearCamino(const std::vector<glm::vec2>& control, float paso)
{
	std::vector<glm::vec2> denso;
	int n = (int)control.size();
	for (int i = 0; i < n; i++)
	{
		const glm::vec2& p0 = control[(i - 1 + n) % n];
		const glm::vec2& p1 = control[i];
		const glm::vec2& p2 = control[(i + 1) % n];
		const glm::vec2& p3 = control[(i + 2) % n];
		for (int k = 0; k < 40; k++)
			denso.push_back(catmullRom(p0, p1, p2, p3, k / 40.0f));
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

static void prepararCamino(Mundo& m, const std::vector<glm::vec2>& control)
{
	m.camino = muestrearCamino(control, 2.0f);
	m.acumulado.resize(m.camino.size() + 1);
	m.acumulado[0] = 0.0f;
	for (size_t i = 0; i < m.camino.size(); i++)
		m.acumulado[i + 1] = m.acumulado[i] + glm::length(m.camino[(i + 1) % m.camino.size()] - m.camino[i]);
	m.largo = m.acumulado.back();
}

// Punto del camino más cercano a p
static glm::vec2 puntoMasCercano(const Mundo& m, const glm::vec2& p)
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

static float distanciaAlCamino(const Mundo& m, const glm::vec2& p)
{
	return glm::length(p - puntoMasCercano(m, p));
}

// Posición y dirección del camino a 's' unidades del inicio de la vuelta
static void puntoEnCamino(const Mundo& m, float s, glm::vec2& posicion, glm::vec2& tangente)
{
	s = fmodf(s, m.largo);
	if (s < 0.0f) s += m.largo;
	size_t i = (size_t)(std::upper_bound(m.acumulado.begin(), m.acumulado.end(), s) - m.acumulado.begin());
	i = (i == 0) ? 0 : i - 1;
	if (i >= m.camino.size()) i = m.camino.size() - 1;
	glm::vec2 a = m.camino[i], b = m.camino[(i + 1) % m.camino.size()];
	float tramo = fmaxf(m.acumulado[i + 1] - m.acumulado[i], 1.0e-6f);
	posicion = a + (b - a) * ((s - m.acumulado[i]) / tramo);
	tangente = glm::normalize(b - a);
}

// Medio ancho del camino en la distancia s: varía un poco para que el borde no se vea de regla.
// Usa frecuencias enteras de la vuelta completa para que el final empate con el inicio.
static float anchoEn(const Mundo& m, float s, float total)
{
	float fase = 6.2831853f * s / total;
	return m.anchoCamino * (1.0f + 0.12f * sinf(7.0f * fase) + 0.07f * sinf(19.0f * fase + 1.3f));
}

static float largoCamino(const Mundo& m)
{
	return m.largo;
}

// Franja de triángulos a lo largo del camino. Vértices: x y z u v nx ny nz, con la normal hacia -Y
// porque este motor usa las normales invertidas (igual que Model.cpp y el piso del curso).
static MeshModel* crearMallaCamino(const Mundo& m, float mosaico)
{
	size_t n = m.camino.size();
	float total = largoCamino(m);
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
		float w = anchoEn(m, s, total);
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

// Piso: un rectángulo FIJO que cubre la zona del planeta (ya no sigue a la cámara).
// Las UV salen de la posición en el mundo, así la textura se repite cada 'mosaico' unidades.
static MeshModel* crearMallaPiso(const glm::vec2& minimo, const glm::vec2& maximo, float mosaico)
{
	GLfloat v[] = {
		minimo.x, 0.0f, minimo.y,   minimo.x / mosaico, minimo.y / mosaico,   0.0f, -1.0f, 0.0f,
		maximo.x, 0.0f, minimo.y,   maximo.x / mosaico, minimo.y / mosaico,   0.0f, -1.0f, 0.0f,
		maximo.x, 0.0f, maximo.y,   maximo.x / mosaico, maximo.y / mosaico,   0.0f, -1.0f, 0.0f,
		minimo.x, 0.0f, maximo.y,   minimo.x / mosaico, maximo.y / mosaico,   0.0f, -1.0f, 0.0f,
	};
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	MeshModel* malla = new MeshModel();
	malla->CreateMeshModel(v, indices, 32, 6);
	return malla;
}

// =============================================================================================
// Racimo de cristales hecho con código: prismas hexagonales con punta, abiertos como flor
// =============================================================================================
static MeshModel* crearRacimoCristales(unsigned int semilla, int& triangulos)
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

		// cada cara con su propia normal (aspecto facetado)
		auto agregarTriangulo = [&](const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, float vBase)
		{
			glm::vec3 nrm = glm::normalize(glm::cross(p1 - p0, p2 - p0));
			glm::vec3 centro = (p0 + p1 + p2) / 3.0f;
			glm::vec3 enEje = base + eje * glm::dot(centro - base, eje);
			if (glm::dot(nrm, centro - enEje) < 0.0f)
				nrm = -nrm;   // que apunte hacia afuera del cristal
			const glm::vec3* p[3] = { &p0, &p1, &p2 };
			unsigned int primero = (unsigned int)(v.size() / 8);
			for (int i = 0; i < 3; i++)
			{
				GLfloat datos[] = { p[i]->x, p[i]->y, p[i]->z, (float)i * 0.5f, vBase,
					-nrm.x, -nrm.y, -nrm.z };   // normal invertida (convención del motor)
				v.insert(v.end(), datos, datos + 8);
				indices.push_back(primero + i);
			}
		};
		for (int k = 0; k < 6; k++)
		{
			int k1 = (k + 1) % 6;
			agregarTriangulo(abajo[k], abajo[k1], arriba[k1], 0.0f);
			agregarTriangulo(abajo[k], arriba[k1], arriba[k], 0.5f);
			agregarTriangulo(arriba[k], arriba[k1], cima, 1.0f);
		}
	}
	triangulos = (int)(indices.size() / 3);
	MeshModel* malla = new MeshModel();
	malla->CreateMeshModel(v.data(), indices.data(), (unsigned int)v.size(), (unsigned int)indices.size());
	return malla;
}

// =============================================================================================
// Colocación de objetos
// =============================================================================================
static void agregarObjeto(Mundo& m, TipoObjeto tipo, const glm::vec2& p, float radio, const glm::mat4& matriz,
	const glm::vec3& color, int variante = 0)
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
static float agregarArbol(Mundo& m, Aleatorio& rnd, TipoObjeto tipo, const glm::vec2& p, float altura, float esbeltez)
{
	float alturaModelo = arbol.estaCargado() ? arbol.getTamano().y : 2.5f;
	float sy = altura / alturaModelo;
	float sxz = sy / esbeltez;
	// ancho en el mundo con las proporciones del árbol original (2.2 de ancho por 2.5 de alto):
	// así la distribución del bosque no cambia aunque el .obj se re-exporte con otra medida
	float anchoMundo = 2.2f * (altura / 2.5f) / esbeltez;
	float inclinacion = glm::radians(rnd.rango(0.0f, 4.0f));
	float dirInclinacion = rnd.rango(0.0f, 6.2831853f);
	glm::mat4 matriz = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, -0.02f * altura, p.y))
		* glm::rotate(glm::mat4(1.0f), inclinacion, glm::vec3(cosf(dirInclinacion), 0.0f, sinf(dirInclinacion)))
		* glm::rotate(glm::mat4(1.0f), rnd.rango(0.0f, 6.2831853f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(sxz, sy, sxz))
		* matrizBase(arbol);
	float tono = rnd.rango(0.82f, 1.05f);
	glm::vec3 color(tono * rnd.rango(0.90f, 1.0f), tono, tono * rnd.rango(0.85f, 0.97f));
	agregarObjeto(m, tipo, p, 0.55f * anchoMundo, matriz, color);
	float radioTronco = 0.13f * anchoMundo;
	if (tipo == OBJ_ARBOL)
		m.obstaculos.push_back({ p, radioTronco });
	return radioTronco;
}

// Roca hecha con el asteroide, normalizado a una caja de -1 a 1 en cada eje.
// 'semiejes' = media medida final en x, y, z; 'hundimiento' = fracción de la altura bajo el piso.
static void agregarRoca(Mundo& m, Aleatorio& rnd, TipoObjeto tipo, const glm::vec2& p, const glm::vec3& semiejes,
	float hundimiento, const glm::vec3& color, float inclinacionMaxima)
{
	glm::vec3 tamano = roca.estaCargado() ? roca.getTamano() * 0.5f : glm::vec3(1.0f);
	glm::mat4 normalizar = glm::scale(glm::mat4(1.0f), 1.0f / glm::max(tamano, glm::vec3(0.001f)))
		* glm::translate(glm::mat4(1.0f), -roca.getCentro());
	float inclinacion = glm::radians(rnd.rango(0.0f, inclinacionMaxima));
	float dirInclinacion = rnd.rango(0.0f, 6.2831853f);
	glm::mat4 matriz = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, semiejes.y * (1.0f - 2.0f * hundimiento), p.y))
		* glm::rotate(glm::mat4(1.0f), inclinacion, glm::vec3(cosf(dirInclinacion), 0.0f, sinf(dirInclinacion)))
		* glm::rotate(glm::mat4(1.0f), rnd.rango(0.0f, 6.2831853f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), semiejes)
		* normalizar;
	agregarObjeto(m, tipo, p, fmaxf(semiejes.x, semiejes.z), matriz, color);
	float radioBase = 0.8f * fminf(semiejes.x, semiejes.z);
	if (tipo == OBJ_AGUJA || ((tipo == OBJ_ROCA || tipo == OBJ_ROCA_MUSGO) && radioBase > 1.0f))
		m.obstaculos.push_back({ p, radioBase });
}

static void agregarCristal(Mundo& m, Aleatorio& rnd, const glm::vec2& p, float escala)
{
	glm::mat4 matriz = glm::translate(glm::mat4(1.0f), glm::vec3(p.x, 0.0f, p.y))
		* glm::rotate(glm::mat4(1.0f), rnd.rango(0.0f, 6.2831853f), glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(escala));
	float tono = rnd.rango(0.85f, 1.15f);
	float rosa = rnd.rango(0.0f, 0.35f);
	glm::vec3 color((0.58f + rosa) * tono, 0.42f * tono, (1.25f - 0.2f * rosa) * tono);
	agregarObjeto(m, OBJ_CRISTAL, p, 0.9f * escala, matriz, color, (int)(rnd.uniforme() * 3.0f) % 3);
	m.obstaculos.push_back({ p, 0.45f * escala });
}

// Recorre el camino y llama a 'accion' cada 'paso' unidades con el punto, la tangente y la izquierda
template <typename Funcion>
static void recorrerCamino(const Mundo& m, float paso, float desfase, Funcion accion)
{
	size_t n = m.camino.size();
	float siguiente = desfase, s = 0.0f;
	for (size_t i = 0; i < n; i++)
	{
		glm::vec2 a = m.camino[i], b = m.camino[(i + 1) % n];
		float largo = glm::length(b - a);
		while (siguiente <= s + largo)
		{
			glm::vec2 p = a + (b - a) * ((siguiente - s) / fmaxf(largo, 1.0e-6f));
			glm::vec2 tangente = glm::normalize(b - a);
			accion(p, tangente, glm::vec2(-tangente.y, tangente.x));
			siguiente += paso;
		}
		s += largo;
	}
}

static void limitesCamino(const Mundo& m, float margen, glm::vec2& minimo, glm::vec2& maximo)
{
	minimo = glm::vec2(1.0e9f);
	maximo = glm::vec2(-1.0e9f);
	for (const glm::vec2& p : m.camino)
	{
		minimo = glm::min(minimo, p);
		maximo = glm::max(maximo, p);
	}
	minimo -= glm::vec2(margen);
	maximo += glm::vec2(margen);
}

// Claro de aterrizaje (claros[0]): se le puede recortar el borde con 'margenAterrizaje' para que
// la avenida empiece en su orilla. Los demás claros (NPCs y portal) siempre quedan libres completos.
static bool bloqueado(const Mundo& m, const glm::vec2& q, float margenAterrizaje)
{
	if (!m.claros.empty() && glm::length(q - m.claros[0].centro) < m.claros[0].radio + margenAterrizaje)
		return true;
	for (size_t k = 1; k < m.claros.size(); k++)
		if (glm::length(q - m.claros[k].centro) < m.claros[k].radio)
			return true;
	return false;
}

// Pone un NPC a un lado del camino, a 's' unidades del inicio de la vuelta. 'lado' = +1 o -1.
// Se separa del camino lo suficiente para que ni su cuerpo ni su movimiento lo invadan, y se le
// abre un claro (sin árboles ni rocas) del tamaño de su movimiento.
static void colocarNPC(Mundo& m, ModeloNPC modelo, AnimacionNPC animacion, float s, float lado,
	float altura, float radio, float a, float b, float h, float omega, float alturaBase, float fase)
{
	glm::vec2 p, t;
	puntoEnCamino(m, s, p, t);
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
static void colocarPortal(Mundo& m, float s, float altura)
{
	glm::vec2 p, t;
	puntoEnCamino(m, s, p, t);
	m.portal.activo = true;
	m.portal.centro = p;
	m.portal.adelante = t;
	m.portal.altura = altura;
	m.portal.ancho = altura * (1.40f / 1.59f);   // proporciones del modelo portal (1.40 x 1.59)
	glm::vec2 lado(-t.y, t.x);
	float columna = 0.42f * m.portal.ancho;       // las dos columnas del marco no se atraviesan
	m.obstaculos.push_back({ p + lado * columna, 0.08f * m.portal.ancho + 0.3f });
	m.obstaculos.push_back({ p - lado * columna, 0.08f * m.portal.ancho + 0.3f });
	m.claros.push_back({ p, 0.5f * m.portal.ancho + 3.0f });
}

// ---------------------------------------------------------------------------------------------
// Planeta selvático: camino con avenida de árboles, una franja de selva alrededor y NPCs
// ---------------------------------------------------------------------------------------------
static void generarSelva(Mundo& m)
{
	Aleatorio rnd(20271u);
	// vuelta cerrada: sale recto hacia -Z desde el claro y regresa por la derecha
	std::vector<glm::vec2> control = {
		{ 0.0f, 22.0f }, { 0.0f, 0.0f }, { 0.0f, -26.0f }, { 5.0f, -58.0f }, { -8.0f, -92.0f }, { 4.0f, -128.0f },
		{ 32.0f, -156.0f }, { 72.0f, -166.0f }, { 108.0f, -146.0f }, { 124.0f, -108.0f }, { 114.0f, -68.0f },
		{ 124.0f, -28.0f }, { 112.0f, 10.0f }, { 82.0f, 34.0f }, { 44.0f, 44.0f }, { 14.0f, 40.0f } };
	m.anchoCamino = 3.4f;
	prepararCamino(m, control);
	m.limiteJugador = LIMITE_JUGADOR_SELVA;
	m.posCapsula = glm::vec3(-11.0f, 0.0f, -4.0f);
	m.posInicio = glm::vec3(0.0f, 0.0f, 14.0f);   // sobre el camino, mirando hacia donde avanza
	m.yawInicio = -90.0f;

	m.claros.push_back({ glm::vec2(0.0f), RADIO_CLARO });   // aterrizaje (siempre es claros[0])
	m.obstaculos.push_back({ glm::vec2(m.posCapsula.x, m.posCapsula.z), 2.6f });

	// --- NPCs: cerca uno del otro (cada 28 unidades), alternando lado, sin pisar el camino ---
	//          modelo              animación     s     lado  altura radio  a     b     h     omega fase
	colocarNPC(m, NPC_NARUTO,          ANIM_SALTOS,  36.0f, 1.0f, 4.2f, 1.3f, 0.0f, 0.0f, 1.6f, 2.1f, 0.0f, 0.0f);
	colocarNPC(m, NPC_ZORRO_BLANCO,    ANIM_REBOTE,  70.0f,-1.0f, 2.2f, 1.2f, 1.2f, 0.0f, 0.5f, 1.4f, 0.0f, 0.0f);
	colocarNPC(m, NPC_CABALLO_DELFIN,  ANIM_OCHO,   104.0f, 1.0f, 7.0f, 4.8f, 3.0f, 3.0f, 0.35f,0.55f,0.0f, 0.0f);
	colocarNPC(m, NPC_CALAMARDO_GUAPO, ANIM_BAILE,  138.0f,-1.0f, 4.6f, 1.3f, 1.0f, 0.0f, 0.25f,2.0f, 0.0f, 0.0f);
	colocarNPC(m, NPC_PULPO,           ANIM_RESPIRA,172.0f, 1.0f, 8.5f, 5.9f, 0.0f, 0.0f, 0.0f, 0.35f,0.0f, 0.0f);
	colocarNPC(m, NPC_TIGRE_FUEGO,     ANIM_ELIPSE, 206.0f,-1.0f, 6.0f, 5.4f, 4.0f, 1.5f, 0.15f,0.45f,0.0f, 0.0f);
	colocarPortal(m, 248.0f, 11.0f);   // un tramo después del último NPC

	RejillaEspacial troncos(12.0f);
	troncos.agregar(glm::vec2(m.posCapsula.x, m.posCapsula.z), 5.0f);
	const glm::vec3 colorMusgo(0.70f, 0.82f, 0.66f);

	// 1) Las paredes del camino: árboles altos a ambos lados cuyas copas se juntan arriba
	recorrerCamino(m, 9.0f, 3.0f, [&](const glm::vec2& c, const glm::vec2& tan, const glm::vec2& izq)
	{
		for (int lado = -1; lado <= 1; lado += 2)
		{
			glm::vec2 q = c + izq * ((float)lado * (m.anchoCamino + rnd.rango(3.4f, 4.6f))) + tan * rnd.rango(-1.0f, 1.0f);
			if (bloqueado(m, q, -6.0f) || distanciaAlCamino(m, q) < m.anchoCamino + 2.8f || !troncos.libre(q, 2.5f)) continue;
			float tronco = agregarArbol(m, rnd, OBJ_ARBOL, q, rnd.rango(30.0f, 46.0f), rnd.rango(1.35f, 1.65f));
			troncos.agregar(q, tronco);
		}
	});

	// 2) Arbustos (algunos luminosos) y rocas con musgo en la orilla del camino
	recorrerCamino(m, 3.0f, 1.0f, [&](const glm::vec2& c, const glm::vec2& tan, const glm::vec2& izq)
	{
		float lado = (rnd.uniforme() < 0.5f) ? -1.0f : 1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(1.0f, 2.6f))) + tan * rnd.rango(-1.0f, 1.0f);
		if (bloqueado(m, q, -8.0f) || distanciaAlCamino(m, q) < m.anchoCamino + 0.7f || !troncos.libre(q, 0.5f)) return;
		float azar = rnd.uniforme();
		if (azar < 0.68f)
		{
			agregarArbol(m, rnd, OBJ_ARBUSTO, q, rnd.rango(3.0f, 6.0f), rnd.rango(0.85f, 1.15f));
			m.objetos.back().brillaDeNoche = rnd.uniforme() < PROBABILIDAD_LUMINOSA_ORILLA;   // plantas luminosas que guían de noche
		}
		else if (azar < 0.82f)
		{
			float r = rnd.rango(1.0f, 2.4f);
			agregarRoca(m, rnd, OBJ_ROCA_MUSGO, q, glm::vec3(r, r * rnd.rango(0.45f, 0.75f), r * rnd.rango(0.8f, 1.2f)),
				0.30f, colorMusgo * rnd.rango(0.85f, 1.1f), 10.0f);
		}
	});

	// 3) Orilla exterior de la franja: una fila cerrada de árboles grandes que cierra la vista
	recorrerCamino(m, 7.5f, 0.0f, [&](const glm::vec2& c, const glm::vec2& tan, const glm::vec2& izq)
	{
		for (int lado = -1; lado <= 1; lado += 2)
		{
			glm::vec2 q = c + izq * ((float)lado * (m.anchoCamino + ANCHO_BOSQUE - rnd.rango(2.0f, 6.0f)));
			if (bloqueado(m, q, 0.0f) || distanciaAlCamino(m, q) < m.anchoCamino + ANCHO_BOSQUE - 8.0f || !troncos.libre(q, 2.5f)) continue;
			float tronco = agregarArbol(m, rnd, OBJ_ARBOL, q, rnd.rango(30.0f, 44.0f), rnd.rango(1.25f, 1.5f));
			troncos.agregar(q, tronco);
		}
	});

	// 4) La selva: solo en una franja de ANCHO_BOSQUE a cada lado del camino (zona finita)
	glm::vec2 minimo, maximo;
	limitesCamino(m, ANCHO_BOSQUE + m.anchoCamino, minimo, maximo);
	const float limiteFranja = m.anchoCamino + ANCHO_BOSQUE;
	for (float x = minimo.x; x < maximo.x; x += CELDA_BOSQUE)
	{
		for (float z = minimo.y; z < maximo.y; z += CELDA_BOSQUE)
		{
			glm::vec2 q(x + rnd.rango(0.0f, CELDA_BOSQUE), z + rnd.rango(0.0f, CELDA_BOSQUE));
			float azar = rnd.uniforme();
			float dist = distanciaAlCamino(m, q);
			if (azar < 0.88f && dist > m.anchoCamino + 10.0f && dist < limiteFranja && !bloqueado(m, q, 0.0f) && troncos.libre(q, 3.0f))
			{
				float tronco = agregarArbol(m, rnd, OBJ_ARBOL, q, rnd.rango(22.0f, 42.0f), rnd.rango(1.25f, 1.65f));
				troncos.agregar(q, tronco);
			}
			// árboles jóvenes, delgados y más bajos, entre los grandes
			glm::vec2 r(x + rnd.rango(0.0f, CELDA_BOSQUE), z + rnd.rango(0.0f, CELDA_BOSQUE));
			float distR = distanciaAlCamino(m, r);
			if (rnd.uniforme() < 0.30f && distR > m.anchoCamino + 5.0f && distR < limiteFranja && !bloqueado(m, r, 0.0f) && troncos.libre(r, 1.5f))
			{
				float tronco = agregarArbol(m, rnd, OBJ_ARBOL, r, rnd.rango(10.0f, 18.0f), rnd.rango(2.2f, 3.0f));
				troncos.agregar(r, tronco);
			}
			// sotobosque: arbustos entre los troncos
			glm::vec2 s(x + rnd.rango(0.0f, CELDA_BOSQUE), z + rnd.rango(0.0f, CELDA_BOSQUE));
			float distS = distanciaAlCamino(m, s);
			if (rnd.uniforme() < 0.25f && distS > m.anchoCamino + 1.0f && distS < limiteFranja && !bloqueado(m, s, -4.0f) && troncos.libre(s, 0.5f))
			{
				agregarArbol(m, rnd, OBJ_ARBUSTO, s, rnd.rango(3.0f, 7.5f), rnd.rango(0.85f, 1.2f));
				m.objetos.back().brillaDeNoche = rnd.uniforme() < PROBABILIDAD_LUMINOSA_BOSQUE;
			}
		}
	}

	// 5) Rocas con musgo sueltas dentro de la franja
	for (float x = minimo.x; x < maximo.x; x += 26.0f)
		for (float z = minimo.y; z < maximo.y; z += 26.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 26.0f), z + rnd.rango(0.0f, 26.0f));
			float dist = distanciaAlCamino(m, q);
			if (rnd.uniforme() > 0.40f || dist < m.anchoCamino + 1.5f || dist > limiteFranja || bloqueado(m, q, 0.0f) || !troncos.libre(q, 2.0f)) continue;
			float r = rnd.rango(1.5f, 4.0f);
			agregarRoca(m, rnd, OBJ_ROCA_MUSGO, q, glm::vec3(r, r * rnd.rango(0.45f, 0.8f), r * rnd.rango(0.8f, 1.2f)),
				0.30f, colorMusgo * rnd.rango(0.85f, 1.1f), 12.0f);
		}

	// 6) Orilla del claro de aterrizaje y del de la serpiente: arbustos que marcan el borde
	//    (el claro del pulpo se deja limpio para que se vea desde el camino)
	for (size_t k = 0; k < m.claros.size() && k < 2; k++)
		for (int i = 0; i < 24; i++)
		{
			const Claro& claro = m.claros[k];
			float a = rnd.rango(0.0f, 6.2831853f);
			float r = rnd.rango(claro.radio - 3.0f, claro.radio + 3.0f);
			glm::vec2 q = claro.centro + glm::vec2(cosf(a), sinf(a)) * r;
			if (distanciaAlCamino(m, q) < m.anchoCamino + 1.5f || !troncos.libre(q, 0.5f)) continue;
			agregarArbol(m, rnd, OBJ_ARBUSTO, q, rnd.rango(3.5f, 6.0f), rnd.rango(0.85f, 1.1f));
		}

	m.mosaicoPiso = 5.0f;
	m.texturaPiso = &texSueloSelva;
	m.texturaCamino = &texSueloRocoso;              // tierra apisonada en el camino
	m.colorPiso = glm::vec3(0.58f, 0.50f, 0.38f);   // hojarasca oscura y húmeda
	m.colorCamino = glm::vec3(0.86f, 0.77f, 0.63f); // tierra pisada, más clara
}

// ---------------------------------------------------------------------------------------------
// Planeta rocoso: desierto rojo con agujas de roca, piedras y cristales violetas en el camino
// ---------------------------------------------------------------------------------------------
static void generarDesierto(Mundo& m)
{
	Aleatorio rnd(19461u);
	// vuelta cerrada: sale recto hacia -Z desde el claro y regresa por la izquierda
	std::vector<glm::vec2> control = {
		{ 0.0f, 22.0f }, { 0.0f, 0.0f }, { 0.0f, -26.0f }, { -10.0f, -60.0f }, { -2.0f, -98.0f }, { -28.0f, -134.0f },
		{ -68.0f, -154.0f }, { -110.0f, -136.0f }, { -130.0f, -98.0f }, { -118.0f, -58.0f }, { -130.0f, -18.0f },
		{ -112.0f, 18.0f }, { -76.0f, 38.0f }, { -40.0f, 44.0f }, { -12.0f, 40.0f } };
	m.anchoCamino = 4.2f;
	prepararCamino(m, control);
	m.limiteJugador = LIMITE_JUGADOR_ROCOSO;
	m.posCapsula = glm::vec3(11.0f, 0.0f, -4.0f);
	m.posInicio = glm::vec3(0.0f, 0.0f, 14.0f);
	m.yawInicio = -90.0f;
	m.claros.push_back({ glm::vec2(0.0f), RADIO_CLARO });
	m.obstaculos.push_back({ glm::vec2(m.posCapsula.x, m.posCapsula.z), 2.6f });

	// --- NPCs: cerca uno del otro, alternando lado, sin pisar el camino ---
	//          modelo              animación       s     lado  altura radio  a     b     h     omega fase  
	colocarNPC(m, NPC_MONSTRUO_NEGRO, ANIM_LEVITA,    36.0f, 1.0f, 3.6f, 1.2f, 0.0f, 0.0f, 0.6f, 1.1f, 1.0f, 0.0f);
	colocarNPC(m, NPC_ESCORPION,      ANIM_OCHO,      70.0f,-1.0f, 1.5f, 3.9f, 2.0f, 2.0f, 0.05f,0.7f, 0.0f, 0.0f);
	colocarNPC(m, NPC_CERBERO,        ANIM_ELIPSE,   104.0f, 1.0f, 6.0f, 3.8f, 5.0f, 1.5f, 0.12f,0.4f, 0.0f, 0.0f);
	colocarNPC(m, NPC_SERPIENTE_ALAS, ANIM_LISSAJOUS,138.0f,-1.0f, 7.0f, 5.4f, 5.0f, 2.5f, 1.5f, 0.35f,7.0f, 0.0f);
	colocarPortal(m, 180.0f, 11.0f);   // un tramo después del último NPC
	// Paul se esconde detrás del portal: se deja libre la zona donde se asoma
	{
		glm::vec2 lado(-m.portal.adelante.y, m.portal.adelante.x);
		m.claros.push_back({ m.portal.centro + m.portal.adelante * 3.0f + lado * 4.0f, 8.0f });
		m.paulActivo = true;
	}

	RejillaEspacial ocupado(16.0f);
	ocupado.agregar(glm::vec2(m.posCapsula.x, m.posCapsula.z), 6.0f);
	const glm::vec3 colorRoca(1.10f, 0.62f, 0.46f);
	const float franjaAgujas = m.anchoCamino + 95.0f;   // zona finita de agujas alrededor del camino
	const float franjaPiedras = m.anchoCamino + 60.0f;

	auto colorRocaAzar = [&]() { float t = rnd.rango(0.80f, 1.10f); return glm::vec3(colorRoca.x * t, colorRoca.y * t * rnd.rango(0.92f, 1.05f), colorRoca.z * t); };

	// grupo de 1 a 3 agujas de roca alrededor de un punto
	auto grupoAgujas = [&](const glm::vec2& centro, float alturaMaxima)
	{
		int cantidad = 1 + (int)(rnd.uniforme() * 3.0f);
		for (int i = 0; i < cantidad; i++)
		{
			glm::vec2 q = centro + glm::vec2(rnd.rango(-5.0f, 5.0f), rnd.rango(-5.0f, 5.0f)) * (i == 0 ? 0.0f : 1.0f);
			float alto = alturaMaxima * (i == 0 ? 1.0f : rnd.rango(0.45f, 0.8f));
			float ancho = alto * rnd.rango(0.07f, 0.12f) + 0.9f;
			if (!ocupado.libre(q, ancho * 0.7f) || bloqueado(m, q, 0.0f) || distanciaAlCamino(m, q) < m.anchoCamino + 3.0f) continue;
			agregarRoca(m, rnd, OBJ_AGUJA, q, glm::vec3(ancho, alto * 0.5f, ancho * rnd.rango(0.8f, 1.2f)),
				0.10f, colorRocaAzar(), 6.0f);
			ocupado.agregar(q, ancho);
		}
	};

	// 1) Cristales a los lados del camino, alternando lado: marcan la ruta y brillan de noche
	int alterna = 0;
	recorrerCamino(m, 12.0f, 6.0f, [&](const glm::vec2& c, const glm::vec2& t, const glm::vec2& izq)
	{
		float lado = (alterna++ % 2 == 0) ? 1.0f : -1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(1.2f, 2.8f))) + t * rnd.rango(-2.0f, 2.0f);
		if (bloqueado(m, q, -10.0f) || distanciaAlCamino(m, q) < m.anchoCamino + 0.9f) return;
		agregarCristal(m, rnd, q, rnd.rango(2.0f, 3.6f));
		ocupado.agregar(q, 2.5f);
	});

	// 2) Agujas de roca cerca del camino, un poco más atrás que los cristales
	recorrerCamino(m, 28.0f, 10.0f, [&](const glm::vec2& c, const glm::vec2& t, const glm::vec2& izq)
	{
		float lado = (rnd.uniforme() < 0.5f) ? -1.0f : 1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(9.0f, 18.0f)));
		if (bloqueado(m, q, 12.0f) || distanciaAlCamino(m, q) < m.anchoCamino + 7.0f) return;
		grupoAgujas(q, rnd.rango(16.0f, 40.0f));
	});

	// 3) Piedras en la orilla del camino
	recorrerCamino(m, 5.0f, 2.0f, [&](const glm::vec2& c, const glm::vec2& t, const glm::vec2& izq)
	{
		if (rnd.uniforme() > 0.45f) return;
		float lado = (rnd.uniforme() < 0.5f) ? -1.0f : 1.0f;
		glm::vec2 q = c + izq * (lado * (m.anchoCamino + rnd.rango(0.8f, 3.5f)));
		if (bloqueado(m, q, -10.0f) || distanciaAlCamino(m, q) < m.anchoCamino + 0.6f || !ocupado.libre(q, 0.8f)) return;
		float r = rnd.rango(0.6f, 1.8f);
		agregarRoca(m, rnd, OBJ_ROCA, q, glm::vec3(r, r * rnd.rango(0.45f, 0.75f), r * rnd.rango(0.8f, 1.2f)),
			0.30f, colorRocaAzar(), 15.0f);
	});

	// 4) Relleno, solo en una franja alrededor del camino: agujas, piedras y cristales grandes
	glm::vec2 minimo, maximo;
	limitesCamino(m, franjaAgujas, minimo, maximo);
	for (float x = minimo.x; x < maximo.x; x += 24.0f)
		for (float z = minimo.y; z < maximo.y; z += 24.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 24.0f), z + rnd.rango(0.0f, 24.0f));
			float dist = distanciaAlCamino(m, q);
			if (rnd.uniforme() < 0.50f && dist > m.anchoCamino + 9.0f && dist < franjaAgujas && !bloqueado(m, q, 12.0f))
				grupoAgujas(q, rnd.rango(12.0f, 44.0f));
		}
	for (float x = minimo.x; x < maximo.x; x += 12.0f)
		for (float z = minimo.y; z < maximo.y; z += 12.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 12.0f), z + rnd.rango(0.0f, 12.0f));
			float dist = distanciaAlCamino(m, q);
			if (rnd.uniforme() > 0.40f || dist < m.anchoCamino + 1.0f || dist > franjaPiedras || bloqueado(m, q, -6.0f)) continue;
			if (!ocupado.libre(q, 1.0f)) continue;
			float r = rnd.rango(0.7f, 3.2f);
			agregarRoca(m, rnd, OBJ_ROCA, q, glm::vec3(r, r * rnd.rango(0.4f, 0.75f), r * rnd.rango(0.8f, 1.2f)),
				0.30f, colorRocaAzar(), 15.0f);
		}
	for (float x = minimo.x; x < maximo.x; x += 46.0f)
		for (float z = minimo.y; z < maximo.y; z += 46.0f)
		{
			glm::vec2 q(x + rnd.rango(0.0f, 46.0f), z + rnd.rango(0.0f, 46.0f));
			float dist = distanciaAlCamino(m, q);
			if (rnd.uniforme() > 0.35f || dist < m.anchoCamino + 6.0f || dist > franjaAgujas || bloqueado(m, q, 0.0f)) continue;
			if (!ocupado.libre(q, 3.0f)) continue;
			agregarCristal(m, rnd, q, rnd.rango(4.0f, 8.0f));
			ocupado.agregar(q, 4.0f);
		}

	// 5) Montañas lejanas: siluetas que se pierden en el polvo y cierran la zona
	glm::vec2 centro = (minimo + maximo) * 0.5f;
	for (int i = 0; i < 28; i++)
	{
		float a = (i + rnd.rango(-0.3f, 0.3f)) * (6.2831853f / 28.0f);
		float r = rnd.rango(360.0f, 470.0f);
		glm::vec2 q = centro + glm::vec2(cosf(a), sinf(a)) * r;
		glm::vec3 semiejes(rnd.rango(55.0f, 100.0f), rnd.rango(28.0f, 70.0f), rnd.rango(55.0f, 100.0f));
		agregarRoca(m, rnd, OBJ_MONTANA, q, semiejes, 0.35f, colorRoca * rnd.rango(0.65f, 0.85f), 4.0f);
	}

	m.mosaicoPiso = 9.0f;
	m.texturaPiso = &texSueloRocoso;
	m.texturaCamino = &texSueloRocoso;
	m.colorPiso = glm::vec3(1.05f, 0.56f, 0.40f);     // tierra rojiza
	m.colorCamino = glm::vec3(1.22f, 0.86f, 0.66f);   // arena más clara pisada
}

// =============================================================================================
// NPCs: posición y orientación en el tiempo (animación continua basada en funciones)
// =============================================================================================
static glm::mat4 matrizNPC(const NPC& n, float tiempo, glm::vec2& posicion)
{
	const Model& modelo = modelosNPC[n.modelo];
	float escala = n.altura / fmaxf(modelo.getTamano().y, 0.001f);
	float th = n.omega * tiempo + n.fase;
	glm::vec2 desplazamiento(0.0f);   // movimiento sobre el piso respecto a su centro
	glm::vec2 velocidad(0.0f);        // derivada de la posición: hacia dónde mira
	float y = n.alturaBase, alabeo = 0.0f, cabeceo = 0.0f, respiracion = 1.0f;
	float yaw = atan2f(-n.afuera.x, -n.afuera.y);   // los que no caminan miran hacia el camino

	switch (n.animacion)
	{
	case ANIM_SALTOS:
	{
		// cada periodo T = 2 pi / omega da un salto que dura D = 0.35 T: y = h sen(pi u / D)
		float periodo = 2.0f * PI_F / n.omega;
		float u = fmodf(tiempo + n.fase, periodo), duracion = 0.35f * periodo;
		if (u < duracion)
			y += n.h * sinf(PI_F * u / duracion);
		yaw += 0.35f * sinf(0.6f * tiempo + n.fase);           // voltea a los lados
		respiracion = 1.0f + 0.015f * sinf(2.0f * tiempo);
		break;
	}
	case ANIM_REBOTE:
		// círculo de radio a: p = a (cos th, sen th); rebote y = h |sen 3th|
		desplazamiento = n.adelante * (n.a * cosf(th)) + n.afuera * (n.a * sinf(th));
		velocidad = n.adelante * (-n.a * sinf(th)) + n.afuera * (n.a * cosf(th));
		y += n.h * fabsf(sinf(3.0f * th));
		break;
	case ANIM_OCHO:
		// lemniscata de Gerono: p = (a sen th, b sen th cos th); trote y = h |sen 3th|
		desplazamiento = n.adelante * (n.a * sinf(th)) + n.afuera * (n.b * sinf(th) * cosf(th));
		velocidad = n.adelante * (n.a * cosf(th)) + n.afuera * (n.b * cosf(2.0f * th));
		y += n.h * fabsf(sinf(3.0f * th));
		cabeceo = 0.04f * sinf(6.0f * th);
		break;
	case ANIM_RESPIRA:
		// respiración y balanceo: escalaY = 1 + 0.04 sen(1.6 t); giro = 0.3 sen(omega t)
		respiracion = 1.0f + 0.04f * sinf(1.6f * tiempo + n.fase);
		alabeo = 0.04f * sinf(0.8f * tiempo + n.fase);
		yaw += 0.3f * sinf(th);
		break;
	case ANIM_ELIPSE:
		// elipse: p = (a cos th, b sen th); paso y = h |sen 4th|
		desplazamiento = n.adelante * (n.a * cosf(th)) + n.afuera * (n.b * sinf(th));
		velocidad = n.adelante * (-n.a * sinf(th)) + n.afuera * (n.b * cosf(th));
		y += n.h * fabsf(sinf(4.0f * th));
		break;
	case ANIM_LEVITA:
		// flota: y = y0 + h sen(omega t); gira y se mece despacio
		y += n.h * sinf(th);
		yaw += 0.6f * sinf(0.45f * tiempo + n.fase);
		alabeo = 0.08f * sinf(1.3f * tiempo + n.fase);
		break;
	case ANIM_BAILE:
		// baile: se mece a lo largo del camino y gira la cadera de un lado a otro
		desplazamiento = n.adelante * (n.a * sinf(th));
		y += n.h * fabsf(sinf(2.0f * th));
		yaw += 0.5f * sinf(2.0f * th);
		alabeo = 0.08f * sinf(th);
		break;
	case ANIM_LISSAJOUS:
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
		break;
	}
	}
	if (n.animacion != ANIM_BAILE && glm::length(velocidad) > 1.0e-4f)
		yaw = atan2f(velocidad.x, velocidad.y);   // los modelos miran hacia +Z: se orientan con su velocidad

	posicion = n.centro + desplazamiento;
	return glm::translate(glm::mat4(1.0f), glm::vec3(posicion.x, y, posicion.y))
		* glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f))
		* glm::rotate(glm::mat4(1.0f), alabeo, glm::vec3(0.0f, 0.0f, 1.0f))
		* glm::rotate(glm::mat4(1.0f), cabeceo, glm::vec3(1.0f, 0.0f, 0.0f))
		* glm::scale(glm::mat4(1.0f), glm::vec3(escala, escala * respiracion, escala))
		* matrizBase(modelo);
}

// =============================================================================================
// API
// =============================================================================================
void InicializarSuperficies()
{
	if (inicializado)
		return;

	hayTexArbol = existeArchivoSuperficie(RUTAS_TEXTURA_ARBOL[0]) || existeArchivoSuperficie(RUTAS_TEXTURA_ARBOL[1]);
	const char* rutaTexArbol = existeArchivoSuperficie(RUTAS_TEXTURA_ARBOL[0]) ? RUTAS_TEXTURA_ARBOL[0] : RUTAS_TEXTURA_ARBOL[1];
	if (hayTexArbol)
		cargarTextura(texArbol, rutaTexArbol);
	else
		printf("[AVISO] No se encontro arbolTexture.png (en Textures/ ni en Models/arbol/): arboles sin textura\n");

	// el árbol se usa tal cual (sin versiones simplificadas); si su .mtl no encuentra la imagen se usa arbolTexture.png
	hayArbol = cargarModelo(arbol, RUTAS_ARBOL, 2, "arbol", nullptr, nullptr);
	if (hayArbol && !arbol.usaTexturaPropia() && hayTexArbol)
	{
		arbol.SetTexturaRespaldo(&texArbol);
		printf("     arbol: su .mtl no encontro la imagen, se usa %s\n", rutaTexArbol);
	}
	hayRoca = cargarModelo(roca, RUTAS_ROCA, 1, "roca", nullptr, nullptr);
	for (int i = 0; i < NUM_MODELOS_NPC; i++)
		hayNPC[i] = cargarModelo(modelosNPC[i], DEFINICION_NPC[i].rutas, DEFINICION_NPC[i].rutas[1] ? 2 : 1,
			DEFINICION_NPC[i].nombre, &texturasNPC[i], DEFINICION_NPC[i].textura);
	hayPortal = cargarModelo(modeloPortal, RUTAS_PORTAL, 2, "portal", &texturaPortal, RUTA_TEXTURA_PORTAL);
	// Paul.obj se exportó con la malla del portal (mismo número de triángulos y mismas medidas):
	// así se dibujaría un segundo portal, por eso no se usa hasta que se re-exporte el personaje
	paulValido = hayNPC[NPC_PAUL];
	if (paulValido && hayPortal && modelosNPC[NPC_PAUL].getNumTriangulos() == modeloPortal.getNumTriangulos()
		&& glm::length(modelosNPC[NPC_PAUL].getTamano() - modeloPortal.getTamano()) < 0.01f)
	{
		printf("[AVISO] Models/npcRoca/Paul.obj tiene la misma malla que el portal (en Blender se exporto el\n"
			"        objeto equivocado). Paul no se dibuja hasta que se re-exporte el personaje.\n");
		paulValido = false;
	}

	cargarTextura(texSueloSelva, RUTA_SUELO_SELVA);
	cargarTextura(texSueloRocoso, RUTA_SUELO_ROCOSO);
	cargarTextura(texPlain, RUTA_PLAIN);

	for (int i = 0; i < 3; i++)
		mallasCristal[i] = crearRacimoCristales(777u + 131u * (unsigned int)i, triangulosCristal[i]);

	generarSelva(mundos[SUPERFICIE_SELVATICA]);
	generarDesierto(mundos[SUPERFICIE_ROCOSA]);

	for (int i = 0; i < 2; i++)
	{
		Mundo& m = mundos[i];
		glm::vec2 minimo, maximo;
		limitesCamino(m, (i == SUPERFICIE_SELVATICA) ? MARGEN_PISO_SELVA : MARGEN_PISO_ROCOSO, minimo, maximo);
		m.mallaPiso = crearMallaPiso(minimo, maximo, m.mosaicoPiso);
		m.mallaCamino = crearMallaCamino(m, (i == SUPERFICIE_SELVATICA) ? 4.0f : 6.0f);
		int arboles = 0;
		for (const Objeto& o : m.objetos)
			if (o.tipo == OBJ_ARBOL || o.tipo == OBJ_ARBUSTO) arboles++;
		printf("[OK] Superficie %s: camino de %.0f unidades, %d objetos (%d arboles y arbustos), %d NPCs, %d obstaculos, piso de %.0f x %.0f\n",
			(i == SUPERFICIE_SELVATICA) ? "selvatica" : "rocosa", largoCamino(m), (int)m.objetos.size(), arboles,
			(int)m.npcs.size(), (int)m.obstaculos.size(), maximo.x - minimo.x, maximo.y - minimo.y);
	}
	inicializado = true;
}

void DibujarSuperficie(TipoSuperficie tipo, const glm::vec3& posCamara, const glm::vec3& dirCamara,
	float factorDia, float tiempo, const UniformsSuperficie& u)
{
	if (!inicializado)
		return;
	Mundo& m = mundos[tipo];
	static float tiempoAnterior = 0.0f;
	float dt = glm::clamp(tiempo - tiempoAnterior, 0.0f, 0.1f);
	tiempoAnterior = tiempo;

	// --- 1) Piso fijo (cubre toda la zona del planeta) ---
	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	glUniform2f(u.offsetTextura, 0.0f, 0.0f);
	matPiso.UseMaterial(u.especular, u.brillo);
	m.texturaPiso->UseTexture();
	glUniform3f(u.color, m.colorPiso.x, m.colorPiso.y, m.colorPiso.z);
	glm::mat4 identidad(1.0f);
	glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(identidad));
	m.mallaPiso->RenderMeshModel();

	// --- 2) Camino: un poco "adelante" del piso en profundidad para que no parpadee ---
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(-1.0f, -4.0f);
	m.texturaCamino->UseTexture();
	glUniform3f(u.color, m.colorCamino.x, m.colorCamino.y, m.colorCamino.z);
	m.mallaCamino->RenderMeshModel();
	glDisable(GL_POLYGON_OFFSET_FILL);

	// recorte: solo lo que está frente a la cámara (cono horizontal un poco más ancho que la vista)
	glm::vec2 camara(posCamara.x, posCamara.z);
	glm::vec2 direccion(dirCamara.x, dirCamara.z);
	float largoDireccion = glm::length(direccion);
	bool usarCono = largoDireccion > 0.25f;   // mirando casi vertical no se recorta por ángulo
	if (usarCono)
		direccion /= largoDireccion;
	float tanCono = tanf(glm::radians(ANGULO_CONO));
	float cosCono = cosf(glm::radians(ANGULO_CONO));
	auto visible = [&](const glm::vec2& posicion, float radio, float distanciaMaxima, float& dist)
	{
		glm::vec2 d = posicion - camara;
		dist = glm::length(d);
		if (dist - radio > distanciaMaxima)
			return false;
		if (usarCono && dist > radio)
		{
			float proyeccion = glm::dot(d, direccion);
			float lateral = sqrtf(fmaxf(dist * dist - proyeccion * proyeccion, 0.0f));
			if (lateral > proyeccion * tanCono + radio / cosCono)
				return false;
		}
		return true;
	};

	double triangulos = 0.0;
	int dibujados = 0;

	// --- 3) NPCs (son pocos e importantes: no cuentan para el presupuesto de la escenografía) ---
	double triangulosNPC = 0.0;
	matNPC.UseMaterial(u.especular, u.brillo);
	glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
	for (const NPC& n : m.npcs)
	{
		if (!hayNPC[n.modelo])
			continue;
		glm::vec2 posicion;
		float dist;
		glm::mat4 matriz = matrizNPC(n, tiempo, posicion);
		if (!visible(posicion, n.radio + fmaxf(n.a, n.b), DISTANCIA_NPC, dist))
			continue;
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(matriz));
		modelosNPC[n.modelo].RenderModel();
		triangulosNPC += modelosNPC[n.modelo].getNumTriangulos();
		dibujados++;
	}

	// --- Portal: brilla con un pulso (emisión = base * (0.6 + 0.4 sen(2.2 t))) ---
	float distPortal;
	if (hayPortal && m.portal.activo && visible(m.portal.centro, 0.5f * m.portal.ancho, DISTANCIA_NPC, distPortal))
	{
		float escala = m.portal.altura / fmaxf(modeloPortal.getTamano().y, 0.001f);
		glm::mat4 matrizPortal = glm::translate(glm::mat4(1.0f), glm::vec3(m.portal.centro.x, 0.0f, m.portal.centro.y))
			// el frente del modelo (+Z, donde está la galaxia) mira hacia quien llega por el camino
			* glm::rotate(glm::mat4(1.0f), atan2f(-m.portal.adelante.x, -m.portal.adelante.y), glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(escala))
			* matrizBase(modeloPortal);
		float pulso = 0.6f + 0.4f * sinf(2.2f * tiempo);
		glUniform3f(u.emision, 0.45f * pulso, 0.50f * pulso, 0.75f * pulso);   // la galaxia del portal brilla
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(matrizPortal));
		modeloPortal.RenderModel();
		glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
		triangulosNPC += modeloPortal.getNumTriangulos();
		dibujados++;
	}

	// --- Paul: escondido detrás del portal; se asoma cuando miras hacia el portal ---
	if (paulValido && m.paulActivo && m.portal.activo)
	{
		glm::vec2 haciaPortal = m.portal.centro - camara;
		float distanciaPortal = glm::length(haciaPortal);
		bool mirando = usarCono && distanciaPortal > 1.0f && distanciaPortal < 110.0f
			&& glm::dot(haciaPortal / distanciaPortal, direccion) > cosf(glm::radians(20.0f));
		// mientras miras el portal se asoma y se esconde (lo decide sen(1.4 t)); si no lo miras, se esconde
		float objetivo = (mirando && sinf(1.4f * tiempo) > -0.2f) ? 1.0f : 0.0f;
		revelacionPaul += (objetivo - revelacionPaul) * fminf(1.0f, 4.0f * dt);
		float r = suave(revelacionPaul);

		glm::vec2 lado(-m.portal.adelante.y, m.portal.adelante.x);
		glm::vec2 pos = m.portal.centro + m.portal.adelante * 2.5f + lado * (r * (0.5f * m.portal.ancho + 1.5f));
		float yaw = atan2f(-m.portal.adelante.x, -m.portal.adelante.y);   // mira hacia quien llega
		glm::vec2 ejeX(cosf(yaw), -sinf(yaw));                               // su derecha en el mundo
		float inclinacion = -0.22f * r * (glm::dot(lado, ejeX) > 0.0f ? 1.0f : -1.0f);   // se ladea al asomarse
		const Model& paul = modelosNPC[NPC_PAUL];
		float escala = 4.4f / fmaxf(paul.getTamano().y, 0.001f);
		glm::mat4 matrizPaul = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, 0.12f * r * fabsf(sinf(3.0f * tiempo)), pos.y))
			* glm::rotate(glm::mat4(1.0f), yaw, glm::vec3(0.0f, 1.0f, 0.0f))
			* glm::rotate(glm::mat4(1.0f), inclinacion, glm::vec3(0.0f, 0.0f, 1.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(escala))
			* matrizBase(paul);
		matNPC.UseMaterial(u.especular, u.brillo);
		glUniform3f(u.color, 1.0f, 1.0f, 1.0f);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(matrizPaul));
		modelosNPC[NPC_PAUL].RenderModel();
		triangulosNPC += paul.getNumTriangulos();
		dibujados++;
	}

	// --- 4) Objetos: del más cercano al más lejano, sin pasar del presupuesto de triángulos ---
	static std::vector<std::pair<float, int>> visibles;   // (distancia, índice); static para no reservar cada cuadro
	visibles.clear();
	for (size_t i = 0; i < m.objetos.size(); i++)
	{
		const Objeto& o = m.objetos[i];
		float dist;
		if (o.tipo != OBJ_MONTANA && visible(o.posicion, o.radio, DISTANCIA_MAXIMA[o.tipo], dist))
			visibles.push_back(std::make_pair(dist, (int)i));
	}
	std::sort(visibles.begin(), visibles.end());

	// los cristales y las plantas luminosas brillan un poco de día y mucho de noche
	float brilloNocturno = 0.06f + 0.75f * (1.0f - factorDia);
	int tipoActual = -1;
	bool emisionActiva = false;

	auto dibujarObjeto = [&](const Objeto& o)
	{
		if (o.tipo != tipoActual)
		{
			if (o.tipo == OBJ_CRISTAL) matCristal.UseMaterial(u.especular, u.brillo);
			else if (o.tipo == OBJ_ARBOL || o.tipo == OBJ_ARBUSTO) matArbol.UseMaterial(u.especular, u.brillo);
			else matRoca.UseMaterial(u.especular, u.brillo);
			tipoActual = o.tipo;
		}
		if (o.tipo == OBJ_CRISTAL)
		{
			glUniform3f(u.emision, o.color.x * 0.55f * brilloNocturno, o.color.y * 0.45f * brilloNocturno, o.color.z * 0.65f * brilloNocturno);
			emisionActiva = true;
		}
		else if (o.brillaDeNoche)
		{
			// planta luminosa: brillo suave verde azulado (se multiplica por el color de su textura,
			// así se siguen viendo las hojas en lugar de una mancha verde)
			glUniform3f(u.emision, 0.10f * brilloNocturno, 0.55f * brilloNocturno, 0.45f * brilloNocturno);
			emisionActiva = true;
		}
		else if (emisionActiva)
		{
			glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
			emisionActiva = false;
		}
		glUniform3f(u.color, o.color.x, o.color.y, o.color.z);
		glUniformMatrix4fv(u.model, 1, GL_FALSE, glm::value_ptr(o.matriz));

		switch (o.tipo)
		{
		case OBJ_ARBOL:
		case OBJ_ARBUSTO:
			if (!hayArbol) break;
			arbol.RenderModel();
			triangulos += arbol.getNumTriangulos();
			break;
		case OBJ_ROCA_MUSGO:
			roca.RenderModelConTextura(hayTexArbol ? texArbol : texPlain);   // la textura de hojas parece musgo
			triangulos += roca.getNumTriangulos();
			break;
		case OBJ_AGUJA:
		case OBJ_ROCA:
		case OBJ_MONTANA:
			roca.RenderModelConTextura(texSueloRocoso);
			triangulos += roca.getNumTriangulos();
			break;
		case OBJ_CRISTAL:
			texPlain.UseTexture();
			mallasCristal[o.variante]->RenderMeshModel();
			triangulos += triangulosCristal[o.variante];
			break;
		default:
			break;
		}
		dibujados++;
	};

	for (size_t i = 0; i < visibles.size(); i++)
	{
		if (triangulos > PRESUPUESTO_TRIANGULOS)
			break;   // lo que falta está más lejos que todo lo ya dibujado
		dibujarObjeto(m.objetos[visibles[i].second]);
	}

	// montañas: siempre (son pocas y están tan lejos que la niebla decide cuánto se ven)
	for (size_t i = 0; i < m.objetos.size(); i++)
		if (m.objetos[i].tipo == OBJ_MONTANA)
			dibujarObjeto(m.objetos[i]);

	glUniform3f(u.emision, 0.0f, 0.0f, 0.0f);
	objetosUltimoCuadro = dibujados;
	triangulosUltimoCuadro = triangulos + triangulosNPC;
}

glm::vec3 LimitarPosicion(TipoSuperficie tipo, const glm::vec3& posicion)
{
	const Mundo& m = mundos[tipo];
	if (!inicializado || m.camino.empty())
		return posicion;
	glm::vec2 p(posicion.x, posicion.z);

	// 1) obstáculos: si el jugador quedó dentro de un tronco, roca o cristal, se le saca por la orilla
	//    (dos pasadas por si quedó entre dos obstáculos)
	for (int pasada = 0; pasada < 2; pasada++)
		for (const Obstaculo& o : m.obstaculos)
		{
			glm::vec2 d = p - o.centro;
			float minimo = o.radio + RADIO_JUGADOR;
			if (fabsf(d.x) > minimo || fabsf(d.y) > minimo)
				continue;   // descarte rápido
			float dist = glm::length(d);
			if (dist < minimo)
				p = o.centro + (dist > 1.0e-4f ? d / dist : glm::vec2(1.0f, 0.0f)) * minimo;
		}

	// 2) zona: no puede alejarse del camino más de limiteJugador
	glm::vec2 cercano = puntoMasCercano(m, p);
	glm::vec2 d = p - cercano;
	float dist = glm::length(d);
	if (dist > m.limiteJugador)
		p = cercano + d * (m.limiteJugador / dist);   // se queda en la orilla de la zona
	return glm::vec3(p.x, posicion.y, p.y);
}

// Carrera dentro del planeta: del punto de inicio hasta un poco después del portal
static float inicioCarrera(const Mundo& m)
{
	(void)m;
	return 8.0f;   // posInicio está a 8 unidades del principio del camino
}

float LargoCarreraSuperficie(TipoSuperficie tipo)
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
	return sPortal + 5.0f - inicioCarrera(m);
}

void PuntoCarreraSuperficie(TipoSuperficie tipo, float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante)
{
	const Mundo& m = mundos[tipo];
	glm::vec2 p, t;
	puntoEnCamino(m, inicioCarrera(m) + distancia, p, t);
	glm::vec2 lado(-t.y, t.x);
	glm::vec2 q = p + lado * carril;
	posicion = glm::vec3(q.x, altura, q.y);
	adelante = glm::vec3(t.x, 0.0f, t.y);
}

glm::vec3 AjustarACamino(TipoSuperficie tipo, const glm::vec3& posicion, float altura, glm::vec3* adelante)
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
	puntoEnCamino(m, sMejor, p, t);
	glm::vec2 lado(-t.y, t.x);
	float lateral = glm::clamp(glm::dot(q - p, lado), -(m.anchoCamino - 1.5f), m.anchoCamino - 1.5f);
	glm::vec2 r = p + lado * lateral;
	if (adelante) *adelante = glm::vec3(t.x, 0.0f, t.y);
	return glm::vec3(r.x, altura, r.y);
}

Model* ModeloPaul()
{
	return (inicializado && paulValido) ? &modelosNPC[NPC_PAUL] : nullptr;
}

bool CruzandoPortal(TipoSuperficie tipo, const glm::vec3& posicion)
{
	const Portal& portal = mundos[tipo].portal;
	if (!inicializado || !portal.activo)
		return false;
	// posición del jugador medida en los ejes del portal: a lo largo del camino y hacia los lados
	if (posicion.y > portal.altura)
		return false;   // con la cámara libre, pasar volando por encima no cuenta
	glm::vec2 d = glm::vec2(posicion.x, posicion.z) - portal.centro;
	glm::vec2 lado(-portal.adelante.y, portal.adelante.x);
	float alFrente = glm::dot(d, portal.adelante);
	float alLado = glm::dot(d, lado);
	return fabsf(alFrente) < 1.4f && fabsf(alLado) < 0.30f * portal.ancho;   // dentro del hueco del marco
}

void EstadisticasSuperficie(int& objetosDibujados, int& triangulosDibujados)
{
	objetosDibujados = objetosUltimoCuadro;
	triangulosDibujados = (int)triangulosUltimoCuadro;
}

AmbienteSuperficie ObtenerAmbiente(TipoSuperficie tipo, float factorDia)
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

glm::vec3 PosicionCapsula(TipoSuperficie tipo)
{
	return mundos[tipo].posCapsula;
}

glm::vec3 PosicionInicio(TipoSuperficie tipo)
{
	return mundos[tipo].posInicio;
}

float YawInicio(TipoSuperficie tipo)
{
	return mundos[tipo].yawInicio;
}
