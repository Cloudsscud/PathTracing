#include "Sphere.h"

std::optional<HitInfo> Sphere:: intersect(const Ray& ray, float tmin, float tmax) const
{
	glm::vec3 co = ray.m_origin - m_center;
	float a = glm::dot(ray.m_direction, ray.m_direction);
	float b = glm::dot(co, ray.m_direction) * 2;
	float c = glm::dot(co, co) - m_radius * m_radius;
	float delte = b * b - 4 * a * c;
	if (delte < 0) return {};
	float hit_t = (-b - std::sqrt(delte)) * 0.5 / a;
	if (hit_t < tmin) {
		hit_t = (-b + std::sqrt(delte)) * 0.5 / a;
	}
	if (hit_t > tmin && hit_t < tmax) {
		glm::vec3 hit_point = ray.hit(hit_t);
		glm::vec3 hit_normal = glm::normalize(hit_point - m_center);
		return HitInfo{ hit_t, hit_point, hit_normal };
	}
	return {};
}