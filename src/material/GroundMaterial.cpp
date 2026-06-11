#include <material/GroundMaterial.h>
#include <sample/Spherical.h>

std::optional<BSDFSample> GroundMaterial::sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const {
	glm::vec3 light_direction = CosineSampleHemisphere({ rng.uniform(), rng.uniform() });
	glm::vec3 bsdf = m_albedo / PI;
	float pdf = CosineSampleHemispherePDF(light_direction);

	if (
		(static_cast<int>(glm::floor(hit_point.x * 8)) % 8 == 0) ||
		(static_cast<int>(glm::floor(hit_point.z * 8)) % 8 == 0)
		) {
		bsdf *= 0.1;
	}
	return BSDFSample{ bsdf, pdf, light_direction };
}