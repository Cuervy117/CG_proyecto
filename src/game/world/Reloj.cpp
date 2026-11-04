#include "Reloj.h"

#include <cmath>
#include <cstdio>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// medidas de hud_reloj.png en pixeles
const float PANEL_ANCHO = 512.0f;
const float PANEL_ALTO = 600.0f;
const float MARGEN = 14.0f;
const glm::vec4 COLOR_LCD(0.47f, 1.0f, 0.55f, 1.0f);   // verde de pantalla de cristal líquido


Reloj::Reloj()
{
	shaderHud = nullptr;
	uniformModel = 0;
	uniformProjection = 0;
	uniformTextureOffset = 0;
	uniformColorHud = 0;
	cuadroPanel = nullptr;
	cuadroDigito = nullptr;
	cuadroIcono = nullptr;
	cuadroLetrero = nullptr;
	listo = false;
	escala = 0.5f;
	origen = glm::vec2(0.0f);
}

// cuadro de 0 a 1 en x, y con las coordenadas de textura indicadas
MeshModel* Reloj::CrearCuadro(float u0, float v0, float u1, float v1)
{
	unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };
	GLfloat vertices[] = {
		//	x		y		z		u	v		nx		ny		nz
		0.0f, 0.0f, 0.0f,		u0, v0,		0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f,		u1, v0,		0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f,		u1, v1,		0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f,		u0, v1,		0.0f, 0.0f, 1.0f,
	};
	MeshModel* cuadro = new MeshModel();
	cuadro->CreateMeshModel(vertices, indices, 32, 6);
	return cuadro;
}

// número n (0-9, 10 = dos puntos, 11 = vacío) o ícono n -> desplazamiento de UV dentro del atlas de
// 4 x 3, igual que toffsetnumerou y toffsetnumerov de la práctica 9:
//   u = 0.25 * (columna),   v = -(fila) / 3
// íconos: 0 nublado, 1 sol, 2 estrellas, 3 polvo, 4 AM, 5 PM, 6 marcador, 7 luna
glm::vec2 Reloj::OffsetCelda(int n)
{
	return glm::vec2(0.25f * (float)(n % 4), -(float)(n / 4) / 3.0f);
}

// letrero k en la última fila del atlas: 0 = DÍA, 1 = NOCHE (cada uno ocupa dos celdas)
glm::vec2 Reloj::OffsetLetrero(int k)
{
	return glm::vec2(0.5f * (float)k, -2.0f / 3.0f);
}

// Dibuja un rectángulo dado en pixeles de hud_reloj.png (x, y desde la esquina superior izquierda)
void Reloj::Dibujar(MeshModel* cuadro, Texture& textura, float x, float y, float ancho, float alto,
	glm::vec2 toffset, glm::vec4 color)
{
	glm::mat4 model(1.0);
	model = glm::translate(model, glm::vec3(origen.x + x * escala, origen.y + (PANEL_ALTO - y - alto) * escala, 0.0f));
	model = glm::scale(model, glm::vec3(ancho * escala, alto * escala, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
	glUniform4fv(uniformColorHud, 1, glm::value_ptr(color));
	textura.UseTexture();
	cuadro->RenderMeshModel();
}

void Reloj::InicializarReloj()
{
	// shader propio del HUD (como el del skybox, que también usa la clase Shader)
	shaderHud = new Shader();
	shaderHud->CreateFromFiles("shaders/hud.vert", "shaders/hud.frag");
	uniformModel = shaderHud->GetModelLocation();
	uniformProjection = shaderHud->GetProjectionLocation();
	uniformTextureOffset = shaderHud->getOffsetLocation();
	uniformColorHud = glGetUniformLocation(shaderHud->GetShaderID(), "colorHud");

	panelTexture = Texture("Textures/hud_reloj.png");
	bool ok = panelTexture.LoadTextureA();
	digitosTexture = Texture("Textures/hud_digitos.png");
	ok = digitosTexture.LoadTextureA() && ok;
	iconosTexture = Texture("Textures/hud_iconos.png");
	ok = iconosTexture.LoadTextureA() && ok;

	cuadroPanel = CrearCuadro(0.0f, 0.0f, 1.0f, 1.0f);
	cuadroDigito = CrearCuadro(0.0f, 2.0f / 3.0f, 0.25f, 1.0f);   // como numeroVertices de la práctica 9
	cuadroIcono = CrearCuadro(0.0f, 2.0f / 3.0f, 0.25f, 1.0f);
	cuadroLetrero = CrearCuadro(0.0f, 2.0f / 3.0f, 0.5f, 1.0f);
	listo = ok;
	if (ok)
		printf("[OK] Reloj en pantalla listo\n");
}

void Reloj::DibujarReloj(int anchoVentana, int altoVentana, float horaDelDia, int dia, int clima)
{
	if (!listo || anchoVentana <= 0 || altoVentana <= 0)
		return;

	// estado de OpenGL para dibujar encima de todo, con transparencia
	GLboolean habiaProfundidad = glIsEnabled(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shaderHud->UseShader();
	// proyección ortogonal: x de 0 al ancho de la ventana y y de 0 al alto (en pixeles)
	glm::mat4 projection = glm::ortho(0.0f, (float)anchoVentana, 0.0f, (float)altoVentana, -1.0f, 1.0f);
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	escala = 0.5f * (float)altoVentana / 768.0f;
	origen = glm::vec2(MARGEN, (float)altoVentana - MARGEN - PANEL_ALTO * escala);   // esquina superior izquierda

	// 1) panel de latón
	Dibujar(cuadroPanel, panelTexture, 0.0f, 0.0f, PANEL_ANCHO, PANEL_ALTO, glm::vec2(0.0f), glm::vec4(1.0f));

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
		Dibujar(cuadroDigito, digitosTexture, 70.0f + 40.0f * i, 166.0f, 40.0f, 64.0f, OffsetCelda(cifras[i]), COLOR_LCD);

	// 3) AM / PM
	Dibujar(cuadroIcono, iconosTexture, 126.0f, 252.0f, 88.0f, 38.0f, OffsetCelda(h24 < 12 ? 4 : 5), COLOR_LCD);

	// 4) marcador de la fase del día: de medianoche (arriba) a mediodía (centro) y medianoche (abajo)
	float y = 44.0f + (horaDelDia / 24.0f) * 280.0f - 14.0f;
	Dibujar(cuadroIcono, iconosTexture, 456.0f, y, 28.0f, 28.0f, OffsetCelda(6), glm::vec4(1.0f));

	// 5) clima: espacio = estrellas, selva = nublado, desierto = sol de día y polvo de noche
	int icono = 2;
	if (clima == 1) icono = 0;
	else if (clima == 2) icono = esDeDia ? 1 : 3;
	else if (clima == 3) icono = esDeDia ? 1 : 2;   // océano despejado
	Dibujar(cuadroIcono, iconosTexture, 377.0f, 380.0f, 76.0f, 76.0f, OffsetCelda(icono), glm::vec4(1.0f));

	// 6) letrero DÍA con sol (de 6 am a 6 pm) o NOCHE con luna (de 6 pm a 6 am), y el número de día
	Dibujar(cuadroIcono, iconosTexture, 30.0f, 504.0f, 64.0f, 64.0f, OffsetCelda(esDeDia ? 1 : 7), glm::vec4(1.0f));
	Dibujar(cuadroLetrero, iconosTexture, 118.0f, 510.0f, 112.0f, 56.0f, OffsetLetrero(esDeDia ? 0 : 1), COLOR_LCD);
	if (dia < 0) dia = 0;
	if (dia > 999) dia = 999;
	int cifrasDia[3] = { dia / 100, (dia / 10) % 10, dia % 10 };
	for (int i = 0; i < 3; i++)
		Dibujar(cuadroDigito, digitosTexture, 330.0f + 50.0f * i, 508.0f, 44.0f, 56.0f, OffsetCelda(cifrasDia[i]), COLOR_LCD);

	// dejar OpenGL como estaba
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);
	glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));
	glDisable(GL_BLEND);
	if (habiaProfundidad)
		glEnable(GL_DEPTH_TEST);
}

Reloj::~Reloj()
{
}
