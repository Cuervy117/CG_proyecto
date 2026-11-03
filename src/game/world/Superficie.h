#pragma once
// Superficie de los planetas. Cada planeta es una zona FINITA alrededor de un camino cerrado:
//  - Selvático: avenida de árboles a ambos lados del camino, una franja de selva alrededor,
//    plantas luminosas de noche y NPCs (Naruto, zorro blanco, caballo-delfín, pulpo y tigre de fuego).
//  - Rocoso: desierto rojo con cristales violetas en el camino, agujas de roca, montañas al
//    fondo y NPCs (monstruo negro, escorpión, Cerbero y serpiente alada).
// Poco después del último NPC hay un portal: al cruzarlo regresas al espacio.
// Todo se genera una sola vez al iniciar (semilla fija: siempre sale igual) y en cada cuadro
// solo se dibujan los objetos cercanos que están frente a la cámara.

#include <glew.h>
#include <glm.hpp>

enum TipoSuperficie
{
	SUPERFICIE_ROCOSA = 0,
	SUPERFICIE_SELVATICA = 1
};

// Ubicaciones de los uniforms de shader_light que usa la superficie
struct UniformsSuperficie
{
	GLuint model;
	GLuint color;
	GLuint offsetTextura;   // toffset del vertex shader
	GLuint especular;
	GLuint brillo;
	GLuint emision;         // uniform vec3 emision del fragment shader
};

// Niebla (shader_light) y cielo (skybox) de cada planeta según la hora del día
struct AmbienteSuperficie
{
	glm::vec3 colorNiebla;
	float densidadNiebla;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;   // rgb = color, a = intensidad
};

// Carga modelos y texturas y genera los dos mundos. Llamar una vez, con OpenGL ya iniciado.
void InicializarSuperficies();

// Dibuja piso, camino, objetos y NPCs del planeta. El shader_light debe estar activo y con luces.
// 'tiempo' (segundos) mueve las animaciones de los NPCs.
void DibujarSuperficie(TipoSuperficie tipo, const glm::vec3& posCamara, const glm::vec3& dirCamara,
	float factorDia, float tiempo, const UniformsSuperficie& u);

// Regresa la posición corregida para que el jugador no salga de la zona del planeta
// (no puede alejarse del camino más de cierta distancia) ni atraviese troncos, rocas, etc.
glm::vec3 LimitarPosicion(TipoSuperficie tipo, const glm::vec3& posicion);

// Carrera dentro del planeta (la nave va sobre el camino, del inicio hasta pasar el portal)
float LargoCarreraSuperficie(TipoSuperficie tipo);
void PuntoCarreraSuperficie(TipoSuperficie tipo, float distancia, float carril, float altura, glm::vec3& posicion, glm::vec3& adelante);
glm::vec3 AjustarACamino(TipoSuperficie tipo, const glm::vec3& posicion, float altura, glm::vec3* adelante);

// Modelo de Paul (para usarlo también como avatar del jugador); nullptr si no se cargó
class Model;
Model* ModeloPaul();

// true cuando el jugador está pasando por el hueco del portal de salida de ese planeta
bool CruzandoPortal(TipoSuperficie tipo, const glm::vec3& posicion);

AmbienteSuperficie ObtenerAmbiente(TipoSuperficie tipo, float factorDia);

glm::vec3 PosicionCapsula(TipoSuperficie tipo);   // dónde va la cápsula de regreso
glm::vec3 PosicionInicio(TipoSuperficie tipo);    // dónde aparece la cámara al aterrizar
float YawInicio(TipoSuperficie tipo);             // hacia dónde mira al aterrizar (grados)

// Cuántos objetos y triángulos se dibujaron en el último cuadro (útil para medir el rendimiento)
void EstadisticasSuperficie(int& objetosDibujados, int& triangulosDibujados);
