#pragma once
#include "shape/Shape.h"

struct Triangle : public Shape {
	glm::vec3 m_p0, m_p1, m_p2;	// ¶¥µã
	glm::vec3 m_n0, m_n1, m_n2;	// ·¨Ïß

	Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2,
			 const glm::vec3& n0, const glm::vec3& n1, const glm::vec3& n2)
	:m_p0(p0), m_p1(p1), m_p2(p2),
	 m_n0(n0), m_n1(n1), m_n2(n2) { }

	Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2)
		:m_p0(p0), m_p1(p1), m_p2(p2)
	{
		glm::vec3 v1 = p1 - p0;
		glm::vec3 v2 = p2 - p0;
		glm::vec3 normal = glm::normalize(glm::cross(v1, v2));
		m_n0 = normal;
		m_n1 = normal;
		m_n2 = normal;
	}

	std::optional<HitInfo> intersect(const Ray& ray, float tmin, float tmax) const override;

	BoundingBox getBounds() const override {
		BoundingBox box{};
		box.expand(m_p0);
		box.expand(m_p1);
		box.expand(m_p2);
		return box;
	}
};