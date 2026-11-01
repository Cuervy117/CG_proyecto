#pragma once

#include <array>
#include <glm.hpp>
#include <memory>
#include <string>
#include <vector>

class MeshModel;
class SceneNode;
class Texture;

namespace cgame
{
// Avatar owns the articulation meshes and texture. Keep it alive while the
// Scene containing its nodes can still render.
class Avatar
{
public:
    bool attach(SceneNode& parent,
        const std::string& objPath = "Models/reksai.obj",
        const std::string& texturePath = "Textures/reksai.png",
        const glm::vec3& spawn = glm::vec3(0.0f),
        float modelScale = 0.75f);

    void setPosition(const glm::vec3& position);
    void update(float dt, const glm::vec3& direction, float speed);

    SceneNode* root() const;

private:
    SceneNode* m_root = nullptr;
    SceneNode* m_bodyPivot = nullptr;
    SceneNode* m_headPivot = nullptr;
    SceneNode* m_tailPivot = nullptr;
    std::array<SceneNode*, 4> m_legPivots{};
    glm::vec3 m_bodyBasePosition{0.0f};
    float m_walkPhase = 0.0f;
    std::unique_ptr<Texture> m_texture;
    std::vector<std::unique_ptr<MeshModel>> m_meshes;
};
}