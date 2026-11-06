#pragma once
/*
NUEVO (no viene en las prácticas): superficie de los planetas rocoso y selvático.
Cada planeta es una zona FINITA alrededor de un camino cerrado:
 - Selvático (Shinobi): avenida de árboles a ambos lados del camino, una franja de selva alrededor y
   NPCs (Naruto, zorro blanco, caballo-delfín, calamardo, pulpo y tigre de fuego).
 - Rocoso (Vacío): desierto rojo con cristales violetas en el camino, agujas de roca, montañas al
   fondo y NPCs (monstruo negro, escorpión, Cerbero y serpiente alada).
Poco después del último NPC hay un portal: al cruzarlo en la nave se regresa al espacio.

Cómo se arma (generación procedural):
 - El camino es una curva de Catmull-Rom cerrada que pasa por unos puntos de control.
 - Los árboles, rocas y cristales se colocan recorriendo el camino y una rejilla alrededor de él, con
   números pseudoaleatorios de SEMILLA FIJA (Aleatorio): siempre sale el mismo mapa (sirve para el
   croquis y el manual). A cada objeto se le calcula UNA sola vez su matriz de modelo.
 - En cada cuadro solo se dibujan los objetos cercanos que están frente a la cámara.
 - Los NPCs tienen una animación continua basada en funciones (seno, elipse, ocho, Lissajous...).
*/

#include <vector>
#include <utility>
#include <glew.h>
#include <glm.hpp>

#include "Model.h"
#include "Texture.h"
#include "Mesh_tn.h"
#include "Material.h"

enum TipoSuperficie
{
	SUPERFICIE_ROCOSA = 0,
	SUPERFICIE_SELVATICA = 1
};

// Niebla (shader_light) y cielo (skybox) de cada planeta según la hora del día
struct AmbienteSuperficie
{
	glm::vec3 colorNiebla;
	float densidadNiebla;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;   // rgb = color, a = intensidad
};

// Modelos de los NPCs (carpetas Models/npcSelva y Models/npcRoca)
enum ModeloNPC
{
	NPC_NARUTO = 0,			// selva
	NPC_ZORRO_BLANCO,
	NPC_CABALLO_DELFIN,
	NPC_PULPO,
	NPC_TIGRE_FUEGO,
	NPC_CALAMARDO_GUAPO,
	NPC_MONSTRUO_NEGRO,		// roca
	NPC_ESCORPION,
	NPC_CERBERO,
	NPC_SERPIENTE_ALAS,
	NPC_PAUL,				// se esconde detrás del portal del planeta rocoso
	NPC_VOIDMITE, NPC_VOIDGRUB, NPC_VOID_SPAWN, NPC_RIFT_HERALD,
	NUM_MODELOS_NPC
};

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

// Cada NPC se mueve poco alrededor de su lugar, con una ecuación (función del tiempo t).
// th = omega * t + fase. "adelante" es la dirección del camino y "afuera" la que se aleja de él.
enum AnimacionNPC
{
	ANIM_SALTOS = 0,	// salto de medio seno cada periodo:        y = h * sen(pi * u / D)  si u < D
	ANIM_REBOTE,		// círculo pequeño rebotando:                p = a (cos th, sen th),  y = h |sen 3th|
	ANIM_OCHO,			// trote en un ocho (lemniscata de Gerono):  p = (a sen th, b sen th cos th)
	ANIM_RESPIRA,		// respira y se balancea en su lugar:        escalaY = 1 + 0.04 sen(1.6 t)
	ANIM_ELIPSE,		// ronda en una elipse pequeña:              p = (a cos th, b sen th)
	ANIM_LEVITA,		// flota y gira:                             y = y0 + h sen(omega t)
	ANIM_LISSAJOUS,		// vuela en un ocho de Lissajous:            p = (a sen th, b sen 2th)
	ANIM_BAILE			// se mece y gira de lado a lado:            x = a sen th, y = h |sen 2th|, giro = 0.5 sen 2th
};

struct Objeto
{
	TipoObjeto tipo;
	glm::vec2 posicion;		// posición en el piso (x, z)
	float radio;			// radio aproximado, para recortar por distancia y ángulo
	glm::mat4 matriz;		// matriz de modelo completa (se calcula una sola vez)
	glm::vec3 color;
	int variante;			// cristales: qué racimo se usa
	bool brillaDeNoche;		// plantas luminosas de la selva
};

struct NPC
{
	ModeloNPC modelo;
	AnimacionNPC animacion;
	float altura;			// altura del modelo en el mundo
	glm::vec2 centro;		// punto fijo alrededor del cual se mueve (a un lado del camino)
	glm::vec2 adelante;		// dirección del camino en ese punto
	glm::vec2 afuera;		// dirección que se aleja del camino
	float a, b, h;			// amplitudes: a lo largo del camino, hacia afuera y vertical
	float omega;			// velocidad angular (radianes por segundo)
	float alturaBase;		// altura sobre el piso (los que vuelan o flotan)
	float fase;
	float radio;			// radio aproximado de su huella (para claros, recorte y obstáculos)
};

// Portal de salida: está sobre el camino, poco después del último NPC
struct Portal
{
	bool activo = false;
	glm::vec2 centro = glm::vec2(0.0f);
	glm::vec2 adelante = glm::vec2(0.0f, 1.0f);   // el portal se cruza en esta dirección
	float altura = 11.0f;
	float ancho = 9.7f;                           // ancho total del marco
};

// Zona sin árboles (aterrizaje, alrededor de cada NPC y del portal)
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

// Un punto del camino con su dirección y su izquierda (para colocar objetos a los lados)
struct PuntoCamino
{
	glm::vec2 punto;
	glm::vec2 tangente;
	glm::vec2 izquierda;
};

struct Mundo
{
	std::vector<glm::vec2> camino;		// centro del camino, un punto cada 2 unidades (curva cerrada)
	std::vector<float> acumulado;		// distancia recorrida hasta cada punto del camino
	float largo = 0.0f;
	float anchoCamino = 3.0f;			// medio ancho
	float limiteJugador = 40.0f;
	std::vector<Claro> claros;			// zonas sin árboles (claros[0] es el de aterrizaje)
	std::vector<Obstaculo> obstaculos;	// lo que el jugador no puede atravesar
	MeshModel* mallaCamino = nullptr;
	MeshModel* mallaPiso = nullptr;
	float mosaicoPiso = 8.0f;			// unidades de mundo que ocupa una repetición de la textura
	Texture* texturaPiso = nullptr;
	Texture* texturaCamino = nullptr;
	glm::vec3 colorPiso = glm::vec3(1.0f);
	glm::vec3 colorCamino = glm::vec3(1.0f);
	std::vector<Objeto> objetos;
	std::vector<NPC> npcs;
	Portal portal;
	bool paulActivo = false;			// Paul se asoma detrás del portal de este planeta
	glm::vec3 posCapsula = glm::vec3(0.0f);
	glm::vec3 posInicio = glm::vec3(0.0f, 0.0f, 20.0f);
	float yawInicio = -90.0f;
};

// NUEVO: generador de números pseudoaleatorios (xorshift). Con la misma semilla da los mismos
// números en cualquier compilador (rand() no), así el mapa siempre sale igual.
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
	float uniforme() { return (float)(siguiente() >> 8) * (1.0f / 16777216.0f); }	// [0, 1)
	float rango(float a, float b) { return a + (b - a) * uniforme(); }
};

class Superficie
{
public:
	Superficie();
	// Carga modelos y texturas y genera los dos mundos (con OpenGL ya iniciado)
	void InicializarSuperficies();
	// Carga solo los NPC que pertenecen a la superficie solicitada. Se llama al entrar por primera vez.
	void PrepararModelosSuperficie(TipoSuperficie tipo);

	// Dibuja piso, camino, objetos y NPCs del planeta. El shader_light debe estar activo y con luces.
	// 'tiempo' (segundos) mueve las animaciones de los NPCs.
	void DibujarSuperficie(TipoSuperficie tipo, glm::vec3 posCamara, glm::vec3 dirCamara, float factorDia, float tiempo,
		GLuint modelLocation, GLuint colorLocation, GLuint offsetLocation,
		GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation);

	// Posición corregida para que el jugador no salga de la zona del planeta (no se aleja del camino
	// más de cierta distancia) ni atraviese troncos, rocas, etc.
	glm::vec3 LimitarPosicion(TipoSuperficie tipo, glm::vec3 posicion);

	// Carrera dentro del planeta (la nave va sobre el camino, del inicio hasta pasar el portal)
	float LargoCarreraSuperficie(TipoSuperficie tipo);
	void PuntoCarreraSuperficie(TipoSuperficie tipo, float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante);
	glm::vec3 AjustarACamino(TipoSuperficie tipo, glm::vec3 posicion, float altura, glm::vec3& adelante);

	Model* ModeloPaul();	// modelo de Paul (también es el avatar a pie); nullptr si no se cargó
	bool CruzandoPortal(TipoSuperficie tipo, glm::vec3 posicion);	// true al pasar por el hueco del portal
	AmbienteSuperficie ObtenerAmbiente(TipoSuperficie tipo, float factorDia);
	glm::vec3 PosicionCapsula(TipoSuperficie tipo);	// dónde va la cápsula de regreso
	glm::vec3 PosicionInicio(TipoSuperficie tipo);	// dónde aparece el jugador al aterrizar
	float YawInicio(TipoSuperficie tipo);			// hacia dónde mira al aterrizar (grados)

	~Superficie();

private:
	// --- utilidades ---
	glm::vec3 PiesDelModelo(const Model& modelo);	// traslación que deja los pies del modelo en y = 0
	float Suave(float x);
	bool Libre(const std::vector<glm::vec3>& ocupados, glm::vec2 p, float radio);
	// --- camino ---
	glm::vec2 CatmullRom(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t);
	std::vector<glm::vec2> MuestrearCamino(const std::vector<glm::vec2>& control, float paso);
	void PrepararCamino(Mundo& m, const std::vector<glm::vec2>& control);
	glm::vec2 PuntoMasCercano(const Mundo& m, glm::vec2 p);
	float DistanciaAlCamino(const Mundo& m, glm::vec2 p);
	void PuntoEnCamino(const Mundo& m, float s, glm::vec2& posicion, glm::vec2& tangente);
	float AnchoEn(const Mundo& m, float s, float total);
	MeshModel* CrearMallaCamino(const Mundo& m, float mosaico);
	MeshModel* CrearMallaPiso(glm::vec2 minimo, glm::vec2 maximo, float mosaico);
	void PuntosDelCamino(const Mundo& m, float paso, float desfase, std::vector<PuntoCamino>& puntos);
	void LimitesCamino(const Mundo& m, float margen, glm::vec2& minimo, glm::vec2& maximo);
	bool Bloqueado(const Mundo& m, glm::vec2 q, float margenAterrizaje);
	// --- cristales hechos con código ---
	MeshModel* CrearRacimoCristales(unsigned int semilla, int& triangulos);
	void AgregarTrianguloCristal(std::vector<GLfloat>& v, std::vector<unsigned int>& indices, glm::vec3 base, glm::vec3 eje,
		glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, float vBase);
	// --- colocación de objetos ---
	void AgregarObjeto(Mundo& m, TipoObjeto tipo, glm::vec2 p, float radio, glm::mat4 matriz, glm::vec3 color, int variante);
	float AgregarArbol(Mundo& m, Aleatorio& rnd, TipoObjeto tipo, glm::vec2 p, float altura, float esbeltez);
	void AgregarRoca(Mundo& m, Aleatorio& rnd, TipoObjeto tipo, glm::vec2 p, glm::vec3 semiejes,
		float hundimiento, glm::vec3 color, float inclinacionMaxima);
	void AgregarCristal(Mundo& m, Aleatorio& rnd, glm::vec2 p, float escala);
	void ColocarNPC(Mundo& m, ModeloNPC modelo, AnimacionNPC animacion, float s, float lado,
		float altura, float radio, float a, float b, float h, float omega, float alturaBase, float fase);
	void ColocarPortal(Mundo& m, float s, float altura);
	glm::vec3 ColorRocaAzar(Aleatorio& rnd);
	void GrupoAgujas(Mundo& m, Aleatorio& rnd, std::vector<glm::vec3>& ocupado, glm::vec2 centro, float alturaMaxima);
	void GenerarSelva(Mundo& m);
	void GenerarDesierto(Mundo& m);
	// --- dibujo ---
	glm::mat4 MatrizNPC(const NPC& n, float tiempo, glm::vec2& posicion);
	bool Visible(glm::vec2 posicion, float radio, float distanciaMaxima, float& dist);
	void DibujarObjeto(const Objeto& o);
	float InicioCarrera(const Mundo& m);

	Mundo mundos[2];
	bool inicializado;

	Model Arbol_M;
	Model Roca_M;
	bool hayArbol, hayRoca;
	Model modelosNPC[NUM_MODELOS_NPC];
	bool hayNPC[NUM_MODELOS_NPC];
	bool modelosSuperficieListos[2];
	Model Portal_M;
	bool hayPortal;
	bool paulValido;			// false si Paul.obj no se cargó o trae la malla equivocada
	float revelacionPaul;		// 0 = escondido detrás del portal, 1 = asomado
	Texture sueloSelvaTexture;
	Texture sueloRocosoTexture;
	Texture arbolTexture;
	Texture plainTexture;
	bool hayTexArbol;
	MeshModel* mallasCristal[3];
	int triangulosCristal[3];

	Material Material_piso;
	Material Material_arbol;
	Material Material_roca;
	Material Material_cristal;
	Material Material_npc;

	// estado del cuadro que se está dibujando (lo usan Visible y DibujarObjeto)
	GLuint uniformModel, uniformColor, uniformTextureOffset, uniformSpecularIntensity, uniformShininess, uniformEmision;
	glm::vec2 camaraPiso;		// posición de la cámara en el piso (x, z)
	glm::vec2 direccionPiso;	// hacia dónde mira la cámara en el piso
	bool usarCono;
	float tanCono, cosCono;
	int tipoActual;
	bool emisionActiva;
	float brilloNocturno;
	double triangulos;
	float tiempoAnterior;
	std::vector<std::pair<float, int>> visibles;	// (distancia, índice) de los objetos que se ven
};

