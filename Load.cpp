#include "Load.hpp"

#include <stdio.h>
#include <string>
#include <vector>

Recursos recursos;


// Funciones de carga
void LoadTextures()
{
	recursos.brickTexture = Texture("Textures/brick.png");
	recursos.brickTexture.LoadTextureA();

	recursos.dirtTexture = Texture("Textures/dirt.png");
	recursos.dirtTexture.LoadTextureA();

	recursos.plainTexture = Texture("Textures/plain.png");
	recursos.plainTexture.LoadTextureA();

	recursos.pisoTexture = Texture("Textures/piso.tga");
	recursos.pisoTexture.LoadTextureA();

	recursos.agaveTexture = Texture("Textures/Agave.tga");
	recursos.agaveTexture.LoadTextureA();

	recursos.dadoTexture = Texture("Textures/star wars logos.png");
	recursos.dadoTexture.LoadTextureA();

	recursos.holocronTexture = Texture("Textures/swl_holo.png");
	recursos.holocronTexture.LoadTextureA();
}


void LoadBlackhawk()
{
	recursos.blackhawkModel = Model();
	recursos.blackhawkModel.LoadModel("Models/uh60.obj");
}

void LoadRover()
{
	recursos.roverCuerpo = Model();
	recursos.roverBrazo1 = Model();
	recursos.roverBrazo2 = Model();
	recursos.roverBrazo3 = Model();
	recursos.roverPinza = Model();

	recursos.roverLlantaDelDer = Model();
	recursos.roverLlantaDelIzq = Model();
	recursos.roverLlantaMedDer = Model();
	recursos.roverLlantaMedIzq = Model();
	recursos.roverLlantaTrasDer = Model();
	recursos.roverLlantaTrasIzq = Model();

	recursos.roverSuspDelDer = Model();
	recursos.roverSuspDelIzq = Model();
	recursos.roverSuspMedDer = Model();
	recursos.roverSuspMedIzq = Model();
	recursos.roverSuspTrasDer = Model();
	recursos.roverSuspTrasIzq = Model();

	recursos.roverCuerpo.LoadModel("Models/rover/rover_cuerpo.fbx");
	recursos.roverBrazo1.LoadModel("Models/rover/rover_brazo_001.fbx");
	recursos.roverBrazo2.LoadModel("Models/rover/rover_brazo_002.fbx");
	recursos.roverBrazo3.LoadModel("Models/rover/rover_brazo_003.fbx");
	recursos.roverPinza.LoadModel("Models/rover/rover_pinza.fbx");

	recursos.roverSuspDelDer.LoadModel("Models/rover/susp_del_der.fbx");
	recursos.roverSuspDelIzq.LoadModel("Models/rover/susp_del_izq.fbx");
	recursos.roverSuspMedDer.LoadModel("Models/rover/susp_med_der.fbx");
	recursos.roverSuspMedIzq.LoadModel("Models/rover/susp_med_izq.fbx");
	recursos.roverSuspTrasDer.LoadModel("Models/rover/susp_tra_der.fbx");
	recursos.roverSuspTrasIzq.LoadModel("Models/rover/susp_tra_izq.fbx");

	recursos.roverLlantaDelDer.LoadModel("Models/rover/llanta_del_der.fbx");
	recursos.roverLlantaDelIzq.LoadModel("Models/rover/llanta_del_izq.fbx");
	recursos.roverLlantaMedDer.LoadModel("Models/rover/llanta_med_der.fbx");
	recursos.roverLlantaMedIzq.LoadModel("Models/rover/llanta_med_izq.fbx");
	recursos.roverLlantaTrasDer.LoadModel("Models/rover/llanta_tra_der.fbx");
	recursos.roverLlantaTrasIzq.LoadModel("Models/rover/llanta_tra_izq.fbx");

}

void LoadAvion()
{
	recursos.avionModel = Model();
	recursos.avionModel.LoadModel("Models/avion/avion_cuerpo.obj");
	recursos.avionHelice = Model();
	recursos.avionHelice.LoadModel("Models/avion/avion_helice.obj");
}

void LoadHolocron()
{
	recursos.holocronModel = Model();
	recursos.holocronModel.LoadModel("Models/holocron/holo_texturizado.obj");
}

void LoadCohete()
{
	recursos.coheteModel = Model();
	recursos.coheteModel.LoadModel("Models/rocket/rocket.obj");
}

void LoadHolocronP05()
{
	recursos.holocronBase = Model();
	for (int i = 0; i < 8; i++)
		recursos.holocronEsquinas[i] = Model();

	recursos.holocronBase.LoadModel("Models/holocron/holocron_base.fbx");

	recursos.holocronEsquinas[0].LoadModel("Models/holocron/holocron_esq_sup_izq_tra.fbx");
	recursos.holocronEsquinas[1].LoadModel("Models/holocron/holocron_esq_sup_der_tra.fbx");
	recursos.holocronEsquinas[2].LoadModel("Models/holocron/holocron_esq_sup_izq_fron.fbx");
	recursos.holocronEsquinas[3].LoadModel("Models/holocron/holocron_esq_sup_der_fron.fbx");
	recursos.holocronEsquinas[4].LoadModel("Models/holocron/holocron_esq_inf_izq_tra.fbx");
	recursos.holocronEsquinas[5].LoadModel("Models/holocron/holocron_esq_inf_der_tra.fbx");
	recursos.holocronEsquinas[6].LoadModel("Models/holocron/holocron_esq_inf_izq_fron.fbx");
	recursos.holocronEsquinas[7].LoadModel("Models/holocron/holocron_esq_inf_der_fron.fbx");
}

void LoadSkyboxP1()
{
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulaft.png");  // -Z frente
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulabk.png");  // +Z atras
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebuladn.png");  // -Y abajo
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulaup.png");  // +Y arriba
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulart.png");  // +X derecha
	skyboxFaces.push_back("Textures/spacenebulaskybox/nebulalf.png");  // -X izquierda

	recursos.skybox = Skybox(skyboxFaces);
}

void LoadSkyboxCupertin()
{

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");  // +X derecha
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");  // -X izquierda
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");  // -Y abajo
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");  // +Y arriba
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");  // +Z atras
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");  // -Z frente

	recursos.skybox = Skybox(skyboxFaces);
}

void LoadTexturasAnimacion()
{
	recursos.flechaTexture = Texture("Textures/flechas.tga");
	recursos.flechaTexture.LoadTextureA();

	recursos.numerosTexture = Texture("Textures/numerosbase.tga");
	recursos.numerosTexture.LoadTextureA();

	recursos.numero1Texture = Texture("Textures/numero1.tga");
	recursos.numero1Texture.LoadTextureA();

	recursos.numero2Texture = Texture("Textures/numero2.tga");
	recursos.numero2Texture.LoadTextureA();
}


void LoadNave()
{
	recursos.naveModel = Model();
	recursos.naveModel.LoadModel("Models/Fighter_03_Obj/Fighter_03.obj");
}

void LoadLlamas()
{
	// Textura de fuego/plasma generada para las llamas de los motores.
	recursos.llamaTexture = Texture("Textures/fuego.png");
	recursos.llamaTexture.LoadTextureA();
}

void LoadAsteroide()
{
	// Asteroide 
	recursos.asteroideModel = Model();
	recursos.asteroideModel.LoadModel("Models/asteroide/Rocky_Asteroid_6.obj");
	recursos.plainTexture = Texture("Textures/plain.png");
	recursos.plainTexture.LoadTextureA();
}

void LoadPlaneta()
{
	recursos.planetaModel = Model();
	recursos.planetaModel.LoadModel("Models/asteroide/Stylized_Planets.obj");
}

void LoadSatelite()
{
	// Practica 9: mismas piezas y jerarquia que la Practica 5
	recursos.satelite_base = Model();
	recursos.satelite_panel_der = Model();
	recursos.satelite_panel_izq = Model();
	recursos.satelite_aspa = Model();

	recursos.satelite_base.LoadModel("Models/satelite/satelite_base.fbx");
	recursos.satelite_panel_der.LoadModel("Models/satelite/satelite_panel_der.fbx");
	recursos.satelite_panel_izq.LoadModel("Models/satelite/satelite_panel_izq.fbx");
	recursos.satelite_aspa.LoadModel("Models/satelite/satelite_aspa_g.fbx");
}

void LoadAll()
{
	LoadTextures();

	LoadBlackhawk();

	LoadRover();

	LoadAvion();

	LoadHolocron();

	LoadCohete();

	LoadHolocronP05();

	LoadNave();

	LoadLlamas();

	LoadAsteroide();

	LoadPlaneta();

	LoadSatelite();

	LoadSkyboxP1();

}
