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
		
	~Skybox();
private:
	MeshModel* skyMesh;
	Shader* skyShader;
	GLuint textureId;
	GLuint uniformProjection, uniformView;
	GLuint uniformTinte, uniformBruma;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;

};

