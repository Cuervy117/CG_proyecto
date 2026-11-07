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
	void RenderPart(const std::string& name);
	bool HasPart(const std::string& name) const;
	void ClearModel();

	// NUEVO: igual que RenderModel(), pero antes de cada malla manda al shader el color difuso (Kd)
	// de su material cuando ese material no tiene textura. La cápsula de regreso no trae imágenes,
	// solo colores en su .mtl (casco blanco, vidrio azul, franja oscura); con RenderModel() normal
	// toda la cápsula saldría de un solo color. 'tinte' multiplica ese color (1,1,1 = sin cambio).
	void RenderModel(GLuint uniformColor, glm::vec3 tinte);

	// NUEVO: dibuja todas las mallas con la textura que se le pasa e ignora la de su .mtl.
	// Así el mismo asteroide sirve de roca con musgo, de aguja del desierto o de montaña.
	void RenderModelConTextura(Texture& textura);

	// NUEVO: medidas de la geometría ya cargada (caja envolvente de todos los vértices).
	// Los modelos salen de Blender con tamaños y posiciones muy distintos (el planeta mide cientos de
	// unidades y el asteroide menos de una). Con estas medidas se centran y se escalan en el main:
	//   model = glm::scale(model, glm::vec3(radioDeseado / Modelo.getRadio()));
	//   model = glm::translate(model, -Modelo.getCentro());
	int getMeshCount() const { return static_cast<int>(MeshList.size()); }
	bool estaCargado() const { return !MeshList.empty(); }
	int getNumTriangulos() const { return numTriangulos; }
	glm::vec3 getCentro() const;	// centro de la caja envolvente
	glm::vec3 getTamano() const;	// largo, alto y ancho de la caja envolvente
	float getRadio() const;			// mitad del lado más grande de la caja envolvente

	~Model();

private:
	void LoadNode(aiNode* node, const aiScene* scene); //assimp
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);
	std::string modelDirectory;
	std::vector<MeshModel*>MeshList;
	std::vector<std::string> meshNames;
	std::vector<Texture*>TextureList;
	std::vector<unsigned int>meshTotex;

	// NUEVO: datos extra de los materiales y de la geometría
	std::vector<glm::vec3> ColorList;	// color difuso (Kd) de cada material
	std::vector<bool> TieneTextura;		// true si el material encontró su propia imagen
	glm::vec3 minimo;					// esquina mínima de la caja envolvente
	glm::vec3 maximo;					// esquina máxima de la caja envolvente
	int numTriangulos;
};

