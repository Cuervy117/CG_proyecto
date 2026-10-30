#pragma once

#include <string>
#include <vector>

#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

struct Recursos
{
	// Texturas
	Texture brickTexture;
	Texture dirtTexture;
	Texture plainTexture;
	Texture pisoTexture;
	Texture agaveTexture;
	Texture dadoTexture;
	Texture holocronTexture;
	Texture llamaTexture;

	// Practica 9: texturas animadas (vienen del zip del profesor)
	Texture flechaTexture;      // flechas.tga    - tira que se desplaza en u
	Texture numerosTexture;     // numerosbase.tga - rejilla 4x3 con los numeros
	Texture numero1Texture;     // numero1.tga
	Texture numero2Texture;     // numero2.tga

	// Modelos
	Model blackhawkModel;

	// Rover
	Model roverCuerpo;
	Model roverBrazo1;
	Model roverBrazo2;
	Model roverBrazo3;
	Model roverPinza;

	Model roverLlantaDelDer;
	Model roverLlantaDelIzq;
	Model roverLlantaMedDer;
	Model roverLlantaMedIzq;
	Model roverLlantaTrasDer;
	Model roverLlantaTrasIzq;

	Model roverSuspDelDer;
	Model roverSuspDelIzq;
	Model roverSuspMedDer;
	Model roverSuspMedIzq;
	Model roverSuspTrasDer;
	Model roverSuspTrasIzq;

	Model avionModel;
	Model avionHelice;

	Model holocronModel;

	Model coheteModel;
	Model holocronBase;
	Model holocronEsquinas[8];

	Model naveModel;

	Model asteroideModel;
	Model planetaModel;

	// Practica 9: satelite jerarquico (P05)
	Model satelite_base;
	Model satelite_panel_der;
	Model satelite_panel_izq;
	Model satelite_aspa;

	// Skybox
	Skybox skybox;
};

extern Recursos recursos;


void LoadTextures();        // las 7 texturas del escenario
void LoadBlackhawk();       // Blackhawk (uh60.obj)
void LoadRover();           // las 11 piezas del rover jerarquico
void LoadAvion();           // avion_texturizado.obj
void LoadSkyboxP1();          // cupertin-lake

// Practica 9
void LoadTexturasAnimacion(); // flechas, numerosbase, numero1, numero2
void LoadSkyboxCupertin();    // skybox del profesor (mismo orden GL: +X,-X,+Y,-Y,+Z,-Z)
void LoadHolocron();        // holo_texturizado.obj
void LoadCohete();
void LoadHolocronP05();

// Practica 8 (escena nueva; estas se llaman desde el main)
void LoadNave();            // modelo de la nave espacial
void LoadLlamas();          // textura de fuego/plasma de los motores
void LoadAsteroide();       // asteroide gris que recibe las luces de la nave
void LoadPlaneta();         // planeta central con anillos
void LoadSatelite();        // satelite jerarquico: base, paneles y aspa

void LoadAll();
