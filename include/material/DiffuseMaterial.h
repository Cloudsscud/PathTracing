#pragma once
#include<material/Material.h>

class DiffuseMaterial :public Material {
public:
	DiffuseMaterial(const glm::vec3& albedo) :m_albedo(albedo) {};
	glm::vec3 sampleBRDF(const glm::vec3& view_direction, glm::vec3& beta, const RNG& rng) const override;

private:
	glm::vec3 m_albedo{};
};