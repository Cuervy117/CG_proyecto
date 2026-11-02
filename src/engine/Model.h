#pragma once
#include <vector>
#include<string>
#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include <glm.hpp>

#include "Mesh_tn.h"
#include "Texture.h"

class Model
{
public:
	Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	// Igual que RenderModel(), pero antes de cada malla manda al shader el color difuso (Kd)
	// de su material cuando ese material no trae textura (por ejemplo la capsula).
	// 'tinte' multiplica ese color; con glm::vec3(1.0f) se ve tal cual se exporto de Blender.
	void RenderModel(GLuint uniformColor, const glm::vec3& tinte);
	// Dibuja todas las mallas con la textura que se le pasa, ignorando la de su .mtl
	// (por ejemplo, el mismo asteroide como roca de la selva o como aguja del desierto).
	void RenderModelConTextura(Texture& textura);

	// Textura que se usa en los materiales que NO encontraron su imagen (por ejemplo, cuando el .mtl
	// exportado por Blender dice texture_diffuse.png pero la imagen se renombro). nullptr = plain.png.
	void SetTexturaRespaldo(Texture* textura) { texturaRespaldo = textura; }
	bool usaTexturaPropia() const;   // true si algun material cargo su propia imagen
	void ClearModel();

	// utilidad (practica 9): saber si el modelo cargo algo y cuantas mallas trae
	int getMeshCount() { return (int)MeshList.size(); }

	// utilidades del proyecto: medidas reales de la geometria cargada
	// (sirven para centrar y escalar el modelo sin volver a leer el .obj)
	bool estaCargado() const { return !MeshList.empty(); }
	int getNumTriangulos() const { return numTriangulos; }
	glm::vec3 getCentro() const;   // centro de la caja envolvente
	float getRadio() const;        // mitad del lado mas grande de la caja envolvente
	glm::vec3 getTamano() const { return MeshList.empty() ? glm::vec3(0.0f) : (maximo - minimo); }   // largo, alto y ancho

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene); //assimp
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	std::vector<MeshModel*>MeshList;
	std::vector<Texture*>TextureList;
	std::vector<unsigned int>meshTotex;

	std::vector<glm::vec3> ColorList;   // color difuso (Kd) de cada material
	std::vector<bool> TieneTextura;     // true si el material encontro su propia textura
	std::string carpetaModelo;          // carpeta del archivo, ahi se buscan primero sus texturas
	glm::vec3 minimo;                   // caja envolvente de todos los vertices
	glm::vec3 maximo;
	int numTriangulos;
	Texture* texturaRespaldo;
};

