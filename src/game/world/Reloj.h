#pragma once
/*
NUEVO (no viene en las prácticas): reloj en pantalla (HUD), siempre visible encima de la escena
 - Hora del ciclo de día y noche (HH:MM y AM/PM): los 120 s del ciclo equivalen a 24 horas.
 - Marcador de la fase del día (luna - sol - luna) que baja por una barra.
 - Clima del lugar (espacio, selva, desierto u océano), letrero DÍA/NOCHE y número de día.

Cómo funciona:
 - Usa su propio shader (shaders/hud.vert y hud.frag): sin luces, sin cámara y con proyección
   ORTOGONAL en pixeles (glm::ortho), así lo que se dibuja queda fijo en la pantalla.
 - Se dibuja al final del cuadro con GL_DEPTH_TEST apagado (queda encima de todo) y con blending
   para las partes transparentes de las imágenes.
 - Los dígitos usan la técnica de la práctica 9 (textura con movimiento): un atlas de números en una
   rejilla de 4 x 3 (hud_digitos.png) y el uniform toffset elige qué número se ve en cada cuadro.
*/

#include <glew.h>
#include <glm.hpp>

#include "Shader_light.h"
#include "Texture.h"
#include "Mesh_tn.h"

class Reloj
{
public:
	Reloj();
	// Carga el shader del HUD y sus texturas (con OpenGL ya iniciado)
	void InicializarReloj();
	// horaDelDia: de 0 a 24. dia: contador de días.
	// clima: 0 = espacio, 1 = selva (nublado), 2 = desierto, 3 = océano (despejado).
	// De 6:00 a 18:00 el letrero dice DÍA con un sol; de 18:00 a 6:00 dice NOCHE con una luna.
	void DibujarReloj(int anchoVentana, int altoVentana, float horaDelDia, int dia, int clima);
	~Reloj();

private:
	MeshModel* CrearCuadro(float u0, float v0, float u1, float v1);
	glm::vec2 OffsetCelda(int n);		// celda n de una rejilla de 4 x 3 -> desplazamiento de UV
	glm::vec2 OffsetLetrero(int k);		// letrero k de la última fila (0 = DÍA, 1 = NOCHE)
	void Dibujar(MeshModel* cuadro, Texture& textura, float x, float y, float ancho, float alto,
		glm::vec2 toffset, glm::vec4 color);

	Shader* shaderHud;
	GLuint uniformModel, uniformProjection, uniformTextureOffset, uniformColorHud;
	Texture panelTexture;
	Texture digitosTexture;
	Texture iconosTexture;
	MeshModel* cuadroPanel;		// UV 0..1: imagen completa
	MeshModel* cuadroDigito;	// UV de la celda superior izquierda de una rejilla de 4 x 3
	MeshModel* cuadroIcono;		// igual que cuadroDigito (otra textura)
	MeshModel* cuadroLetrero;	// dos celdas de ancho (letreros DÍA y NOCHE)
	bool listo;
	float escala;				// en una ventana de 768 de alto el panel se dibuja a la mitad
	glm::vec2 origen;			// esquina inferior izquierda del panel en la pantalla
};
