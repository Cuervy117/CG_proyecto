#include "Input.hpp"

#include <cmath>

EstadoEscena escena;

static bool teclasAnteriores[1024] = { false };

// true solo en el frame en que la tecla pasa de suelta a presionada
static bool Flanco(bool* teclas, int tecla)
{
	return teclas[tecla] && !teclasAnteriores[tecla];
}


void InitEscena()
{
	escena.roverPos = glm::vec3(0.0f, -0.3f, 0.0f);
	escena.roverYaw = 0.0f;
	escena.roverScale = 0.5f;
	escena.roverSpeed = 0.1f;

	escena.avionPos = glm::vec3(-7.5f, 6.0f, 20.0f);
	escena.avionYaw = 180.0f;
	escena.avionScale = 0.2f;
	escena.avionSpeed = 5.5f;

	escena.holocronPos = glm::vec3(-20.0f, 10.0f, -3.0f);

	escena.colorFaro = 0;
	escena.holocronEncendida = true;
	escena.pinzaEncendida = true;

	// Nave flotando
	escena.navePos = glm::vec3(0.0f, 3.0f, 0.0f);
	escena.naveYaw = 0.0f;
	escena.naveScale = 1.0f;
	escena.naveSpeed = 4.0f;

	// Arranca todo apagado
	escena.naveEncendida = false;
	escena.motor1Encendido = false;
	escena.motor2Encendido = false;
}


void ProcesarTeclas(Window& ventana, Camera& camara, GLfloat delta, GLfloat objDelta)
{
	bool* teclas = ventana.getsKeys();

	// camara
	camara.keyControl(teclas, delta);
	camara.mouseControl(ventana.getXChange(), ventana.getYChange());

	// ---- rover: DESHABILITADO (era la escena anterior) ----
	// se deja comentado para no borrar la logica, ya no se usa
	// glm::vec3 roverFrente = glm::vec3(glm::cos(glm::radians(escena.roverYaw)), 0.0f,
	// 	-glm::sin(glm::radians(escena.roverYaw)));
	// if (teclas[GLFW_KEY_UP])   // adelante
	// 	escena.roverPos += roverFrente * escena.roverSpeed * objDelta;
	// if (teclas[GLFW_KEY_DOWN]) // atras
	// 	escena.roverPos -= roverFrente * escena.roverSpeed * objDelta;

	// ---- nave: flechas para desplazarse, ESPACIO sube, M baja ----
	// Frente y derecha de la nave segun su yaw.
	glm::vec3 naveFrente = glm::vec3(glm::sin(glm::radians(escena.naveYaw)), 0.0f,
		glm::cos(glm::radians(escena.naveYaw)));
	// La derecha de la pantalla (con yaw = 0 es -X)
	glm::vec3 naveDerecha = glm::vec3(-glm::cos(glm::radians(escena.naveYaw)), 0.0f,
		glm::sin(glm::radians(escena.naveYaw)));

	if (teclas[GLFW_KEY_UP])    // adelante
		escena.navePos += naveFrente * escena.naveSpeed * objDelta;
	if (teclas[GLFW_KEY_DOWN])  // atras
		escena.navePos -= naveFrente * escena.naveSpeed * objDelta;
	if (teclas[GLFW_KEY_RIGHT]) // derecha
		escena.navePos += naveDerecha * escena.naveSpeed * objDelta;
	if (teclas[GLFW_KEY_LEFT])  // izquierda
		escena.navePos -= naveDerecha * escena.naveSpeed * objDelta;
	if (teclas[GLFW_KEY_SPACE]) // subir
		escena.navePos.y += escena.naveSpeed * objDelta;
	if (teclas[GLFW_KEY_M])     // bajar
		escena.navePos.y -= escena.naveSpeed * objDelta;

	// Limite para no perder la nave en el vacio (quitar si no se quiere)
	escena.navePos.x = glm::clamp(escena.navePos.x, -100.0f, 100.0f);
	escena.navePos.y = glm::clamp(escena.navePos.y, -100.0f, 100.0f);
	escena.navePos.z = glm::clamp(escena.navePos.z, -100.0f, 100.0f);

	// avion
	glm::vec3 avionFrente = glm::vec3(glm::sin(glm::radians(escena.avionYaw)), 0.0f,
		glm::cos(glm::radians(escena.avionYaw)));
	if (teclas[GLFW_KEY_I]) // subir
		escena.avionPos.y += escena.avionSpeed * objDelta;
	if (teclas[GLFW_KEY_K]) // bajar
		escena.avionPos.y -= escena.avionSpeed * objDelta;
	if (teclas[GLFW_KEY_J]) // adelante
		escena.avionPos += avionFrente * escena.avionSpeed * objDelta;
	if (teclas[GLFW_KEY_L]) // atras
		escena.avionPos -= avionFrente * escena.avionSpeed * objDelta;

	// C: cicla el color del faro del avion 
	if (Flanco(teclas, GLFW_KEY_C)) {
		escena.colorFaro = escena.colorFaro + 1;
		if (escena.colorFaro > 5)
			escena.colorFaro = 0;
	}

	// H: prende/apaga la luz del holocron
	if (Flanco(teclas, GLFW_KEY_H))
		escena.holocronEncendida = !escena.holocronEncendida;

	// P: prende/apaga la luz de la pinza del rover
	if (Flanco(teclas, GLFW_KEY_P))
		escena.pinzaEncendida = !escena.pinzaEncendida;

	// N: prende/apaga al mismo tiempo las dos luces de la nave 
	if (Flanco(teclas, GLFW_KEY_N))
		escena.naveEncendida = !escena.naveEncendida;

	// 1: motor 1
	if (Flanco(teclas, GLFW_KEY_1))
		escena.motor1Encendido = !escena.motor1Encendido;

	// 2: motor 2 
	if (Flanco(teclas, GLFW_KEY_2))
		escena.motor2Encendido = !escena.motor2Encendido;

	// Guardar para poder detectar el flanco en el siguiente frame.
	for (int i = 0; i < 1024; i++)
		teclasAnteriores[i] = teclas[i];
}
