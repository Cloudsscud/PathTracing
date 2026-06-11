#pragma once

#include <glm/glm.hpp>
#include <util/RNG.h>
#include <optional>

struct BSDFSample {
	glm::vec3 m_bsdf;
	float m_pdf;
	glm::vec3 m_light_direction;
};

class Material {
public:
	virtual std::optional<BSDFSample> sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const = 0;
	void setEmissive(const glm::vec3& emissive) { this->m_emissive = emissive; }
public:
	glm::vec3 m_emissive{};
};