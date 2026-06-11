#include <material/SpecularMaterial.h>

std::optional<BSDFSample> SpecularMaterial::sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const {
	glm::vec3 light_direction = { -view_direction.x, view_direction.y, -view_direction.z };
	glm::vec3 bsdf = m_albedo / glm::abs(view_direction.y);
	float pdf = 1;	// bsdf和pdf的delta函数省掉了
	return BSDFSample{bsdf, pdf, light_direction};
}