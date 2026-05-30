#include "Triangle.h"

std::optional<HitInfo> Triangle::intersect(const Ray& ray, float tmin, float tmax) const {
	glm::vec3 e1 = m_p1 - m_p0;
	glm::vec3 e2 = m_p2 - m_p0;
	glm::vec3 s1 = glm::cross(ray.m_direction, e2);

	float inv_det = 1.f / glm::dot(s1, e1);

	glm::vec3 s = ray.m_origin - m_p0;
	float b1 = inv_det * glm::dot(s1, s);
	if (b1 < 0 || b1 > 1) {
		return {};
	}

	glm::vec3 s2 = glm::cross(s, e1);
	float b2 = inv_det * glm::dot(s2, ray.m_direction);
	if (b2 < 0 || b1 + b2 > 1) {
		return {};
	}

	float t = inv_det * glm::dot(s2, e2);
	if (t > tmin && t < tmax) {
		glm::vec3 pos = ray.hit(t);
		glm::vec3 normal = (1.f - b1 - b2) * m_n0 + b1 * m_n1 + b2 * m_n2;
		return HitInfo{ t, pos, normal };
	}
	return {};
}