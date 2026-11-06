#include "Skybox.h"



Skybox::Skybox()
{
	skyMesh = nullptr;
	skyShader = nullptr;
	uniformProjection = 0;
	uniformView = 0;
	uniformTinte = 0;
	uniformBruma = 0;
	uniformCieloSolido = 0;
	tinteCielo = glm::vec3(1.0f);
	brumaHorizonte = glm::vec4(0.0f);
	cieloSolido = glm::vec4(0.0f);
}

bool Skybox::Inicializar(const std::array<CarasCielo, 3>& cielos)
{
	skyShader = new Shader();
	skyShader->CreateFromFiles("shaders/skybox.vert", "shaders/skybox.frag");
	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();
	// NUEVO: uniforms de ambientación de skybox.frag (ver Skybox.h)
	uniformTinte = glGetUniformLocation(skyShader->GetShaderID(), "tinteCielo");
	uniformBruma = glGetUniformLocation(skyShader->GetShaderID(), "brumaHorizonte");
	uniformCieloSolido = glGetUniformLocation(skyShader->GetShaderID(), "cieloSolido");
	tinteCielo = glm::vec3(1.0f);
	brumaHorizonte = glm::vec4(0.0f);
	cieloSolido = glm::vec4(0.0f);

	// Cada ubicación conserva su cubemap en GPU. Cambiar de zona sólo cambia el enlace.
	glGenTextures(static_cast<GLsizei>(texturas.size()), texturas.data());
	stbi_set_flip_vertically_on_load(true);
	for (size_t zona = 0; zona < cielos.size(); ++zona)
	{
		glBindTexture(GL_TEXTURE_CUBE_MAP, texturas[zona]);
		int lado = 0;
		for (size_t i = 0; i < cielos[zona].size(); ++i)
		{
			int ancho = 0, alto = 0;
			const std::string& ruta = cielos[zona][i];
			auto* pixels = stbi_load(ruta.c_str(), &ancho, &alto, nullptr, STBI_rgb_alpha);
			if (!pixels || ancho != alto || (i && ancho != lado))
			{
				fprintf(stderr, "Cara ausente o incompatible en el cubemap: %s\n", ruta.c_str());
				stbi_image_free(pixels);
				return false;
			}
			lado = ancho;
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<GLenum>(i), 0, GL_RGBA8,
				ancho, alto, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			stbi_image_free(pixels);
		}
		for (auto eje : {GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_R})
			glTexParameteri(GL_TEXTURE_CUBE_MAP, eje, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}


	//Creando el Mesh del skybox
	unsigned int skyboxIndices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//v0
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V1
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V2
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V3
		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V4
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V5
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,//V6
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f//V7
	};
	skyMesh = new MeshModel();
	skyMesh->CreateMeshModel(skyboxVertices, skyboxIndices, 64, 36);
	return true;

}

void Skybox::DrawSkybox(Fondo fondo, glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	glDepthMask(false);
	skyShader->UseShader();
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	// NUEVO: ambientación del lugar actual
	glUniform3f(uniformTinte, tinteCielo.x, tinteCielo.y, tinteCielo.z);
	glUniform4f(uniformBruma, brumaHorizonte.x, brumaHorizonte.y, brumaHorizonte.z, brumaHorizonte.w);
	glUniform4f(uniformCieloSolido, cieloSolido.x, cieloSolido.y, cieloSolido.z, cieloSolido.w);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturas[static_cast<size_t>(fondo)]);
	//skyShader->Validate();
	skyMesh->RenderMeshModel();
	glDepthMask(true);

}

void Skybox::SetAmbiente(glm::vec3 tinte, glm::vec4 bruma)
{
	tinteCielo = tinte;
	brumaHorizonte = bruma;
}

void Skybox::SetCieloSolido(glm::vec3 cenit, float cantidad)
{
	cieloSolido = glm::vec4(cenit, cantidad);
}


Skybox::~Skybox()
{
	if (texturas[0]) glDeleteTextures(static_cast<GLsizei>(texturas.size()), texturas.data());
	delete skyMesh;
	delete skyShader;
}
