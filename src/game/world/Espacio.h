#pragma once
/*
NUEVO (no viene en las prácticas): espacio entre los planetas
 - Pista de carreras (Models/pista/pista.obj) en tres tramos que ENTRAN a los planetas (la carrera
   sigue dentro de cada planeta y sale por su portal o por la cresta de la ola):
     tramo 0: sale del océano -> sube a la meta -> entra al planeta rocoso por abajo
     tramo 1: sale del rocoso -> sube y cruza el cinturón -> entra al selvático por arriba
     tramo 2: sale del selvático por abajo -> baja -> entra al planeta océano
   Cada tramo es una curva de Catmull-Rom: una curva suave que pasa exactamente por los puntos de
   control. Se guarda un punto de la curva cada 1 unidad para recorrerla y para pegar la nave al camino.
 - Luces en las orillas de la pista que "corren" (animación continua con una función seno).
 - Línea de meta y arcos de luz donde la pista entra a la atmósfera de cada planeta.
 - Cinturón de asteroides entre los planetas, cada uno girando sobre su propio eje.
 - Base de inicio: plataforma circular bajo la pista, antes de la meta (ahí aparece el jugador).
*/

#include <vector>
#include <glew.h>
#include <glm.hpp>

#include "Model.h"
#include "Texture.h"
#include "Mesh_tn.h"
#include "Material.h"
#include "Oceano.h"

// Planetas a los que entra la pista
enum PlanetaPista
{
	PLANETA_ROCOSO = 0,
	PLANETA_SELVATICO,
	PLANETA_OCEANO
};

const int NUM_TRAMOS = 3;

// Un tramo de la pista: un punto cada 1 unidad y la distancia recorrida hasta cada punto
struct Tramo
{
	std::vector<glm::vec3> puntos;
	std::vector<float> acumulado;
	float largo = 0.0f;
};

struct Asteroide
{
	glm::vec3 base, estiramiento, eje;
	float escala, giro, fase;
	bool grande;
};

// NUEVO: generador de números pseudoaleatorios (xorshift). Con la misma semilla siempre da los mismos
// números, así el cinturón de asteroides siempre sale igual (rand() puede cambiar entre compiladores).
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

class Espacio
{
public:
	Espacio();
	// Carga la pista y los asteroides (con OpenGL ya iniciado). Necesita al océano para sus datos.
	void InicializarEspacio(Oceano* oceanoPlaneta);

	// Pista (pista.obj), sus luces, la línea de meta y los arcos de entrada a cada atmósfera
	void DibujarPista(float tiempo, GLuint modelLocation, GLuint colorLocation,
		GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation);
	// Cinturón de asteroides (solo los que están frente a la cámara)
	void DibujarCinturon(glm::vec3 posCamara, glm::vec3 dirCamara, float tiempo, GLuint modelLocation, GLuint colorLocation,
		GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation);
	// Base de inicio: plataforma bajo la pista, antes de la meta
	void DibujarBase(float tiempo, GLuint modelLocation, GLuint colorLocation,
		GLuint specularIntensityLocation, GLuint shininessLocation, GLuint emisionLocation);
	// centro del piso de la base, dirección de la carrera, su derecha y el radio de la plataforma
	void DatosBase(glm::vec3& centro, glm::vec3& adelante, glm::vec3& derecha, float& radio);

	// Recorrido de la pista (lo usa la nave rival): tramos 0, 1 y 2; la meta está en el tramo 0
	float LargoTramo(int tramo);
	float DistanciaMeta();
	void PuntoEnPista(int tramo, float s, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba);

	// Regresa la posición corregida para quedar sobre el camino, a 'altura' sobre el piso y sin salirse
	// de sus orillas; 'adelante' y 'arriba' regresan la orientación del camino en ese punto
	glm::vec3 AjustarAPista(glm::vec3 posicion, float altura, glm::vec3& adelante, glm::vec3& arriba);

	// Lugar de salida en la meta para la nave del jugador ('carril' > 0 = a la derecha)
	void SalidaEnMeta(glm::vec3& posicion, glm::vec3& adelante, glm::vec3& arriba, float altura, float carril);
	// Punto de aparición al salir de un planeta (yaw y pitch en grados, como los usa la clase Camera)
	void SalidaDePlaneta(int planeta, glm::vec3& posicion, float& yaw, float& pitch, float alturaOjos);

	~Espacio();

private:
	glm::vec3 CatmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);
	void ConstruirTramo(Tramo& tramo, const glm::vec3* control, int n, int indiceMarca, float* sMarca);
	glm::vec3 PuntoTramo(const Tramo& tramo, float s, glm::vec3* tangente);
	void MarcoTramo(const Tramo& tramo, float s, glm::vec3& p, glm::vec3& adelante, glm::vec3& arriba, glm::vec3& derecha);
	void MallaTramo(const Tramo& tramo, std::vector<GLfloat>& v, std::vector<unsigned int>& indices);
	void AgregarVertice(std::vector<GLfloat>& v, glm::vec3 p, float s, float t, glm::vec3 n);
	glm::vec3 PuntoBase(float angulo, float radio, float y);
	void ConstruirBase();
	float DistanciaAPista(glm::vec3 q);
	void GenerarCinturon();
	void DibujarMarca(glm::vec3 centro, glm::vec3 ejeX, glm::vec3 ejeY, glm::vec3 normal, float ancho, float largo, glm::vec3 color);
	float DistanciaAPlaneta(const Tramo& tramo, glm::vec3 centro, float distancia, bool desdeElFinal);
	void PuntoDeAparicion(const Tramo& tramo, float s, float alturaOjos, glm::vec3& posicion, float& yaw, float& pitch);

	Tramo tramos[NUM_TRAMOS];	// 0: océano -> meta -> rocoso, 1: rocoso -> selvático, 2: selvático -> océano
	float sMeta;				// dónde está la meta dentro del tramo 0
	std::vector<Asteroide> asteroides;
	glm::vec3 direccionCinturon;

	Model Pista_M;
	bool hayModeloPista;
	MeshModel* mallaPistaCodigo[NUM_TRAMOS];	// respaldo si falta pista.obj
	Texture pistaTexture;
	Texture asteroideTexture;
	Texture plainTexture;
	Texture baseTexture;
	Model AsteroideGrande_M;
	Model AsteroideChico_M;
	bool hayGrande, hayChico, grandeEnTodos;
	MeshModel* cuadroMarca;
	MeshModel* mallaBase;
	MeshModel* mallaPilares;
	glm::vec3 centroBase, adelanteBase, derechaBase;
	Material Material_pista;
	Material Material_marca;
	Material Material_asteroide;
	bool inicializado;
	Oceano* oceano;

	// para que AjustarAPista busque cerca del último punto (y no brinque de un tramo a otro)
	int tramoActual;
	int indiceActual;

	// ubicaciones de los uniforms (se guardan al inicio de cada Dibujar)
	GLuint uniformModel, uniformColor, uniformSpecularIntensity, uniformShininess, uniformEmision;
};

