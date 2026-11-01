#include "Avatar.h"

#include "Model.h"
#include "SceneNode.h"

#include <cmath>
#include <memory>

namespace cgame
{
bool Avatar::attach(SceneNode& parent, Model& model,
    const glm::vec3& spawn, float modelScale)
{
    if (m_root != nullptr || model.getMeshCount() <= 0 || modelScale <= 0.0f)
        return false;

    auto node = std::make_unique<SceneNode>("c.avatar.reksai");
    node->position = spawn;
    node->scale = glm::vec3(modelScale);
    node->setModel(&model);

    m_baseY = spawn.y;
    m_root = parent.add(std::move(node));
    return m_root != nullptr;
}

void Avatar::setPosition(const glm::vec3& position)
{
    if (m_root == nullptr)
        return;
    m_root->position = position;
    m_baseY = position.y;
}

void Avatar::update(float dt, const glm::vec3& direction, float speed)
{
    if (m_root == nullptr)
        return;

    const float safeDt = dt < 0.0f ? 0.0f : (dt > 0.05f ? 0.05f : dt);
    const glm::vec2 flatDirection(direction.x, direction.z);
    const float directionLength2 = glm::dot(flatDirection, flatDirection);
    if (directionLength2 > 0.0001f)
        m_root->rotationEuler.y = std::atan2(flatDirection.x, flatDirection.y);

    if (speed > 0.05f && directionLength2 > 0.0001f)
    {
        m_walkPhase = std::fmod(m_walkPhase + safeDt * (5.0f + speed), 6.2831853f);
        m_root->position.y = m_baseY + std::abs(std::sin(m_walkPhase)) * 0.035f;
    }
    else
    {
        m_walkPhase = 0.0f;
        m_root->position.y = m_baseY;
    }
}

SceneNode* Avatar::root() const
{
    return m_root;
}
}