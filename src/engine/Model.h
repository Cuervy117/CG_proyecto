#pragma once
#include <vector>
#include <glm.hpp>
#include<string>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include "Mesh_tn.h"
#include "Texture.h"

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();
	void RenderModelConTextura(Texture& textura);
	void SetTexturaRespaldo(Texture* textura) { texturaRespaldo = textura; }
	bool usaTexturaPropia() const;
	bool estaCargado() const { return !MeshList.empty(); }
	int getNumTriangulos() const { return numTriangulos; }
	glm::vec3 getCentro() const;
	float getRadio() const;
	glm::vec3 getTamano() const { return estaCargado() ? maximo - minimo : glm::vec3(0); }

	// utilidad (practica 9): saber si el modelo cargo algo y cuantas mallas trae
	int getMeshCount() { return (int)MeshList.size(); }

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene); //assimp
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	std::vector<MeshModel*>MeshList;
	std::vector<Texture*>TextureList;
	std::vector<unsigned int>meshTotex;
	std::string modelDirectory;
	std::vector<bool> TieneTextura;
	glm::vec3 minimo, maximo;
	int numTriangulos = 0;
	Texture* texturaRespaldo = nullptr;
};

