#include "Scene.h"

#include "Mesh_tn.h"
#include "Model.h"
#include "Shader_light.h"
#include "Texture.h"

#include <gtc/type_ptr.hpp>

#include <cstdio>

namespace
{
void renderNodo(const SceneNode& nodo, Texture* texturaRespaldo,
	GLuint uniformModel, GLuint uniformColor,
	GLuint uniformSpecular, GLuint uniformShininess,
	bool& avisoMallaSinTextura)
{
	if (!nodo.visible)
		return;

	Model* modelo = nodo.model();
	MeshModel* malla = nodo.mesh();
	if (modelo != nullptr || malla != nullptr)
	{
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(nodo.world));
		glUniform3fv(uniformColor, 1, glm::value_ptr(nodo.tint));
		glUniform1f(uniformSpecular, nodo.specular);
		glUniform1f(uniformShininess, nodo.shininess);

		if (modelo != nullptr)
		{
			modelo->RenderModel();
		}
		else
		{
			Texture* textura = (nodo.texture() != nullptr) ? nodo.texture() : texturaRespaldo;
			if (textura != nullptr)
			{
				textura->UseTexture();
				malla->RenderMeshModel();
			}
			else if (!avisoMallaSinTextura)
			{
				printf("Aviso: malla sin textura omitida (%s)\n", nodo.name().c_str());
				avisoMallaSinTextura = true;
			}
		}
	}

	for (const std::unique_ptr<SceneNode>& hijo : nodo.children())
	{
		renderNodo(*hijo, texturaRespaldo, uniformModel, uniformColor,
			uniformSpecular, uniformShininess, avisoMallaSinTextura);
	}
}
}

void Scene::updateWorld()
{
	root.updateWorld();
}

SceneNode* Scene::find(const std::string& nodeName)
{
	return root.find(nodeName);
}

const SceneNode* Scene::find(const std::string& nodeName) const
{
	return root.find(nodeName);
}

void Scene::setFallbackTexture(Texture* texture)
{
	m_fallbackTexture = texture;
}

Texture* Scene::fallbackTexture() const
{
	return m_fallbackTexture;
}

void Scene::render(Shader& shader, const glm::mat4& view, const glm::mat4& projection)
{
	shader.UseShader();

	const GLuint uniformModel = shader.GetModelLocation();
	const GLuint uniformColor = shader.getColorLocation();
	const GLuint uniformSpecular = shader.GetSpecularIntensityLocation();
	const GLuint uniformShininess = shader.GetShininessLocation();

	glUniformMatrix4fv(shader.GetViewLocation(), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(shader.GetProjectionLocation(), 1, GL_FALSE, glm::value_ptr(projection));

	bool aviso = m_warnedMissingTexture;
	renderNodo(root, m_fallbackTexture, uniformModel, uniformColor,
		uniformSpecular, uniformShininess, aviso);
	m_warnedMissingTexture = aviso;
}
