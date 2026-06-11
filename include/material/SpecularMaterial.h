#pragma once
#include <material/Material.h>

class SpecularMaterial :public Material {
public:
	SpecularMaterial(const glm::vec3& albedo) :m_albedo(albedo) {};
	std::optional<BSDFSample> sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const override;

private:
	glm::vec3 m_albedo{};
};