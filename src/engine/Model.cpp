#include "Model.h"
#include <unordered_map>
#include <cfloat>
#include <cmath>


Model::Model()
{
	// NUEVO: caja envolvente vacía (se va agrandando con cada vértice que se carga)
	minimo = glm::vec3(FLT_MAX);
	maximo = glm::vec3(-FLT_MAX);
	numTriangulos = 0;
}

void Model::LoadModel(const std::string & fileName)
{
	Assimp::Importer importer;//					Pasa de Polygons y Quads a triangulos, modifica orden para el origen, generar normales si el  objeto no tiene, trata vértices iguales como 1 solo
	//const aiScene *scene=importer.ReadFile(fileName,aiProcess_Triangulate |aiProcess_FlipUVs|aiProcess_GenSmoothNormals|aiProcess_JoinIdenticalVertices);
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene || !scene->mRootNode)
	{	
		// NUEVO (corrección): en la práctica faltaba el %s del motivo, por eso nunca se veía el error
		printf("Fallo en cargar el modelo: %s\n   Motivo: %s\n", fileName.c_str(), importer.GetErrorString());
		return;
	}
	LoadNode(scene->mRootNode, scene);
	LoadMaterials(scene);
	}

void Model::ClearModel()
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		if (MeshList[i])
		{
			delete MeshList[i];
			MeshList[i] = nullptr;

		}
	}

	for (unsigned int i = 0; i < TextureList.size(); i++)
	{
		if (TextureList[i])
		{
			delete TextureList[i];
			TextureList[i] = nullptr;
		}
	}

}

void Model::RenderModel()
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		unsigned int materialIndex = meshTotex[i];
		// NUEVO (corrección): en la práctica decía if (!materialIndex< TextureList.size()&& ...).
		// El ! se evalúa antes que el <, así que los modelos con un solo material nunca enlazaban su
		// textura (se quedaban con la del objeto que se dibujó antes).
		if (materialIndex < TextureList.size() && TextureList[materialIndex])
		{
			TextureList[materialIndex]->UseTexture();
		}
		MeshList[i]->RenderMeshModel();

	}


}

void Model::RenderModel(GLuint uniformColor, glm::vec3 tinte)
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		unsigned int materialIndex = meshTotex[i];
		glm::vec3 color = tinte;
		if (materialIndex < TextureList.size() && TextureList[materialIndex])
		{
			TextureList[materialIndex]->UseTexture();
			// sin imagen propia se usa el color Kd del .mtl (con imagen manda la imagen)
			if (!TieneTextura[materialIndex])
			{
				color = ColorList[materialIndex] * tinte;
			}
		}
		glUniform3f(uniformColor, color.x, color.y, color.z);
		MeshList[i]->RenderMeshModel();
	}
}

void Model::RenderModelConTextura(Texture& textura)
{
	textura.UseTexture();
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		MeshList[i]->RenderMeshModel();
	}
}

glm::vec3 Model::getCentro() const
{
	if (MeshList.empty())
		return glm::vec3(0.0f);
	return (minimo + maximo) * 0.5f;
}

glm::vec3 Model::getTamano() const
{
	if (MeshList.empty())
		return glm::vec3(0.0f);
	return maximo - minimo;
}

float Model::getRadio() const
{
	if (MeshList.empty())
		return 1.0f;
	glm::vec3 semiejes = (maximo - minimo) * 0.5f;
	return fmaxf(fmaxf(semiejes.x, semiejes.y), fmaxf(semiejes.z, 0.001f));
}


Model::~Model()
{
}

void Model::LoadNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i <node->mNumMeshes; i++)
	{
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
	}
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		LoadNode(node->mChildren[i], scene);
	}
}

void Model::LoadMesh(aiMesh * mesh, const aiScene * scene)
{
	// NUEVO: Blender a veces exporta mallas de solo líneas o puntos (aristas sueltas). No se pueden
	// dibujar como GL_TRIANGLES y no traen normales (el programa tronaba al leer mNormals), se ignoran.
	if (!(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) || mesh->mNumVertices == 0)
		return;

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x,mesh->mVertices[i].y ,mesh->mVertices[i].z });

		// NUEVO: caja envolvente del modelo completo (ver getCentro, getTamano y getRadio)
		glm::vec3 p(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		minimo = glm::min(minimo, p);
		maximo = glm::max(maximo, p);

		//UV
		if (mesh->mTextureCoords[0])//si tiene coordenadas de texturizado
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x,mesh->mTextureCoords[0][i].y});
		}
		else
		{
			vertices.insert(vertices.end(), { 0.0f,0.0f });
		}
		//Normals importante, las normales son negativas porque la luz interactúa con ellas de esa forma, cómo se vio con el dado/cubo
		if (mesh->mNormals)
		{
			vertices.insert(vertices.end(), { -mesh->mNormals[i].x,-mesh->mNormals[i].y ,-mesh->mNormals[i].z });
		}
		else
		{
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });   // NUEVO: por si la malla no trae normales
		}
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// NUEVO: después de aiProcess_Triangulate solo deben quedar triángulos (3 índices)
		if (face.mNumIndices != 3)
			continue;
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}
	if (indices.empty())
		return;

	MeshModel* newMesh = new MeshModel();
	newMesh->CreateMeshModel(&vertices[0], &indices[0], vertices.size(), indices.size());
	MeshList.push_back(newMesh);
	meshTotex.push_back(mesh->mMaterialIndex);
	numTriangulos += (int)(indices.size() / 3);
}

void Model::LoadMaterials(const aiScene * scene)
{
	std::unordered_map<std::string, Texture*> loadedTextures; // Mapa para evitar duplicados
	TextureList.resize(scene->mNumMaterials);
	ColorList.resize(scene->mNumMaterials);
	TieneTextura.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene ->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		TextureList[i] = nullptr;
		TieneTextura[i] = false;

		// NUEVO: color difuso Kd del .mtl (lo usa RenderModel(uniformColor, tinte)).
		// Blender lo exporta en espacio lineal; se pasa a sRGB (^1/2.2) para que se vea como en Blender.
		ColorList[i] = glm::vec3(1.0f);
		aiColor3D kd(1.0f, 1.0f, 1.0f);
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, kd) == AI_SUCCESS)
		{
			ColorList[i] = glm::pow(glm::max(glm::vec3(kd.r, kd.g, kd.b), glm::vec3(0.0f)), glm::vec3(1.0f / 2.2f));
		}

		if (material->GetTextureCount(aiTextureType_DIFFUSE	))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE,0,&path)==AI_SUCCESS)
			{
				//para quitar del path del modelo todo lo que este antes del \ de ubicación de directorio
				// NUEVO: también se busca / porque algunos .mtl traen rutas con / (C:/Users/...)
				std::string fullPath = std::string(path.data);
				std::size_t idx = fullPath.find_last_of("/\\");
				std::string filename = (idx == std::string::npos) ? fullPath : fullPath.substr(idx + 1);

				std::string texPath = std::string("Textures/") + filename;
				// Verifica si ya se cargó esta textura antes
				auto it = loadedTextures.find(texPath);
				if (it != loadedTextures.end())
				{
					TextureList[i] = it->second; // Reutiliza la textura ya cargada
				}
				else
				{
					Texture* newTex = new Texture(texPath.c_str());
					// NUEVO: siempre LoadTextureA (RGBA). En la práctica los .jpg usaban LoadTexture (RGB)
					// y las imágenes con un ancho que no es múltiplo de 4 se veían chuecas.
					if (newTex->LoadTextureA())
					{
						TextureList[i] = newTex;
						loadedTextures[texPath] = newTex; // Almacena para reutilizarla
					}
					else
					{
						printf("Fallo en cargar la Textura :%s\n", texPath.c_str());
						delete newTex;
					}
				}
				TieneTextura[i] = (TextureList[i] != nullptr);
			}
		}
		//Si no se pudo cargar la textura o no tiene una textura asignada, asignar una textura por defecto
		if (!TextureList[i])
		{
			TextureList[i] = new Texture("Textures/plain.png"); //textura que se aplicará a los modelos si no tienen textura o la textura no se puede cargar
			TextureList[i]->LoadTextureA();
		}

	}
}

