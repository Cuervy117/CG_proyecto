#pragma once

#include "CommonValues.h"
#include <vector>
#include <string>
#include<glew.h>
#include<glm.hpp>
#include<glfw3.h>
#include <gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include "Mesh_tn.h"
#include "Shader_light.h"
class Skybox
{
public:
	Skybox();
	Skybox(std::vector<std::string> faceLocations);
	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);
	// Ambientacion del cielo: 'tinte' multiplica el cubemap y 'bruma' (rgb = color, a = intensidad)
	// pinta el horizonte. Con tinte (1,1,1) y bruma.a = 0 el skybox se ve como siempre.
	void SetAmbiente(const glm::vec3& tinte, const glm::vec4& bruma);
	// Cielo despejado: degradado del color de la bruma (horizonte) a 'cenit' (arriba). 'cantidad' = 1
	// lo reemplaza por completo; 0 deja el cubemap normal.
	void SetCieloSolido(const glm::vec3& cenit, float cantidad);
		
	~Skybox();
private:
	MeshModel* skyMesh;
	Shader* skyShader;
	GLuint textureId;
	GLuint uniformProjection, uniformView;
	GLuint uniformTinte, uniformBruma, uniformCieloSolido;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;
	glm::vec4 cieloSolido;

};

