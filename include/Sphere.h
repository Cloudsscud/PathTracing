#pragma once

#include "Shape.h"

struct Sphere : public Shape {
	Sphere(glm::vec3 center, float r):m_center(center), m_radius(r) {}

	glm::vec3 m_center;
	float m_radius;

	std::optional<HitInfo> intersect(
		const Ray& ray,
		float tmin = 1e-5,
		float tmax = std::numeric_limits<float>::infinity()
	) const override;
};