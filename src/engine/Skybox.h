#pragma once

#include "CommonValues.h"
#include <array>
#include <string>
#include<glew.h>
#include<glm.hpp>
#include<glfw3.h>
#include <gtc\matrix_transform.hpp>
#include<gtc\type_ptr.hpp>
#include "Mesh_tn.h"
#include "Shader_light.h"
enum class Fondo { Espacio, Roca, Selva };
using CarasCielo = std::array<std::string, 6>;
class Skybox
{
public:
	Skybox();
	Skybox(const Skybox&) = delete;
	Skybox& operator=(const Skybox&) = delete;
	bool Inicializar(const std::array<CarasCielo, 3>& cielos);
	void DrawSkybox(Fondo fondo, glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

	// El ciclo día/noche modula el cubemap elegido para la zona:
	//  - tinte: multiplica el color del cubemap (verdoso en la selva, rojizo en el desierto)
	//  - bruma: rgb = color de la bruma del horizonte, a = cuánta bruma (0 = nada)
	// Con tinte (1,1,1) y bruma.a = 0 el skybox se ve igual que en la práctica (en el espacio).
	void SetAmbiente(glm::vec3 tinte, glm::vec4 bruma);
	// NUEVO: cielo despejado del planeta océano: degradado del color de la bruma (horizonte) al
	// color 'cenit' (arriba). cantidad = 1 lo reemplaza por completo; 0 deja el cubemap normal.
	void SetCieloSolido(glm::vec3 cenit, float cantidad);
		
	~Skybox();
private:
	MeshModel* skyMesh;
	Shader* skyShader;
	std::array<GLuint, 3> texturas{};
	GLuint uniformProjection, uniformView;
	// NUEVO: uniforms de skybox.frag para la ambientación
	GLuint uniformTinte, uniformBruma, uniformCieloSolido;
	glm::vec3 tinteCielo;
	glm::vec4 brumaHorizonte;
	glm::vec4 cieloSolido;

};
