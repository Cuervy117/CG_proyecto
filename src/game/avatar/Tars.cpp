#include "Tars.h"

#include "Mesh_tn.h"
#include "SceneNode.h"

#include <array>
#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace cgame
{
namespace
{
constexpr float kPi = 3.14159265358979323846f;
constexpr float kBarLength = 3.35f;
constexpr float kBarWidth = 0.52f;
constexpr float kBarDepth = 0.34f;

struct BoxGeometry
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
};

void appendFace(BoxGeometry& geometry, const std::array<glm::vec3, 4>& points,
	const glm::vec3& normal)
{
	const unsigned int base = static_cast<unsigned int>(geometry.vertices.size() / 8);
	const glm::vec2 uv[4] = {
		glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f),
		glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f)
	};
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		geometry.vertices.insert(geometry.vertices.end(), {
			points[i].x, points[i].y, points[i].z, uv[i].x, uv[i].y,
			-normal.x, -normal.y, -normal.z
		});
	}
	geometry.indices.insert(geometry.indices.end(), {
		base, base + 1, base + 2, base, base + 2, base + 3
	});
}

BoxGeometry makeBox(float width, float height, float depth,
	const glm::vec3& center = glm::vec3(0.0f))
{
	const float x = width * 0.5f;
	const float y = height * 0.5f;
	const float z = depth * 0.5f;
	const glm::vec3 p000 = center + glm::vec3(-x, -y, -z);
	const glm::vec3 p001 = center + glm::vec3(-x, -y, z);
	const glm::vec3 p010 = center + glm::vec3(-x, y, -z);
	const glm::vec3 p011 = center + glm::vec3(-x, y, z);
	const glm::vec3 p100 = center + glm::vec3(x, -y, -z);
	const glm::vec3 p101 = center + glm::vec3(x, -y, z);
	const glm::vec3 p110 = center + glm::vec3(x, y, -z);
	const glm::vec3 p111 = center + glm::vec3(x, y, z);

	BoxGeometry geometry;
	appendFace(geometry, { p001, p101, p111, p011 }, glm::vec3(0, 0, 1));
	appendFace(geometry, { p100, p000, p010, p110 }, glm::vec3(0, 0, -1));
	appendFace(geometry, { p000, p001, p011, p010 }, glm::vec3(-1, 0, 0));
	appendFace(geometry, { p101, p100, p110, p111 }, glm::vec3(1, 0, 0));
	appendFace(geometry, { p010, p011, p111, p110 }, glm::vec3(0, 1, 0));
	appendFace(geometry, { p000, p100, p101, p001 }, glm::vec3(0, -1, 0));
	return geometry;
}

std::unique_ptr<MeshModel> makeMesh(BoxGeometry geometry)
{
	auto mesh = std::make_unique<MeshModel>();
	mesh->CreateMeshModel(geometry.vertices.data(), geometry.indices.data(),
		static_cast<unsigned int>(geometry.vertices.size()),
		static_cast<unsigned int>(geometry.indices.size()));
	return mesh;
}
}

bool Tars::attach(SceneNode& parent, const glm::vec3& spawn, float modelScale)
{
	if (m_root != nullptr || modelScale <= 0.0f)
		return false;

	auto rootNode = std::make_unique<SceneNode>("c.avatar.tars");
	rootNode->position = spawn;
	rootNode->scale = glm::vec3(modelScale);
	SceneNode* root = rootNode.get();

	// TARS stands as four tall, neighboring panels. The outer panels lean only
	// a little; they should read as a compact robot, not as crossed beams.
	const float baseAngles[4] = { -0.075f, -0.018f, 0.018f, 0.075f };
	const glm::vec3 barPositions[4] = {
		glm::vec3(-0.81f, 0.0f, 0.0f), glm::vec3(-0.27f, 0.0f, 0.0f),
		glm::vec3(0.27f, 0.0f, 0.0f), glm::vec3(0.81f, 0.0f, 0.0f)
	};
	const glm::vec3 barTints[4] = {
		glm::vec3(0.68f, 0.71f, 0.72f), glm::vec3(0.82f, 0.83f, 0.81f),
		glm::vec3(0.77f, 0.79f, 0.78f), glm::vec3(0.60f, 0.64f, 0.66f)
	};
	const float face = -kBarDepth * 0.5f - 0.018f;
	for (std::size_t i = 0; i < m_segments.size(); ++i)
	{
		auto bar = std::make_unique<SceneNode>("c.avatar.tars.bar." + std::to_string(i + 1));
		bar->position = barPositions[i];
		bar->rotationEuler.z = baseAngles[i];
		bar->tint = barTints[i];
		bar->specular = 0.72f;
		bar->shininess = 42.0f;
		auto barMesh = makeMesh(makeBox(kBarWidth, kBarLength, kBarDepth));
		bar->setMesh(barMesh.get());
		m_segments[i] = root->add(std::move(bar));
		m_meshes.push_back(std::move(barMesh));

		const auto addDetail = [&](const std::string& name, float width, float height,
			float depth, const glm::vec3& position, const glm::vec3& tint)
		{
			auto detailMesh = makeMesh(makeBox(width, height, depth));
			auto detail = std::make_unique<SceneNode>(name);
			detail->position = position;
			detail->tint = tint;
			detail->specular = 0.18f;
			detail->setMesh(detailMesh.get());
			m_segments[i]->add(std::move(detail));
			m_meshes.push_back(std::move(detailMesh));
		};
		// Dark upper display and lower end band from the real TARS silhouette.
		addDetail("c.avatar.tars.bar.display", kBarWidth * 0.78f, 0.62f, 0.045f,
			glm::vec3(0.0f, 0.62f, face), glm::vec3(0.035f, 0.045f, 0.052f));
		addDetail("c.avatar.tars.bar.lower.band", kBarWidth * 0.82f, 0.34f, 0.045f,
			glm::vec3(0.0f, -1.28f, face), glm::vec3(0.055f, 0.065f, 0.073f));
		addDetail("c.avatar.tars.bar.rail.left", 0.035f, 3.05f, 0.035f,
			glm::vec3(-0.225f, 0.0f, face - 0.018f), glm::vec3(0.32f, 0.35f, 0.36f));
		addDetail("c.avatar.tars.bar.rail.right", 0.035f, 3.05f, 0.035f,
			glm::vec3(0.225f, 0.0f, face - 0.018f), glm::vec3(0.32f, 0.35f, 0.36f));
		for (int seam = 0; seam < 4; ++seam)
		{
			const float y = -0.78f + 0.48f * static_cast<float>(seam);
			addDetail("c.avatar.tars.bar.panel.seam", kBarWidth * 0.92f, 0.028f, 0.028f,
				glm::vec3(0.0f, y, face - 0.012f), glm::vec3(0.35f, 0.38f, 0.39f));
		}
	}

	// A cross-axis hinge joins the four panels. Small metal pivot caps mark the
	// articulation points without hiding the panel fronts.
	auto hubMesh = makeMesh(makeBox(1.88f, 0.12f, 0.20f,
		glm::vec3(0.0f, 0.0f, 0.04f)));
	auto hub = std::make_unique<SceneNode>("c.avatar.tars.central.hub");
	hub->tint = glm::vec3(0.42f, 0.45f, 0.46f);
	hub->specular = 0.5f;
	hub->setMesh(hubMesh.get());
	root->add(std::move(hub));
	m_meshes.push_back(std::move(hubMesh));

	for (int i = 0; i < 4; ++i)
	{
		auto pivotMesh = makeMesh(makeBox(0.10f, 0.10f, 0.08f));
		auto pivot = std::make_unique<SceneNode>("c.avatar.tars.pivot." + std::to_string(i));
		pivot->position = glm::vec3(barPositions[i].x, -0.02f, -kBarDepth * 0.5f - 0.055f);
		pivot->tint = glm::vec3(0.30f, 0.33f, 0.34f);
		pivot->specular = 0.8f;
		pivot->setMesh(pivotMesh.get());
		root->add(std::move(pivot));
		m_meshes.push_back(std::move(pivotMesh));
	}

	m_groundOffset = kBarLength * 0.5f * modelScale;
	m_basePosition = spawn;
	m_root = parent.add(std::move(rootNode));
	return m_root != nullptr;
}

void Tars::setPosition(const glm::vec3& position)
{
	m_basePosition = position;
	if (m_root != nullptr)
		m_root->position = position + glm::vec3(0.0f, m_groundOffset, 0.0f);
}

void Tars::update(float dt, const glm::vec3&, float speed)
{
	if (m_root == nullptr)
		return;
	const float safeDt = dt < 0.0f ? 0.0f : (dt > 0.05f ? 0.05f : dt);
	const bool walking = speed > 0.05f;
	m_phase = std::fmod(m_phase + safeDt * (walking ? 2.2f : 0.55f), 2.0f * kPi);
	const float gait = walking ? 0.20f : 0.008f;
	const float baseAngles[4] = { -0.075f, -0.018f, 0.018f, 0.075f };
	for (std::size_t i = 0; i < m_segments.size(); ++i)
	{
		// Los dos paneles exteriores avanzan juntos; los dos interiores hacen
		// el paso contrario. En la siguiente mitad del ciclo intercambian el turno.
		const float phase = m_phase + ((i == 0 || i == 3) ? 0.0f : kPi);
		m_segments[i]->rotationEuler.z = baseAngles[i] + std::sin(phase) * gait * 0.32f;
		m_segments[i]->rotationEuler.x = std::cos(phase) * gait;
	}
	m_root->position = m_basePosition + glm::vec3(0.0f,
		m_groundOffset + std::abs(std::sin(m_phase * 2.0f)) * (walking ? 0.035f : 0.012f), 0.0f);
}

SceneNode* Tars::root() const
{
	return m_root;
}
}
