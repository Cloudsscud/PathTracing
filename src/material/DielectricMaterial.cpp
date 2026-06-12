#include <material/DielectricMaterial.h>

float fresnell(float etai_div_etat, float cos_theta_t, float& cos_theta_i){
	float sin2_theta_t = 1 - cos_theta_t * cos_theta_t;
	float sin2_theta_i = sin2_theta_t / (etai_div_etat * etai_div_etat);

	if (sin2_theta_i >= 1) {
		// 全反射
		return 1;
	}

	cos_theta_i = glm::sqrt(1 - sin2_theta_i);
	float r_pal = (cos_theta_i - etai_div_etat * cos_theta_t) / (cos_theta_i + etai_div_etat * cos_theta_t);
	float r_perp = (etai_div_etat * cos_theta_i -  cos_theta_t) / (etai_div_etat * cos_theta_i + cos_theta_t);
	return 0.5f * (r_pal * r_pal + r_perp * r_perp);
}

std::optional<BSDFSample> DielectricMaterial::sampleBSDF(const glm::vec3& hit_point, const glm::vec3& view_direction, const RNG& rng) const{
	float etai_div_etat = m_ior;
	glm::vec3 normal = glm::vec3{ 0,1,0 };
	float cos_theta_t = view_direction.y;
	if (cos_theta_t < 0) {
		etai_div_etat = 1.f / m_ior;
		normal = { 0,-1,0 };	// 法线与观察方向在同一个半球
		cos_theta_t = -cos_theta_t;
	}

	float cos_theta_i;
	float fr = fresnell(etai_div_etat, cos_theta_t, cos_theta_i);

	if (rng.uniform() <= fr) {
		// 反射
		glm::vec3 light_direction{ -view_direction.x, view_direction.y, -view_direction.z };
		return BSDFSample{m_albedo_r/glm::abs(light_direction.y), 1, light_direction};
	}
	else {
		// 透射
		glm::vec3 light_direction{ (-view_direction / etai_div_etat) + (cos_theta_t / etai_div_etat - cos_theta_i) * normal };
		return BSDFSample{ m_albedo_t/(etai_div_etat* etai_div_etat)/ glm::abs(view_direction.y), 1, light_direction};
	}
}