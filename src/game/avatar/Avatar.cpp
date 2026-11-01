#include "Avatar.h"

#include "Mesh_tn.h"
#include "SceneNode.h"
#include "Texture.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cgame
{
namespace
{
enum PartId
{
    Body = 0,
    Head,
    Tail,
    RightFrontLeg,
    LeftFrontLeg,
    RightBackLeg,
    LeftBackLeg,
    PartCount
};

struct PartGeometry
{
    std::vector<GLfloat> vertices;
    std::vector<unsigned int> indices;
    std::unordered_map<std::string, unsigned int> vertexLookup;
    glm::vec3 positionSum{0.0f};
    unsigned int positionCount = 0;
    glm::vec3 pivot{0.0f};
};

bool contains(const std::string& text, const char* part)
{
    return text.find(part) != std::string::npos;
}

PartId classifyGroup(const std::string& group)
{
    if (contains(group, "Tail"))
        return Tail;
    if (group == "Head" || group == "Neck" || contains(group, "Jaw") ||
        contains(group, "Tongue"))
        return Head;

    const bool front = contains(group, "_Front_");
    const bool back = contains(group, "_Back_");
    const bool legPart = contains(group, "Foot") || contains(group, "Knee") ||
        contains(group, "Hip");
    if ((front || back) && legPart)
    {
        const bool left = group.rfind("L_", 0) == 0;
        if (front)
            return left ? LeftFrontLeg : RightFrontLeg;
        return left ? LeftBackLeg : RightBackLeg;
    }
    return Body;
}

bool resolveIndex(const std::string& text, std::size_t count, std::size_t& result)
{
    if (text.empty())
        return false;
    try
    {
        const int raw = std::stoi(text);
        const long long resolved = raw > 0 ? static_cast<long long>(raw - 1)
            : static_cast<long long>(count) + raw;
        if (resolved < 0 || resolved >= static_cast<long long>(count))
            return false;
        result = static_cast<std::size_t>(resolved);
        return true;
    }
    catch (...)
    {
        return false;
    }
}

bool appendVertex(PartGeometry& part, const std::string& token,
    const std::vector<glm::vec3>& positions,
    const std::vector<glm::vec2>& texcoords,
    const std::vector<glm::vec3>& normals,
    unsigned int& result)
{
    const auto existing = part.vertexLookup.find(token);
    if (existing != part.vertexLookup.end())
    {
        result = existing->second;
        return true;
    }

    std::string fields[3];
    std::size_t begin = 0;
    for (int field = 0; field < 2; ++field)
    {
        const std::size_t slash = token.find('/', begin);
        if (slash == std::string::npos)
        {
            fields[field] = token.substr(begin);
            begin = token.size();
            break;
        }
        fields[field] = token.substr(begin, slash - begin);
        begin = slash + 1;
    }
    if (begin < token.size())
        fields[2] = token.substr(begin);

    std::size_t positionIndex = 0;
    if (!resolveIndex(fields[0], positions.size(), positionIndex))
        return false;
    glm::vec2 uv(0.0f);
    if (!fields[1].empty())
    {
        std::size_t uvIndex = 0;
        if (!resolveIndex(fields[1], texcoords.size(), uvIndex))
            return false;
        uv = texcoords[uvIndex];
    }
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    if (!fields[2].empty())
    {
        std::size_t normalIndex = 0;
        if (!resolveIndex(fields[2], normals.size(), normalIndex))
            return false;
        normal = normals[normalIndex];
    }

    const glm::vec3 position = positions[positionIndex];
    result = static_cast<unsigned int>(part.vertices.size() / 8);
    part.vertexLookup.emplace(token, result);
    part.vertices.insert(part.vertices.end(), {
        position.x, position.y, position.z, uv.x, uv.y,
        -normal.x, -normal.y, -normal.z
    });
    part.positionSum += position;
    ++part.positionCount;
    return true;
}

bool loadObjParts(const std::string& path, std::array<PartGeometry, PartCount>& parts)
{
    std::ifstream input(path);
    if (!input)
    {
        std::printf("No se pudo abrir el OBJ de Rek'Sai: %s\n", path.c_str());
        return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texcoords;
    std::vector<glm::vec3> normals;
    PartId activePart = Body;
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream row(line);
        std::string kind;
        row >> kind;
        if (kind == "v")
        {
            glm::vec3 value;
            if (row >> value.x >> value.y >> value.z)
                positions.push_back(value);
        }
        else if (kind == "vt")
        {
            glm::vec2 value;
            if (row >> value.x >> value.y)
                texcoords.push_back(value);
        }
        else if (kind == "vn")
        {
            glm::vec3 value;
            if (row >> value.x >> value.y >> value.z)
                normals.push_back(value);
        }
        else if (kind == "g" || kind == "o")
        {
            std::string group;
            row >> group;
            activePart = classifyGroup(group);
        }
        else if (kind == "f")
        {
            std::vector<unsigned int> face;
            std::string token;
            bool valid = true;
            while (row >> token)
            {
                if (!token.empty() && token[0] == '#')
                    break;
                unsigned int vertexIndex = 0;
                if (!appendVertex(parts[activePart], token, positions, texcoords,
                    normals, vertexIndex))
                {
                    valid = false;
                    break;
                }
                face.push_back(vertexIndex);
            }
            if (!valid || face.size() < 3)
                return false;
            for (std::size_t i = 1; i + 1 < face.size(); ++i)
            {
                parts[activePart].indices.push_back(face[0]);
                parts[activePart].indices.push_back(face[i]);
                parts[activePart].indices.push_back(face[i + 1]);
            }
        }
    }

    for (PartGeometry& part : parts)
    {
        if (part.indices.empty() || part.positionCount == 0)
            continue;
        part.pivot = part.positionSum / static_cast<float>(part.positionCount);
        for (std::size_t i = 0; i < part.vertices.size(); i += 8)
        {
            part.vertices[i] -= part.pivot.x;
            part.vertices[i + 1] -= part.pivot.y;
            part.vertices[i + 2] -= part.pivot.z;
        }
    }

    for (PartId required : { Body, Head, Tail, RightFrontLeg,
        LeftFrontLeg, RightBackLeg, LeftBackLeg })
    {
        if (parts[required].indices.empty())
        {
            std::printf("El OBJ de Rek'Sai no contiene geometria para la parte %d\n",
                static_cast<int>(required));
            return false;
        }
    }
    return true;
}
}

bool Avatar::attach(SceneNode& parent, const std::string& objPath,
    const std::string& texturePath, const glm::vec3& spawn, float modelScale)
{
    if (m_root != nullptr || modelScale <= 0.0f)
        return false;

    std::array<PartGeometry, PartCount> parts;
    if (!loadObjParts(objPath, parts))
        return false;

    auto texture = std::make_unique<Texture>(texturePath.c_str());
    if (!texture->LoadTextureA())
        return false;

    std::vector<std::unique_ptr<MeshModel>> meshes;
    auto rootNode = std::make_unique<SceneNode>("c.avatar.reksai");
    rootNode->position = spawn;
    rootNode->scale = glm::vec3(modelScale);
    SceneNode* partNodes[PartCount] = {};
    const char* names[PartCount] = {
        "c.avatar.reksai.body", "c.avatar.reksai.head", "c.avatar.reksai.tail",
        "c.avatar.reksai.leg.right_front", "c.avatar.reksai.leg.left_front",
        "c.avatar.reksai.leg.right_back", "c.avatar.reksai.leg.left_back"
    };

    for (int i = 0; i < PartCount; ++i)
    {
        PartGeometry& part = parts[static_cast<std::size_t>(i)];
        auto mesh = std::make_unique<MeshModel>();
        mesh->CreateMeshModel(part.vertices.data(), part.indices.data(),
            static_cast<unsigned int>(part.vertices.size() / 8),
            static_cast<unsigned int>(part.indices.size()));

        auto node = std::make_unique<SceneNode>(names[i]);
        node->position = part.pivot;
        node->setMesh(mesh.get());
        node->setTexture(texture.get());
        partNodes[i] = rootNode->add(std::move(node));
        meshes.push_back(std::move(mesh));
    }

    m_bodyPivot = partNodes[Body];
    m_headPivot = partNodes[Head];
    m_tailPivot = partNodes[Tail];
    m_legPivots = { partNodes[RightFrontLeg], partNodes[LeftFrontLeg],
        partNodes[RightBackLeg], partNodes[LeftBackLeg] };
    m_bodyBasePosition = m_bodyPivot->position;
    m_meshes = std::move(meshes);
    m_texture = std::move(texture);
    m_root = parent.add(std::move(rootNode));
    return m_root != nullptr;
}

void Avatar::setPosition(const glm::vec3& position)
{
    if (m_root != nullptr)
        m_root->position = position;
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
        m_walkPhase = std::fmod(m_walkPhase + safeDt * (4.0f + speed * 2.0f), 6.2831853f);
        const float stride = std::sin(m_walkPhase) * 0.52f;
        const float oppositeStride = std::sin(m_walkPhase + 3.1415926f) * 0.52f;
        m_legPivots[0]->rotationEuler.x = stride;
        m_legPivots[3]->rotationEuler.x = stride;
        m_legPivots[1]->rotationEuler.x = oppositeStride;
        m_legPivots[2]->rotationEuler.x = oppositeStride;
        m_bodyPivot->position.y = m_bodyBasePosition.y +
            std::abs(std::sin(m_walkPhase * 2.0f)) * 0.035f;
        m_headPivot->rotationEuler.x = std::sin(m_walkPhase * 2.0f) * 0.04f;
        m_tailPivot->rotationEuler.y = std::sin(m_walkPhase * 0.5f) * 0.22f;
    }
    else
    {
        m_walkPhase = 0.0f;
        for (SceneNode* leg : m_legPivots)
            leg->rotationEuler = glm::vec3(0.0f);
        m_bodyPivot->position = m_bodyBasePosition;
        m_headPivot->rotationEuler = glm::vec3(0.0f);
        m_tailPivot->rotationEuler = glm::vec3(0.0f);
    }
}

SceneNode* Avatar::root() const
{
    return m_root;
}
}