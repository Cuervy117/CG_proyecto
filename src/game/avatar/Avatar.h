#pragma once

#include <glm.hpp>

class Model;
class SceneNode;

namespace cgame
{
// SceneNode owns the instance; Model is borrowed and must outlive the scene.
class Avatar
{
public:
    bool attach(SceneNode& parent, Model& model,
        const glm::vec3& spawn = glm::vec3(0.0f), float modelScale = 0.75f);

    void setPosition(const glm::vec3& position);
    void update(float dt, const glm::vec3& direction, float speed);

    SceneNode* root() const;

private:
    SceneNode* m_root = nullptr;
    float m_baseY = 0.0f;
    float m_walkPhase = 0.0f;
};
}