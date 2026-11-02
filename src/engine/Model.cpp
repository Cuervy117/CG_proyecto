#include "Model.h"
#include <unordered_map>
#include <fstream>
#include <cfloat>
#include <cmath>

// true si el archivo existe y se puede abrir (para decidir dónde está cada textura)
static bool existeArchivoModelo(const std::string& ruta)
{
	std::ifstream f(ruta.c_str());
	return f.good();
}

Model::Model()
{
	minimo = glm::vec3(FLT_MAX);
	maximo = glm::vec3(-FLT_MAX);
	numTriangulos = 0;
	texturaRespaldo = nullptr;
}

void Model::LoadModel(const std::string & fileName)
{
	// Carpeta del modelo, por ejemplo "Models/planetaSelva/". Ahí se buscan primero las texturas que pide su .mtl
	std::size_t ultimaBarra = fileName.find_last_of("/\\");
	carpetaModelo = (ultimaBarra == std::string::npos) ? std::string("") : fileName.substr(0, ultimaBarra + 1);

	Assimp::Importer importer;//					Pasa de Polygons y Quads a triangulos, modifica orden para el origen, generar normales si el  objeto no tiene, trata vértices iguales como 1 solo
	//const aiScene *scene=importer.ReadFile(fileName,aiProcess_Triangulate |aiProcess_FlipUVs|aiProcess_GenSmoothNormals|aiProcess_JoinIdenticalVertices);
	const aiScene *scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);
	if (!scene || !scene->mRootNode)
	{	
		// antes el printf no mostraba GetErrorString porque faltaba su %s
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
		// Antes decía (!materialIndex< TextureList.size() && ...): el ! se evalúa antes que el <,
		// así que los modelos con un solo material nunca enlazaban su propia textura.
		if (materialIndex < TextureList.size() && TextureList[materialIndex])
		{
			// sin imagen propia se usa la textura de respaldo (si se asigno una)
			if (texturaRespaldo && !TieneTextura[materialIndex])
				texturaRespaldo->UseTexture();
			else
				TextureList[materialIndex]->UseTexture();
		}
		MeshList[i]->RenderMeshModel();

	}


}

void Model::RenderModel(GLuint uniformColor, const glm::vec3& tinte)
{
	for (unsigned int i = 0; i < MeshList.size(); i++)
	{
		unsigned int materialIndex = meshTotex[i];
		glm::vec3 color = tinte;
		if (materialIndex < TextureList.size())
		{
			if (texturaRespaldo && !TieneTextura[materialIndex])
				texturaRespaldo->UseTexture();
			else if (TextureList[materialIndex])
				TextureList[materialIndex]->UseTexture();
			// sin textura propia (ni de respaldo) se usa el color Kd del .mtl; con textura manda la textura
			if (!TieneTextura[materialIndex] && !texturaRespaldo)
				color = ColorList[materialIndex] * tinte;
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

bool Model::usaTexturaPropia() const
{
	for (size_t i = 0; i < TieneTextura.size(); i++)
		if (TieneTextura[i])
			return true;
	return false;
}

glm::vec3 Model::getCentro() const
{
	if (MeshList.empty())
		return glm::vec3(0.0f);
	return (minimo + maximo) * 0.5f;
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
	// Mallas de solo líneas o puntos (aristas sueltas que a veces exporta Blender) no se pueden
	// dibujar como GL_TRIANGLES y además no traen normales: se ignoran.
	if (!(mesh->mPrimitiveTypes & aiPrimitiveType_TRIANGLE) || mesh->mNumVertices == 0)
		return;

	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertices.insert(vertices.end(), { mesh->mVertices[i].x,mesh->mVertices[i].y ,mesh->mVertices[i].z });

		// caja envolvente del modelo completo (el main la usa para centrarlo y escalarlo)
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
			vertices.insert(vertices.end(), { 0.0f, 0.0f, 0.0f });
		}
	}
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// después de aiProcess_Triangulate solo deberían quedar triángulos
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
	ColorList.assign(scene->mNumMaterials, glm::vec3(1.0f));
	TieneTextura.assign(scene->mNumMaterials, false);
	for (unsigned int i = 0; i < scene ->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];
		TextureList[i] = nullptr;

		// Color difuso Kd del .mtl. Blender lo exporta en espacio lineal; se pasa a sRGB (^1/2.2)
		// para que se vea como en el visor de Blender.
		aiColor3D kd(1.0f, 1.0f, 1.0f);
		if (material->Get(AI_MATKEY_COLOR_DIFFUSE, kd) == AI_SUCCESS)
		{
			ColorList[i] = glm::pow(glm::max(glm::vec3(kd.r, kd.g, kd.b), glm::vec3(0.0f)), glm::vec3(1.0f / 2.2f));
		}

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE,0,&path)==AI_SUCCESS)
			{
				std::string fullPath = std::string(path.data);
				std::size_t idx = fullPath.find_last_of("/\\"); // algunos exportadores usan / y otros \ como separador
				std::string filename = (idx == std::string::npos) ? fullPath : fullPath.substr(idx + 1);

				// Dónde se busca la textura, en este orden:
				// 1) la ruta del .mtl relativa a la carpeta del modelo (por si usa subcarpetas)
				// 2) junto al modelo, por ejemplo Models/planetaSelva/texture_diffuse.png
				// 3) en Textures/, como lo hacía el código original
				// Antes solo se buscaba en Textures/, y como Texture::LoadTextureA regresaba true
				// aunque no existiera el archivo, el modelo se quedaba con una textura vacía (negro).
				std::vector<std::string> candidatos;
				bool rutaRelativa = !fullPath.empty() && fullPath.find(':') == std::string::npos
					&& fullPath[0] != '/' && fullPath[0] != '\\';
				if (rutaRelativa)
					candidatos.push_back(carpetaModelo + fullPath);
				candidatos.push_back(carpetaModelo + filename);
				candidatos.push_back(std::string("Textures/") + filename);

				std::string texPath;
				for (size_t c = 0; c < candidatos.size(); c++)
				{
					if (existeArchivoModelo(candidatos[c]))
					{
						texPath = candidatos[c];
						break;
					}
				}

				if (texPath.empty())
				{
					printf("No se encontro la textura %s (se busco en %s y en Textures/)\n",
						filename.c_str(), carpetaModelo.empty() ? "./" : carpetaModelo.c_str());
				}
				else
				{
					// Verifica si ya se cargó esta textura antes
					auto it = loadedTextures.find(texPath);
					if (it != loadedTextures.end())
					{
						TextureList[i] = it->second; // Reutiliza la textura ya cargada
					}
					else
					{
						Texture* newTex = new Texture(texPath.c_str());
						// LoadTextureA fuerza 4 canales (RGBA): funciona igual con png, tga y jpg
						if (newTex->LoadTextureA())
						{
							TextureList[i] = newTex;
							loadedTextures[texPath] = newTex; // Almacena para reutilizarla
						}
						else
						{
							printf("Fallo en cargar la textura: %s\n", texPath.c_str());
							delete newTex;
						}
					}
					TieneTextura[i] = (TextureList[i] != nullptr);
				}
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
