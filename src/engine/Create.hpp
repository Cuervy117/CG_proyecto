#pragma once

#include <vector>
#include <glew.h>

#include "Mesh_tn.h"

// Mallas creadas con código, igual que en la práctica: se guardan en meshListModel (declarada en el main).
//  meshListModel[0]: cuadro unitario en el plano XY (guía de la órbita del satélite)
//  meshListModel[1]: cuadro de la llama de los motores de la nave rival
//  meshListModel[2]: cubo unitario (poste de las luminarias)
extern std::vector<MeshModel*> meshListModel;

void CreateObjects();
