#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }

	//Animaciones básicas que se inician con teclado (igual que luzHolocron o luzPinzaRover de la práctica)
	bool getPuertaCapsula() { return puertaCapsula; }		// tecla 2: abre / cierra la puerta de la cápsula
	bool getPanelesSatelite() { return panelesSatelite; }	// tecla 3: abre / cierra los paneles del satélite

	//Luces de tipo spotlight que se prenden y apagan con teclado
	bool getLinterna() { return linterna; }					// tecla F: linterna ligada a la cámara
	bool getLuzNave() { return luzNave; }					// tecla L: faro de miNave
	bool getFocoBase() { return focoBase; }					// tecla J: foco de la base de inicio

	//Cámara del avatar o cámara libre
	bool getCamaraLibre() { return camaraLibre; }			// tecla C
	void SalirDeCamaraLibre() { camaraLibre = false; }

	// NUEVO (no viene en las prácticas): acciones de UN solo pulso.
	// E y K no prenden ni apagan algo, disparan una acción una sola vez (entrar o salir de un planeta,
	// subir o bajar de la nave). En ManejaTeclado solo se anota que se presionó la tecla y el main la
	// "consume" con estos getters: regresan true una vez y se ponen en false, igual que getXChange()
	// regresa el movimiento del mouse y lo pone en cero.
	bool getEntrarSalir();									// tecla E
	bool getAbordarNave();									// tecla K

	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;
	bool puertaCapsula;
	bool panelesSatelite;
	bool linterna;
	bool luzNave;
	bool focoBase;
	bool camaraLibre;
	bool entrarSalir;
	bool abordarNave;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);

};

