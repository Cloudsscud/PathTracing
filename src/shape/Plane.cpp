#include "shape/Plane.h"

std::optional<HitInfo> Plane::intersect(const Ray& ray, float tmin, float tmax) const {
	glm::vec3 op = m_point - ray.m_origin;
	float invers = 1.f / glm::dot(m_normal, ray.m_direction);
	float hit_t = glm::dot(op, m_normal) * invers;
	if (hit_t > tmin && hit_t < tmax) {
		glm::vec3 hit_point = ray.hit(hit_t);
		return HitInfo{ hit_t, hit_point, m_normal };
	}
	return {};
}