#include "Window.h"

Window::Window()
{
	width = 800;
	height = 600;
	mainWindow = NULL;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = true;
	puertaCapsula = false;
	panelesSatelite = true;
	linterna = false;
	luzNave = true;
	focoBase = true;
	camaraLibre = false;
	entrarSalir = false;
	abordarNave = false;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	mainWindow = NULL;
	lastX = 0.0f;
	lastY = 0.0f;
	xChange = 0.0f;
	yChange = 0.0f;
	mouseFirstMoved = true;
	//valores iniciales de las banderas que cambian con el teclado
	puertaCapsula = false;		// la puerta de la cápsula empieza cerrada
	panelesSatelite = true;		// los paneles del satélite empiezan abiertos
	linterna = false;			// la linterna empieza apagada
	luzNave = true;				// el faro de miNave empieza encendido
	focoBase = true;			// el foco de la base empieza encendido
	camaraLibre = false;		// se empieza con la cámara del avatar
	entrarSalir = false;
	abordarNave = false;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Fallo inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Proyecto Final: Viajes espaciales y evolucion de especies intergalacticas", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Fallo inicializacion de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
	return 0;
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
	// NUEVO: cursor capturado. Así el mouse gira la cámara sin límite y el cursor no se sale de la
	// ventana (en la práctica el cursor se queda libre). TAB lo libera y lo vuelve a capturar.
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(mainWindow, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}

bool Window::getEntrarSalir()
{
	bool pulso = entrarSalir;
	entrarSalir = false;
	return pulso;
}

bool Window::getAbordarNave()
{
	bool pulso = abordarNave;
	abordarNave = false;
	return pulso;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	// NUEVO: TAB libera el cursor (para usar otras ventanas) o lo vuelve a capturar
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		bool capturado = glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
		glfwSetInputMode(window, GLFW_CURSOR, capturado ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		theWindow->mouseFirstMoved = true;   // para que la cámara no brinque al volver a capturarlo
	}

	//Animación básica 1: abrir y cerrar la puerta de la cápsula de regreso
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		theWindow->puertaCapsula = !theWindow->puertaCapsula;
	}
	//Animación básica 2: abrir y cerrar los paneles del satélite
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		theWindow->panelesSatelite = !theWindow->panelesSatelite;
	}

	//Spotlights: se prenden y apagan con teclado
	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		theWindow->linterna = !theWindow->linterna;
	}
	if (key == GLFW_KEY_L && action == GLFW_PRESS)
	{
		theWindow->luzNave = !theWindow->luzNave;
	}
	if (key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		theWindow->focoBase = !theWindow->focoBase;
	}

	//Cambio de cámara: cámara del avatar (tercera persona) o cámara libre
	if (key == GLFW_KEY_C && action == GLFW_PRESS)
	{
		theWindow->camaraLibre = !theWindow->camaraLibre;
	}

	// NUEVO: acciones de un solo pulso (ver Window.h)
	// E: entrar a un planeta cuando se está cerca de su atmósfera, o salir al espacio desde un planeta
	if (key == GLFW_KEY_E && action == GLFW_PRESS)
	{
		theWindow->entrarSalir = true;
	}
	// K: subir a miNave en la base (empieza la carrera) o bajar / subir de ella en los planetas
	if (key == GLFW_KEY_K && action == GLFW_PRESS)
	{
		theWindow->abordarNave = true;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			//printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			//printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}


Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
