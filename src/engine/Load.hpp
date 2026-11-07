#pragma once

#include <vector>
#include <string>

#include "Texture.h"
#include "Model.h"
#include "Skybox.h"

// Texturas, modelos y skybox del espacio. Se declaran en el main (como en la práctica) y aquí
// solo se cargan, para que el main no quede tan largo.

// Texturas
extern Texture plainTexture;
extern Texture llamaTexture;

// Modelos
extern Model Nave_M;				// primer rival (X-Wing)
extern Model FreezerStation_M;
extern Model FighterHangar_M;
extern Model Ufo_M; // segundo rival (UFO)
extern Model PlanetaReksai_M;
extern Model Luna_M;
extern Model Planeta_M;				// planeta estilizado (respaldo si falla planetaRoca.obj)
extern Model PlanetaRocoso_M;		// planeta rocoso (Vacío)
extern Model PlanetaSelvatico_M;	// planeta selvático (Shinobi)
extern Model CapsulaCuerpo_M;		// cápsula de regreso de los planetas
extern Model CapsulaPuerta_M;		// puerta de la cápsula (pieza separada para animarla)
extern Model SateliteBase_M;		// satélite jerárquico: base
extern Model SatelitePanelDer_M;	// panel derecho
extern Model SatelitePanelIzq_M;	// panel izquierdo
extern Model SateliteAspa_M;		// aspa (rotor)

// Skybox
extern Skybox skybox;

void LoadTextures();	// plain y fuego de los motores
void LoadModels();		// modelos necesarios para la zona inicial (espacio)
void LoadPlanetaRocoso();	// carga diferida al entrar al planeta rocoso
void LoadPlanetaSelvatico();	// carga diferida al entrar al planeta selvático
void LoadCapsulaModels();	// carga diferida compartida por las superficies
bool LoadSkybox();
