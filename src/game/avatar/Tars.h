#pragma once

#include <array>
#include <glm.hpp>
#include <memory>
#include <vector>

class MeshModel;
class SceneNode;

namespace cgame
{
// Procedural, articulated TARS-inspired robot. Its geometry is made from
// hinged rectangular modules and uses the scene's white fallback texture.
class Tars
{
public:
	bool attach(SceneNode& parent, const glm::vec3& spawn = glm::vec3(0.0f),
		float modelScale = 1.0f);

	void setPosition(const glm::vec3& position);
	void update(float dt, const glm::vec3& direction, float speed);
	SceneNode* root() const;

private:
	SceneNode* m_root = nullptr;
	std::array<SceneNode*, 4> m_segments{};
	glm::vec3 m_basePosition{0.0f};
	float m_groundOffset = 0.0f;
	float m_phase = 0.0f;
	std::vector<std::unique_ptr<MeshModel>> m_meshes;
};
}
