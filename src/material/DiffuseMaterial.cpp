#include <material/DiffuseMaterial.h>
#include <sample/Spherical.h>

glm::vec3 DiffuseMaterial::sampleBRDF(const glm::vec3& view_direction, glm::vec3& beta, const RNG& rng) const {
	beta *= m_albedo;
	return UniformSampleHemisphere(rng);
}