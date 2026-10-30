#pragma once

#include <vector>

#include "Mesh_tn.h"

struct Geometria
{
	std::vector<MeshModel*> meshListModel;

	MeshModel* pisoMesh = nullptr;
	MeshModel* vegetacionMesh = nullptr;
	MeshModel* dadoMesh = nullptr;
	MeshModel* holocronMesh = nullptr;
	MeshModel* llamaMesh = nullptr;  

	// Practica 9: quad unitario en XY con UV 0..1 (paneles con textura animada)
	MeshModel* quadMesh = nullptr;
};

extern Geometria geometria;


void CreateObjects();    // los 2 triangulos, el piso y la vegetacion
void CrearDado();        // cubo texturizado con los logos
void CrearHolocron();    // holocron
void CrearLlamas();      // llamas de los motores

// Practica 9: quad unitario (1x1) en el plano XY, UV 0..1 y normal +Z.
// Sirve para paneles; para acostarlo en el piso basta un rotate(-90, X).
void CrearQuad();

// Llama a las tres anteriores en el orden del escenario.
void CreateScene();
