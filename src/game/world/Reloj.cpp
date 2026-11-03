#include "Reloj.h"

#include <cmath>
#include <cstdio>
#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader_light.h"
#include "Texture.h"
#include "Mesh_tn.h"

namespace
{
	Shader shaderHud;
	GLuint uModel = 0, uProyeccion = 0, uOffset = 0, uColor = 0;
	Texture texPanel, texDigitos, texIconos;
	MeshModel* quadPanel = nullptr;    // UV 0..1: imagen completa
	MeshModel* quadDigito = nullptr;   // UV de la celda superior izquierda de una rejilla de 4 x 3
	MeshModel* quadIcono = nullptr;    // UV de la celda superior izquierda de una rejilla de 4 x 3
	MeshModel* quadLetrero = nullptr;  // dos celdas de ancho (letreros DÍA y NOCHE)
	bool listo = false;

	// medidas de hud_reloj.png; en una ventana de 768 de alto se dibuja a la mitad (256 x 300)
	const float PANEL_ANCHO = 512.0f, PANEL_ALTO = 600.0f;
	const float MARGEN = 14.0f;
	const glm::vec4 COLOR_LCD(0.47f, 1.0f, 0.55f, 1.0f);   // verde de pantalla de cristal líquido

	float escala = 0.5f;
	glm::vec2 origen(0.0f);   // esquina inferior izquierda del panel en la pantalla

	// cuadro de 0 a 1 en x, y con las UV indicadas
	MeshModel* crearQuad(float u0, float v0, float u1, float v1)
	{
		GLfloat v[] = {
			0.0f, 0.0f, 0.0f,   u0, v0,   0.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 0.0f,   u1, v0,   0.0f, 0.0f, 1.0f,
			1.0f, 1.0f, 0.0f,   u1, v1,   0.0f, 0.0f, 1.0f,
			0.0f, 1.0f, 0.0f,   u0, v1,   0.0f, 0.0f, 1.0f,
		};
		unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
		MeshModel* m = new MeshModel();
		m->CreateMeshModel(v, indices, 32, 6);
		return m;
	}

	// número n (0-9, 10 = dos puntos, 11 = vacío) -> desplazamiento de UV dentro del atlas de 4 x 3
	glm::vec2 offsetDigito(int n)
	{
		return glm::vec2(0.25f * (float)(n % 4), -(float)(n / 4) / 3.0f);
	}

	// ícono i en el atlas de 4 x 3: 0 nublado, 1 sol, 2 estrellas, 3 polvo, 4 AM, 5 PM, 6 marcador, 7 luna
	glm::vec2 offsetIcono(int i)
	{
		return glm::vec2(0.25f * (float)(i % 4), -(float)(i / 4) / 3.0f);
	}

	// letrero k en la última fila del atlas: 0 = DÍA, 1 = NOCHE (cada uno ocupa dos celdas)
	glm::vec2 offsetLetrero(int k)
	{
		return glm::vec2(0.5f * (float)k, -2.0f / 3.0f);
	}

	// Dibuja un rectángulo dado en pixeles de hud_reloj.png (x, y desde la esquina superior izquierda)
	void dibujar(MeshModel* quad, Texture& textura, float x, float y, float ancho, float alto,
		const glm::vec2& offset, const glm::vec4& color)
	{
		glm::mat4 m = glm::translate(glm::mat4(1.0f),
			glm::vec3(origen.x + x * escala, origen.y + (PANEL_ALTO - y - alto) * escala, 0.0f))
			* glm::scale(glm::mat4(1.0f), glm::vec3(ancho * escala, alto * escala, 1.0f));
		glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(m));
		glUniform2f(uOffset, offset.x, offset.y);
		glUniform4f(uColor, color.r, color.g, color.b, color.a);
		textura.UseTexture();
		quad->RenderMeshModel();
	}

	bool cargar(Texture& t, const char* ruta)
	{
		t = Texture(ruta);
		if (t.LoadTextureA())
			return true;
		printf("[ERROR] Reloj: no se encontro %s\n", ruta);
		return false;
	}
}

void InicializarReloj()
{
	shaderHud.CreateFromFiles("shaders/hud.vert", "shaders/hud.frag");
	GLuint programa = shaderHud.GetShaderID();
	uModel = glGetUniformLocation(programa, "model");
	uProyeccion = glGetUniformLocation(programa, "projection");
	uOffset = glGetUniformLocation(programa, "toffset");
	uColor = glGetUniformLocation(programa, "colorHud");

	bool ok = cargar(texPanel, "Textures/hud_reloj.png");
	ok = cargar(texDigitos, "Textures/hud_digitos.png") && ok;
	ok = cargar(texIconos, "Textures/hud_iconos.png") && ok;

	quadPanel = crearQuad(0.0f, 0.0f, 1.0f, 1.0f);
	quadDigito = crearQuad(0.0f, 2.0f / 3.0f, 0.25f, 1.0f);   // como numeroVertices de la práctica 9
	quadIcono = crearQuad(0.0f, 2.0f / 3.0f, 0.25f, 1.0f);
	quadLetrero = crearQuad(0.0f, 2.0f / 3.0f, 0.5f, 1.0f);
	listo = ok;
	if (ok)
		printf("[OK] Reloj en pantalla listo\n");
}

void DibujarReloj(int anchoVentana, int altoVentana, float horaDelDia, int dia, int clima)
{
	if (!listo || anchoVentana <= 0 || altoVentana <= 0)
		return;

	// estado de OpenGL para dibujar encima de todo, con transparencia
	GLboolean habiaProfundidad = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaderHud.UseShader();
	glm::mat4 proyeccion = glm::ortho(0.0f, (float)anchoVentana, 0.0f, (float)altoVentana, -1.0f, 1.0f);
	glUniformMatrix4fv(uProyeccion, 1, GL_FALSE, glm::value_ptr(proyeccion));
	escala = 0.5f * (float)altoVentana / 768.0f;
	origen = glm::vec2(MARGEN, (float)altoVentana - MARGEN - PANEL_ALTO * escala);   // esquina superior izquierda

	// 1) panel de latón
	dibujar(quadPanel, texPanel, 0.0f, 0.0f, PANEL_ANCHO, PANEL_ALTO, glm::vec2(0.0f), glm::vec4(1.0f));

	// 2) hora HH:MM en formato de 12 horas; los dos puntos parpadean cada media hora del juego
	horaDelDia = fmodf(fmaxf(horaDelDia, 0.0f), 24.0f);
	bool esDeDia = horaDelDia >= 6.0f && horaDelDia < 18.0f;   // de 6 am a 6 pm
	int h24 = (int)horaDelDia;
	int minutos = (int)((horaDelDia - (float)h24) * 60.0f);
	int h12 = h24 % 12;
	if (h12 == 0) h12 = 12;
	bool puntos = fmodf(horaDelDia * 2.0f, 1.0f) < 0.5f;
	int cifras[5] = { h12 / 10, h12 % 10, puntos ? 10 : 11, minutos / 10, minutos % 10 };
	for (int i = 0; i < 5; i++)
		dibujar(quadDigito, texDigitos, 70.0f + 40.0f * i, 166.0f, 40.0f, 64.0f, offsetDigito(cifras[i]), COLOR_LCD);

	// 3) AM / PM
	dibujar(quadIcono, texIconos, 126.0f, 252.0f, 88.0f, 38.0f, offsetIcono(h24 < 12 ? 4 : 5), COLOR_LCD);

	// 4) marcador de la fase del día: de medianoche (arriba) a mediodía (centro) y medianoche (abajo)
	float y = 44.0f + (horaDelDia / 24.0f) * 280.0f - 14.0f;
	dibujar(quadIcono, texIconos, 456.0f, y, 28.0f, 28.0f, offsetIcono(6), glm::vec4(1.0f));

	// 5) clima: espacio = estrellas, selva = nublado, desierto = sol de día y polvo de noche
	int icono = 2;
	if (clima == 1) icono = 0;
	else if (clima == 2) icono = esDeDia ? 1 : 3;
	else if (clima == 3) icono = esDeDia ? 1 : 2;   // océano despejado
	dibujar(quadIcono, texIconos, 377.0f, 380.0f, 76.0f, 76.0f, offsetIcono(icono), glm::vec4(1.0f));

	// 6) letrero DÍA con sol (de 6 am a 6 pm) o NOCHE con luna (de 6 pm a 6 am), y el número de día
	dibujar(quadIcono, texIconos, 30.0f, 504.0f, 64.0f, 64.0f, offsetIcono(esDeDia ? 1 : 7), glm::vec4(1.0f));
	dibujar(quadLetrero, texIconos, 118.0f, 510.0f, 112.0f, 56.0f, offsetLetrero(esDeDia ? 0 : 1), COLOR_LCD);
	if (dia < 0) dia = 0;
	if (dia > 999) dia = 999;
	int cifrasDia[3] = { dia / 100, (dia / 10) % 10, dia % 10 };
	for (int i = 0; i < 3; i++)
		dibujar(quadDigito, texDigitos, 330.0f + 50.0f * i, 508.0f, 44.0f, 56.0f, offsetDigito(cifrasDia[i]), COLOR_LCD);

	// dejar OpenGL como estaba
	glUniform2f(uOffset, 0.0f, 0.0f);
	glDisable(GL_BLEND);
	if (habiaProfundidad)
		glEnable(GL_DEPTH_TEST);
}
