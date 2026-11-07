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
	// Modelos independientes para los dos rivales de la carrera
	Nave_M = Model();
	Nave_M.LoadModel("Models/X_wing_obj.obj");
	FreezerStation_M.LoadModel("Models/freezer_station.obj");
	Ufo_M = Model();
	Ufo_M.LoadModel("Models/UFO.obj");

	// planeta estilizado: solo se usa si no se pudo cargar planetaRoca.obj
	Planeta_M = Model();
	Planeta_M.LoadModel("Models/asteroide/Stylized_Planets.obj");

	FighterHangar_M.LoadModel("Models/Fighter_02.obj");
	Luna_M.LoadModel("Models/Moon.obj");
	PlanetaReksai_M.LoadModel("Models/planet_reksai.obj");
	SateliteBase_M.LoadModel("Models/satelite/satelite_base.fbx");
	SatelitePanelDer_M.LoadModel("Models/satelite/satelite_panel_der.fbx");
	SatelitePanelIzq_M.LoadModel("Models/satelite/satelite_panel_izq.fbx");
	SateliteAspa_M.LoadModel("Models/satelite/satelite_aspa_g.fbx");

	printf("[OK] Modelos iniciales del espacio cargados; planetas y capsula quedan bajo demanda.\n");
}

void LoadPlanetaRocoso()
{
	static bool intentado = false;
	if (intentado) return;
	intentado = true;
	printf("Cargando Models/planetaRoca/planetaRoca.obj bajo demanda ...\n");
	PlanetaRocoso_M = Model();
	PlanetaRocoso_M.LoadModel("Models/planetaRoca/planetaRoca.obj");
}

void LoadPlanetaSelvatico()
{
	static bool intentado = false;
	if (intentado) return;
	intentado = true;
	printf("Cargando Models/planetaSelva/planetaSelvatico.obj bajo demanda ...\n");
	PlanetaSelvatico_M = Model();
	PlanetaSelvatico_M.LoadModel("Models/planetaSelva/planetaSelvatico.obj");
}

void LoadCapsulaModels()
{
	static bool intentado = false;
	if (intentado) return;
	intentado = true;
	CapsulaCuerpo_M = Model();
	CapsulaCuerpo_M.LoadModel("Models/capsula/capsulaCuerpo.obj");
	CapsulaPuerta_M = Model();
	CapsulaPuerta_M.LoadModel("Models/capsula/capsulaPuerta.obj");
}

bool LoadSkybox()
{
	// Orden de las seis caras para la convención TexCoords = -pos del shader.
	const char* lados[] = { "ft", "bk", "dn", "up", "rt", "lf" };
	std::array<CarasCielo, 3> cielos;
	for (size_t i = 0; i < 6; ++i) {
		const std::string lado = lados[i];
		cielos[static_cast<size_t>(Fondo::Espacio)][i] = "Textures/spacenebulaskybox/nebula" + lado + ".png";
		cielos[static_cast<size_t>(Fondo::Roca)][i] = "Textures/Vacio/cielo_" + lado + ".png";
		cielos[static_cast<size_t>(Fondo::Selva)][i] = "Textures/Skybox/cupertin-lake_" + lado + ".tga";
	}
	return skybox.Inicializar(cielos);
}
