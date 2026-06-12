#pragma once
#include <material/Material.h>

class ConductorMaterial : public Material {
public:
	ConductorMaterial(const glm::vec3& ior, const glm::vec3& k)
		:m_ior(ior),m_k(k)
	{}

	std::optional<BSDFSample> sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const override;

private:
	glm::vec3 m_ior, m_k;	// ÕÛÉäÂÊ, ÎüÊÕÂÊ
};