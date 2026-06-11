#include <material/DiffuseMaterial.h>
#include <sample/Spherical.h>

std::optional<BSDFSample> DiffuseMaterial::sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const {
	glm::vec3 light_direction = CosineSampleHemisphere({ rng.uniform(), rng.uniform() });
	glm::vec3 bsdf = m_albedo / PI;
	float pdf = CosineSampleHemispherePDF(light_direction);
	return BSDFSample{bsdf, pdf, light_direction};
}