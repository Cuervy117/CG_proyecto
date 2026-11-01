#pragma once

#include <glew.h>
#include <glm.hpp>

#include "Window.h"
#include "Camera.h"
#include <array>
#include <cstddef>

// Entrada desacoplada del estado heredado de prácticas. Cada frame conserva
// teclas sostenidas, flancos de pulsación y deltas de mouse/rueda para C.
namespace cgame
{
struct InputFrame
{
    static constexpr std::size_t KeyCount = 1024;

    std::array<bool, KeyCount> down{};
    std::array<bool, KeyCount> pressed{};
    float deltaTime = 0.0f;
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
    float wheelDelta = 0.0f;

    bool isDown(int key) const;
    bool wasPressed(int key) const;
};

// Mapeo C del control único definido en PLAN_MAESTRO.md §8.2.
// E se conserva como una acción contextual: Experience decide si interactúa
// o asciende antes de aplicar movimiento vertical.
struct ControlFrame
{
    glm::vec2 movementXZ{0.0f};
    bool descend = false;
    bool ascendOrInteractDown = false;
    bool ascendOrInteractPressed = false;
    bool planetMode = false;
    bool raceMode = false;
    bool flightMode = false;
    bool observationMode = false;
    bool launchRocket = false;
    bool toggleCapsuleDoor = false;
    bool togglePanels = false;
    bool toggleShipSpot = false;
    bool togglePlatformSpot = false;
    bool nextObservationTarget = false;
    float observationZoom = 0.0f;
    float mouseDeltaX = 0.0f;
    float mouseDeltaY = 0.0f;
};

ControlFrame mapControls(const InputFrame& input);

class InputCapture
{
public:
    explicit InputCapture(Window& window);
    ~InputCapture();

    InputCapture(const InputCapture&) = delete;
    InputCapture& operator=(const InputCapture&) = delete;

    const InputFrame& update(float dt);
    const InputFrame& frame() const;

private:
    static void onScroll(GLFWwindow* window, double xOffset, double yOffset);

    Window& m_window;
    GLFWwindow* m_glfwWindow = nullptr;
    InputFrame m_frame;
    std::array<bool, InputFrame::KeyCount> m_previous{};
    float m_pendingWheel = 0.0f;
};
}

struct EstadoEscena
{
	// Rover
	glm::vec3 roverPos;
	GLfloat   roverYaw;
	GLfloat   roverScale;
	GLfloat   roverSpeed;

	// Avion
	glm::vec3 avionPos;
	GLfloat   avionYaw;
	GLfloat   avionScale;
	GLfloat   avionSpeed;

	// Holocron
	glm::vec3 holocronPos;

	// Nave (practica 8)
	glm::vec3 navePos;
	GLfloat   naveYaw;
	GLfloat   naveScale;
	GLfloat   naveSpeed;

	// Luces: las teclas solo escriben estas banderas.
	unsigned int colorFaro;         
	bool holocronEncendida;
	bool pinzaEncendida;

	// Practica 8: nave con dos motores
	bool naveEncendida;     // N -> point light + spotlight de la nave
	bool motor1Encendido;   // 1 -> llama + point light del motor 1
	bool motor2Encendido;   // 2 -> llama + point light del motor 2
};

extern EstadoEscena escena;


void InitEscena(); // valores iniciales
void ProcesarTeclas(Window& ventana, Camera& camara, GLfloat delta, GLfloat objDelta);  // una vez por frame
