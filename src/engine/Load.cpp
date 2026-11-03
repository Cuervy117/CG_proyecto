#include "Load.hpp"

#include <stdio.h>


void LoadTextures()
{
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();

	// textura de fuego/plasma para las llamas de los motores de la nave rival
	llamaTexture = Texture("Textures/fuego.png");
	llamaTexture.LoadTextureA();
}

void LoadModels()
{
	// nave rival de la carrera
	Nave_M = Model();
	Nave_M.LoadModel("Models/Fighter_03_Obj/Fighter_03.obj");

	// planeta estilizado: solo se usa si no se pudo cargar planetaRoca.obj
	Planeta_M = Model();
	Planeta_M.LoadModel("Models/asteroide/Stylized_Planets.obj");

	// satélite jerárquico: cada pieza es un modelo aparte para poder moverla desde su unión
	SateliteBase_M = Model();
	SateliteBase_M.LoadModel("Models/satelite/satelite_base.fbx");
	SatelitePanelDer_M = Model();
	SatelitePanelDer_M.LoadModel("Models/satelite/satelite_panel_der.fbx");
	SatelitePanelIzq_M = Model();
	SatelitePanelIzq_M.LoadModel("Models/satelite/satelite_panel_izq.fbx");
	SateliteAspa_M = Model();
	SateliteAspa_M.LoadModel("Models/satelite/satelite_aspa_g.fbx");

	// planetas: cada .obj pesa unos 100 MB y tarda varios segundos (la consola muestra el avance)
	printf("Cargando Models/planetaSelva/planetaSelvatico.obj ...\n");
	PlanetaSelvatico_M = Model();
	PlanetaSelvatico_M.LoadModel("Models/planetaSelva/planetaSelvatico.obj");
	printf("Cargando Models/planetaRoca/planetaRoca.obj ...\n");
	PlanetaRocoso_M = Model();
	PlanetaRocoso_M.LoadModel("Models/planetaRoca/planetaRoca.obj");

	// cápsula de regreso: el cuerpo y la puerta son modelos separados (la puerta gira con la tecla 2)
	CapsulaCuerpo_M = Model();
	CapsulaCuerpo_M.LoadModel("Models/capsula/capsulaCuerpo.obj");
	CapsulaPuerta_M = Model();
	CapsulaPuerta_M.LoadModel("Models/capsula/capsulaPuerta.obj");
}

void LoadSkybox()
{
	// NUEVO: el skybox se carga DESPUÉS de las texturas. Skybox.cpp no llama a
	// stbi_set_flip_vertically_on_load y usa el valor que dejó Texture::LoadTextureA (true);
	// si se cargara antes, las caras saldrían volteadas.
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulaft.png");
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulabk.png");
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebuladn.png");
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulaup.png");
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulart.png");
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulalf.png");

	skybox = Skybox(skyboxFaces);
}
