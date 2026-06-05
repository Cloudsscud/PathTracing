#pragma once
#include "shape/Shape.h"

class Plane : public Shape {
public:
	glm::vec3 m_point;
	glm::vec3 m_normal;

	Plane(const glm::vec3& point, const glm::vec3& normal)
		:m_point(point), m_normal(glm::normalize(normal)) {
	}

	std::optional<HitInfo> intersect(const Ray& ray,float tmin,	float tmax) const override;
};