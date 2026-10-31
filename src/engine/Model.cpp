#include "Model.h"
#include <unordered_map>
#include <algorithm>
#include <fstream>

static std::string normalizarRuta(std::string ruta)
{
	for (char& c : ruta)
	{
		if (c == '\\')
			c = '/';
	}
	while (ruta.rfind("./", 0) == 0)
		ruta.erase(0, 2);
	return ruta;
}

static std::string nombreArchivo(const std::string& ruta)
{
	const std::size_t idx = ruta.find_last_of('/');
	return (idx == std::string::npos) ? ruta : ruta.substr(idx + 1);
}

static bool existeArchivo(const std::string& ruta)
{
	std::ifstream archivo(ruta, std::ios::binary);
	return archivo.good();
}

static std::string resolverRutaTextura(const std::string& rutaImportada,
	const std::string& directorioModelo)
{
	const std::string ruta = normalizarRuta(rutaImportada);
	const std::string archivo = nombreArchivo(ruta);
	std::vector<std::string> candidatos;

	auto agregar = [&candidatos](const std::string& candidato)
	{
		if (candidato.empty())
			return;
		if (std::find(candidatos.begin(), candidatos.end(), candidato) == candidatos.end())
			candidatos.push_back(candidato);
	};

	if (ruta.size() > 1 && ruta[1] == ':')
		agregar(ruta);

	const std::size_t texturas = ruta.find("Textures/");
	if (texturas != std::string::npos)
		agregar(ruta.substr(texturas));
	else if (!ruta.empty() && ruta[0] != '/')
		agregar("Textures/" + ruta);

	agregar("Textures/" + archivo);
	agregar(directorioModelo + "/" + ruta);
	agregar(directorioModelo + "/" + archivo);
	agregar(ruta);

	for (const std::string& candidato : candidatos)
	{
		if (existeArchivo(candidato))
			return candidato;
	}

	return "Textures/" + archivo;
}

Model::Model()
{
}

void Model::LoadModel(const std::string & fileName)
{
	const std::string rutaModelo = normalizarRuta(fileName);
	const std::size_t barra = rutaModelo.find_last_of('/');
	modelDirectory = (barra == std::string::npos || barra == 0)
		? "." : rutaModelo.substr(0, barra);

	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene)
	{
		printf("Fallo en cargar el modelo: %s (%s)\n", fileName.c_str(), importer.GetErrorString());
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
		if (materialIndex < TextureList.size() && TextureList[materialIndex])
		{
			TextureList[materialIndex]->UseTexture();
		}
		MeshList[i]->RenderMeshModel();
	}
}

Model::~Model()
{
}

void Model::LoadNode(aiNode * node, const aiScene * scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
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
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		if (mesh->mTextureCoords[0])
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else
		{
			vertices.insert(vertices.end(), { 0.0f, 0.0f });
		}
		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	MeshModel* newMesh = new MeshModel();
	newMesh->CreateMeshModel(&vertices[0], &indices[0], vertices.size(), indices.size());
	MeshList.push_back(newMesh);
	meshTotex.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene * scene)
{
	std::unordered_map<std::string, Texture*> loadedTextures;
	TextureList.resize(scene->mNumMaterials);
	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		TextureList[i] = nullptr;
		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				const std::string texPath = resolverRutaTextura(path.data, modelDirectory);
				const std::string filename = nombreArchivo(texPath);

				auto it = loadedTextures.find(texPath);
				if (it != loadedTextures.end())
				{
					TextureList[i] = it->second;
				}
				else
				{
					Texture* newTex = new Texture(texPath.c_str());
					const std::size_t punto = filename.find_last_of('.');
					const std::string ext = (punto == std::string::npos)
						? "" : filename.substr(punto + 1);

					bool loaded = false;
					if (ext == "tga" || ext == "png")
					{
						loaded = newTex->LoadTextureA();
					}
					else
					{
						loaded = newTex->LoadTexture();
					}

					if (loaded)
					{
						TextureList[i] = newTex;
						loadedTextures[texPath] = newTex;
					}
					else
					{
						printf("Fallo al cargar la textura: %s\n", texPath.c_str());
						delete newTex;
					}
				}
			}
		}

		if (!TextureList[i])
		{
			Texture* fallback = new Texture("Textures/plain.png");
			if (fallback->LoadTextureA())
			{
				TextureList[i] = fallback;
			}
			else
			{
				printf("Material %u sin textura de respaldo\n", i);
				delete fallback;
			}
		}
	}
}
