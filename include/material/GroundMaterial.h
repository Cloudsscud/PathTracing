#pragma once
#include <material/Material.h>

class GroundMaterial :public Material {
public:
	GroundMaterial(const glm::vec3& albedo) :m_albedo(albedo) {};

	glm::vec3 sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, glm::vec3& beta, const RNG& rng) const override;
public:
	glm::vec3 m_albedo{};
};