#pragma once
#include "shape/Shape.h"

class Plane : public Shape {
public:
	glm::vec3 m_point;
	glm::vec3 m_normal;

	Plane(const glm::vec3& point, const glm::vec3& normal)
		:m_point(point), m_normal(normal) {
	}

	std::optional<HitInfo> intersect(
		const Ray& ray,
		float tmin = 1e-5,
		float tmax = std::numeric_limits<float>::infinity()
	) const override;
};