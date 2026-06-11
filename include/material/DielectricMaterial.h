#pragma once
#include <material/Material.h>

class DielectricMaterial : public Material {
public:
	DielectricMaterial(float ior, const glm::vec3& albedo)
		:m_ior(ior), m_albedo_r(albedo), m_albedo_t(albedo)
	{
	}

	DielectricMaterial(float ior, const glm::vec3& albedo_r, const glm::vec3& albedo_t)
		:m_ior(ior), m_albedo_r(albedo_r), m_albedo_t(albedo_t)
	{
	}

	glm::vec3 sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, glm::vec3& beta, const RNG& rng) const override;

private:
	float m_ior;	// 折射率
	glm::vec3 m_albedo_r, m_albedo_t;	// 反射r与投射t的反照率
};